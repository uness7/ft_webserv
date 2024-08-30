#include "../inc/Request.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iterator>
#include <ostream>
#include <sstream>
#include <string>
#include <sys/_types/_size_t.h>
#include <vector>

Request::Request()
    : _method(""), _path(""), _query(""), _mimetype(""), _body(), _headers(),
      _data() {}
Request::Request(std::string entireRequest)
    : _method(""), _path(""), _query(""), _mimetype(""), _body(), _headers(),
      _data(entireRequest) {
  parseData();
}

Request::~Request() {}

Request::Request(const Request &cp) { *this = cp; }

Request &Request::operator=(const Request &rhs) {
  if (this != &rhs) {
    this->_path = rhs.getPath();
    this->_method = rhs.getMethod();
    this->_mimetype = rhs.getMimeType();
    this->_query = rhs._query;
    this->_body = rhs._body;
    this->_headers = rhs._headers;
  }
  return *this;
}

void Request::parseData(void) {
  std::stringstream requestStream(_data);
  std::string method;
  std::string path;
  std::string line;
  bool isBody = false;
  std::vector<char> data_bin;

  // printing stringstream
  std::string requestContent = requestStream.str();
  requestStream >> method;
  requestStream >> path;
  setPath(path);
  setMethod(method);
  std::getline(requestStream, line);
  std::vector<char> body;

  while (std::getline(requestStream, line)) {
    if (line.empty()) {
      isBody = true;
      continue;
    }
    if (isBody) {
      body.insert(body.end(), line.begin(), line.end());
      body.push_back('\n');
    } else {
      std::size_t found = line.find(":");
      if (found == std::string::npos) {
        isBody = true;
        continue;
      }
      std::string key = line.substr(0, found);
      std::string value = line.substr(found + 1);
      std::transform(key.begin(), key.end(), key.begin(), ::tolower);
      _headers.insert(std::make_pair(key, value));
    }
  }
  if (!body.empty() && body.back() == '\n')
    body.pop_back();
  if (!body.empty())
    _body = body;
}

void Request::handleFirstLineHeader(unsigned int socketFd) {
  std::ostringstream lineStream = Utils::get_next_line(socketFd);
  std::istringstream iss(lineStream.str());
  std::string method;
  std::string path;
  iss >> method >> path;
  setMethod(method);
  setPath(path);
}

void Request::saveHeaderLine(std::string &line) {
  std::size_t found = line.find(":");
  if (found != std::string::npos) {
    std::string key = line.substr(0, found);
    std::string value = line.substr(found + 1);
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    _headers.insert(std::make_pair(key, value));
  }
}

void Request::readFromSocket(unsigned int socketFd) {
  std::ifstream client;
  handleFirstLineHeader(socketFd);

  while (true) {
    std::ostringstream lineStream = Utils::get_next_line(socketFd);
    std::string line = lineStream.str();
    if (line.empty() ||
        line.find_first_not_of(" \t\r\n") == std::string::npos) {
      break;
    }

    saveHeaderLine(line);
  }

  std::string contentLengthStr = getHeaderField("content-length");
  if (contentLengthStr.empty())
    return;

  size_t contentLength = std::stol(contentLengthStr);

  std::cout << "ct: " << contentLength << std::endl;
  _body = std::vector<char>(contentLength);

  size_t totalRead = 0;

  while (totalRead < contentLength) {
    std::ostringstream bodyStream = Utils::get_next_line(socketFd);
    std::string chunk = bodyStream.str() + "\n";
    if (chunk.size() == 0)
      break;
    // std::cout << "chunk: " << chunk << " | " << chunk.size() << std::endl;
    std::cout << "chunk: " << chunk.size() << std::endl;
    std::copy(chunk.begin(), chunk.begin() + chunk.size(), _body.begin() + totalRead);
    totalRead += chunk.size();
    // std::cout << "body: " << _body.data() << " | " << _body.size() << std::endl;
    std::cout << "body: " << totalRead << std::endl;
  }
}

void Request::appendToBody() {}

void Request::setMethod(std::string s) { this->_method = s; }

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

bool Request::isCGI() const {
  return getMimeType() == "application/python" ? true : false;
}

std::string Request::getMethod() const { return this->_method; }

std::string Request::getPath() const { return this->_path; }

std::string Request::getMimeType() const { return this->_mimetype; }

std::string Request::getQuery() const { return this->_query; }

std::vector<char> Request::getBody() const { return this->_body; }

std::string Request::getHeaderField(std::string field) const {
  if (_headers.count(field))
    return _headers.at(field);
  return "";
}

std::ostream &operator<<(std::ostream &out, const Request &req) {
  out << req._data << std::endl;
  return out;
}
