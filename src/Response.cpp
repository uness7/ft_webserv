#include "../inc/Response.hpp"

Response::Response() : _value(""), _statusCode(), _contentType(""), _buffer("") {}
Response::Response(const Request &request) : _value(""), _statusCode(), _contentType(""), _buffer("") {
  this->build(request);
}
Response::Response(const Response &cp) { *this = cp;}
Response &Response::operator=(const Response &rhs) {
  if (this != &rhs)
  {
    this->_value = rhs._value;
    this->_statusCode = rhs.getStatusCode();
    this->_contentType = rhs._contentType;
    this->_buffer = rhs._buffer;
  }
  return *this;
}
Response::~Response() {}

void Response::build(const Request &request) {
  std::string path = request.getPath();
  std::ifstream inFile(
      std::string("." + path)
          .c_str()); // Problem when path is a directory ("/" | "/static")
  std::stringstream buffer;
  if (inFile.is_open()) {
    buffer << inFile.rdbuf();
    inFile.close();
    setStatusCode(200);
    _buffer = buffer.str();
    _contentType = request.getMimeType();
  } else {
    std::cout << request;
    std::ifstream inFile("./static/index.html");
    buffer << inFile.rdbuf();
    inFile.close();
    setStatusCode(404);
    _buffer = buffer.str();
    _contentType = "text/html";
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
