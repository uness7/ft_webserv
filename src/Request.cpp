#include "../inc/Request.hpp"
#include <cstddef>
#include <cstring>
#include <sstream>
#include <sys/socket.h>
#include <vector>

Request::Request(ServerConfig config)
    : _contentLength(0), _config(config), _statusCode(0) {}

Request::~Request() {}

Request::Request(const Request &cp) { *this = cp; }

Request &Request::operator=(const Request &rhs) {
  if (this != &rhs) {
    this->_config = rhs._config;
    this->_path = rhs.getPath();
    this->_method = rhs.getMethod();
    this->_mimetype = rhs.getMimeType();
    this->_query = rhs._query;
    this->_body = rhs._body;
    this->_headers = rhs._headers;
    this->_contentLength = rhs._contentLength;
    this->_statusCode = rhs._statusCode;
  }
  return *this;
}

void Request::saveHeaderLine(std::string &line) {
  std::size_t found = line.find(":");

  if (found != std::string::npos) {
    std::string key = line.substr(0, found);
    std::string value = line.substr(found + 1);
    if (value[0] == ' ')
      value.erase(0, 1);
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    _headers[key] = value;
    if (key == "content-length")
      _contentLength = atoll(value.c_str());
  }
}

bool Request::isValidClientMaxBody() const {
  if (_target.client_max_body_size > 0)
    return _target.client_max_body_size > getContentLength();
  else if (_config.client_max_body_size > 0)
    return _config.client_max_body_size > getContentLength();
  return true;
}

Bytes Request::getNextChunk(unsigned int fd) const {
  char buffer[BUFFER_SIZE];
  int bytesRead = recv(fd, &buffer, BUFFER_SIZE, 0);
  if (bytesRead <= 0) {
    return Bytes();
  }
  return Bytes(buffer, buffer + bytesRead);
}

Bytes Request::seekCRLF(Bytes const &request, Bytes::size_type &index) {
  Bytes chunk;

  while (index < request.size()) {
    if (index + 1 < request.size() && request[index] == '\r' &&
        request[index + 1] == '\n') {
      index += 2;
      break;
    }
    chunk.push_back(request[index]);
    index++;
  }
  return (chunk);
}

bool Request::areHeadersValid() {
  std::string method = getMethod();
  if (method != "GET" && method != "POST" && method != "DELETE") {
    _statusCode = 501;
    return false;
  }
  if (_target.allowed_methods.size() &&
      std::find(_target.allowed_methods.begin(), _target.allowed_methods.end(),
                method) == _target.allowed_methods.end()) {
    _statusCode = 405;
    return false;
  }
  if (isValidClientMaxBody() == false) {
    _statusCode = 413;
    return false;
  }

  return true;
}

bool Request::canResolvePath() {

  short path_index =
      _config.getLocationByPathRequested(this->getPath(), this->_target);
  if (path_index == -1)
    return false;

  std::string end_s = getPath().substr(path_index);
  if (!_target.root.empty() &&
      _target.root.compare(1, _target.root.size() - 1, end_s, 0,
                           _target.root.size() - 1) == 0)
    return true;

  if (end_s.empty() && !_target.index.empty())
    setPath(_target.root + _target.index);
  else
    setPath(_target.root + end_s);
  return true;
}

long Request::read(unsigned int fd) {
  Bytes request;
  Bytes chunk;
  do {
    chunk = getNextChunk(fd);
    request.insert(request.end(), chunk.begin(), chunk.end());
  } while (chunk.size());

  size_t req_index = 0;
  std::stringstream line;
  Bytes binaryLine = seekCRLF(request, req_index);
  line << std::string(binaryLine.begin(), binaryLine.end());
  std::string token;
  for (int i = 0; i < 3 && std::getline(line, token, ' '); i++) {
    if (i == 0) {
      std::transform(token.begin(), token.end(), token.begin(), ::toupper);
      this->_method = token;
    } else if (i == 1)
      this->_path = token;
    else if (i == 2)
      this->_httpv = token;
  }

  if (canResolvePath() == false) {
    _statusCode = 404;
    return request.size();
  }

  while (req_index < request.size()) {
    Bytes binaryLine = seekCRLF(request, req_index);
    std::string headerLine(binaryLine.begin(), binaryLine.end());
    if (headerLine.empty())
      break;

    this->saveHeaderLine(headerLine);
  }

  if (areHeadersValid() == false)
    return request.size();

  if (this->getMethod() == "POST") {
    readBody(fd, request, req_index);
    if (_statusCode)
      return request.size();
    if (this->getHeaderField("transfer-encoding") == "chunked") {
      bool isChunk = false;

      while (req_index < request.size()) {
        Bytes binaryLine = seekCRLF(request, req_index);
        if (binaryLine.empty())
          break;
        if (isChunk)
          _body.insert(_body.end(), binaryLine.begin(), binaryLine.end());
        isChunk = !isChunk;
      }
    } else // form data
      _body = Bytes(request.begin() + req_index, request.end());
  }
  return request.size();
}

void Request::readBody(unsigned int fd, Bytes &request, size_t index) {
  Bytes chunk;
  int recv_time = 0;
  if (getHeaderField("expect") == "100-continue") {
    std::string toContinue = getHttpv() + " 100 Continue\r\n\r\n";
    send(fd, toContinue.c_str(), toContinue.size(), 0);
  }
  if (this->getContentLength() > 0) {
    while (static_cast<size_t>(getContentLength()) > request.size() - index) {
      if (recv_time > 1000) {
        _statusCode = 408;
        return;
      }
      chunk = getNextChunk(fd);
      if (chunk.empty()) {
        recv_time++;
        continue;
      }
      request.insert(request.end(), chunk.begin(), chunk.end());
    }
    return;
  }
  if (getHeaderField("expect") == "100-continue" &&
      getHeaderField("transfer-encoding") == "chunked") {
    std::string end = "\r\n0\r\n\r\n";
    while (true) {
      if (recv_time > 1000) {
        _statusCode = 408;
        break;
      }

      chunk = getNextChunk(fd);
      if (chunk.empty()) {
        recv_time++;
        continue;
      }

      request.insert(request.end(), chunk.begin(), chunk.end());

      std::string endRequest =
          std::string(request.end() - end.size(), request.end());

      if (endRequest == end)
        break;
    }
  }
}

void Request::setPath(std::string s) {
  std::size_t found = s.find("?");

  if (found != std::string::npos) {
    this->_path = s.substr(0, found);
    this->_query = s.substr(found + 1);
  } else
    this->_path = s;
  setMimeType();
}

void Request::setMimeType() {
  if (_path.length() >= 5 && _path.substr(_path.length() - 5) == ".html") {
    _mimetype = "text/html";
  } else if (_path.length() >= 4 &&
             _path.substr(_path.length() - 4) == ".css") {
    _mimetype = "text/css";
  } else if (_path.length() >= 4 &&
             _path.substr(_path.length() - 4) == ".jpg") {
    _mimetype = "image/jpg";
  } else if (_path.length() >= 3 && _path.substr(_path.length() - 3) == ".py") {
    _mimetype = "application/python";
  } else if (_path.length() >= 3 && _path.substr(_path.length() - 3) == ".js") {
    _mimetype = "application/javascript";
  } else if (_path.length() >= 4 &&
             _path.substr(_path.length() - 4) == ".mp4") {
    _mimetype = "video/mp4";
  } else {
    _mimetype = "text/plain";
  }
}

/* Getters Methods */

std::string Request::getMethod() const { return this->_method; }

long long Request::getContentLength() const { return this->_contentLength; }

std::string Request::getPath() const { return this->_path; }

std::string Request::getMimeType() const { return this->_mimetype; }

std::string Request::getQuery() const { return this->_query; }

std::string Request::getHttpv() const { return this->_httpv; }

short Request::getStatusCode() const { return this->_statusCode; }

LocationConfig Request::getPathLocation() const { return this->_target; }

Bytes &Request::getBody() { return this->_body; }

std::string Request::getHeaderField(std::string field) const {
  if (_headers.count(field)) {
    std::string target = _headers.at(field);
    return target;
  }
  return "";
}
