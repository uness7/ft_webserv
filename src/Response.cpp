#include "../inc/Response.hpp"
#include <cstring>
#include <string>
#include <cstdlib>

Response::Response() : _value(""), _statusCode(), _contentType(""), _buffer(""), _client(NULL) {}
Response::Response(Client *client) : _value(""), _statusCode(), _contentType(""), _buffer(""), _client(client) {
  this->build();
}
Response::Response(const Response &cp) { *this = cp;}
Response &Response::operator=(const Response &rhs) {
  if (this != &rhs)
  {
    this->_value = rhs._value;
    this->_statusCode = rhs.getStatusCode();
    this->_contentType = rhs._contentType;
    this->_buffer = rhs._buffer;
    this->_client = rhs._client;
  }
  return *this;
}
Response::~Response() {}

std::vector<std::string> splitString(const std::string& str) {
  
    std::vector<std::string> result;
    std::istringstream iss(str);
    std::string part;

    // Utilisation de getline pour séparer la chaîne en deux parties
    if (std::getline(iss, part, ' ')) {
        result.push_back(part);
        if (std::getline(iss, part)) {
            result.push_back(part);
        }
    }

    return result;
}

void Response::buildError() {
  Request &request =_client->getRequest();
  std::vector<std::string> error_page = splitString(_client->getConfig().error_page);
  if (error_page.size() != 2)
    return;
  std::string newPath = _client->getConfig().locations.begin()->second.root + error_page[1];
  _client->getRequest().setPath(newPath); 
  std::stringstream buffer;
  std::ifstream inFile("." + request.getPath());
  buffer << inFile.rdbuf();
  inFile.close();
  setStatusCode(atoi(error_page[0].c_str()));
  _buffer = buffer.str();
  _contentType = request.getMimeType();
}

void Response::build() {
  Request &request =_client->getRequest();
  std::string newPath;
  if (request.getPath().compare("/") == 0) {
    newPath = _client->getConfig().locations.begin()->second.root + "/" + _client->getConfig().locations.begin()->second.index;
  } else 
  {
    newPath = _client->getConfig().locations.begin()->second.root + request.getPath();
  }
  _client->getRequest().setPath(newPath); 
  std::ifstream inFile(
      std::string("." + request.getPath())
          .c_str()); // Problem when path is a directory ("/" | "/static")
  std::stringstream buffer;
  if (inFile.is_open()) {
    buffer << inFile.rdbuf();
    inFile.close();
    setStatusCode(200);
    _buffer = buffer.str();
    _contentType = request.getMimeType();
  } else {
    buildError();
  }
  std::ostringstream ss;
  ss << "HTTP/1.1 " << this->getStatusToString() << "\nContent-Type: " << _contentType
     << "\nContent-Length: " << _buffer.size() << "\n\n"
     << _buffer;
  this->_value = ss.str();
}

const std::string Response::getResponse() const { return this->_value; }

void Response::setStatusCode(unsigned short code) {
  _statusCode.code = code;
  switch (code) {
    case 200:
      _statusCode.status = "OK";
      break;
    default:
      _statusCode.code = 404;
      _statusCode.status = "Not Found";
      break;
  }
}
void Response::setStatusCode(STATUS_CODE statusCode) {
  this->_statusCode.code = statusCode.code;
  this->_statusCode.status = statusCode.status;
}
STATUS_CODE Response::getStatusCode() const { return this->_statusCode; }
std::string Response::getStatusToString() const {
  std::stringstream ss;
  ss << _statusCode.code << " " << _statusCode.status;
  return ss.str();
}
