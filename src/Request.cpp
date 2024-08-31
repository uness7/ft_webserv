#include "../inc/Request.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iterator>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

Request::Request()
    : _method(""), _path(""), _query(""), _mimetype(""), _body(), _headers(),
    _data() {}
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

long Request::handleFirstLineHeader(unsigned int socketFd) {
  std::ostringstream lineStream; 
  long bytesRead = Utils::get_next_line(socketFd, lineStream);
  if (bytesRead < 0)
      return bytesRead;
  std::istringstream iss(lineStream.str());
  std::string method;
  std::string path;
  iss >> method >> path;
  setMethod(method);
  setPath(path);
  return bytesRead;
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

long Request::readFromSocket(unsigned int socketFd) {
  std::ifstream client;
  handleFirstLineHeader(socketFd);
  std::ostringstream lineStream;

  while (true) {
    long bytesRead = Utils::get_next_line(socketFd, lineStream);
    if (bytesRead < 0)
        return bytesRead;
    std::string line = lineStream.str();
    if (line.empty() ||
        line.find_first_not_of(" \t\r\n") == std::string::npos) {
      break;
    }

    saveHeaderLine(line);
  }

  std::string contentLengthStr = getHeaderField("content-length");
  if (contentLengthStr.empty())
    return 1;

  long long contentLength = atoll(contentLengthStr.c_str());

  std::cout << "ct: " << contentLength << std::endl;
  _body = std::vector<char>(contentLength);

  size_t totalRead = 0;

  while (static_cast<long long>(totalRead) < contentLength) {
    long bytesRead = Utils::get_next_line(socketFd, lineStream);;
    if (bytesRead < 0)
        return bytesRead;
    std::string chunk = lineStream.str() + "\n";
    if (chunk.size() == 0)
      break;
    // std::cout << "chunk: " << chunk << " | " << chunk.size() << std::endl;
    std::cout << "chunk: " << chunk.size() << std::endl;
    std::copy(chunk.begin(), chunk.begin() + chunk.size(), _body.begin() + totalRead);
    totalRead += chunk.size();
    // std::cout << "body: " << _body.data() << " | " << _body.size() << std::endl;
    std::cout << "body: " << totalRead << std::endl;
  }
  return 1;
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
