#include "../inc/Response.hpp"

Response::Response() : _value("") {}
Response::Response(const Request &request) : _value("") {
  this->build(request);
}
Response::Response(const Response &cp) { *this = cp;}
Response &Response::operator=(const Response &rhs) {
  if (this != &rhs)
  {
    this->_value = rhs._value;
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
  } else {
    std::cerr << "Unable to open file " << path << std::endl;
    this->_value = "HTTP/1.1 302 Found\nLocation: /static/index.html\n\n";
    return;
  }
  std::string fileRequested = buffer.str();
  std::ostringstream ss;
  ss << "HTTP/1.1 200 OK\nContent-Type: " << request.getMimeType()
     << "\nContent-Length: " << fileRequested.size() << "\n\n"
     << fileRequested;
  this->_value = ss.str();
}

const std::string Response::getResponse() const { return this->_value; }
