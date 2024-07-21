#include "../inc/Client.hpp"

Client::Client(unsigned short fd, FD_WAITING_FOR status)
    : _fd(fd), _waitFor(status), _dataSent(0), _request(""), _response() {}
Client::Client(const Client &cp) : _fd(0), _request("") { *this = cp; }
Client &Client::operator=(const Client &rhs) {
  if (this != &rhs) {
    this->setFd(rhs._fd);
    this->_request = rhs._request;
    this->_waitFor = rhs._waitFor;
    this->_dataSent = rhs._dataSent;
  }
  return *this;
}
Client::~Client() {}

unsigned short Client::getFd() const { return this->_fd; }
void Client::setFd(unsigned short fd) { this->_fd = fd; }

const Request &Client::getRequest() const { return this->_request; }
int Client::readRequest() {
  char buffer[BUFFER_SIZE];
  memset(&buffer, 0, BUFFER_SIZE);
  int byteReceived = read(getFd(), buffer, BUFFER_SIZE);
  setWaitingStatus(WRITING);
  if (byteReceived > 0) {
    _request = Request(buffer);
    buildResponse();
  }
  return byteReceived;
}

void Client::buildResponse() {
  if (getRequest().getMethod().empty()) {
    std::cout << ("Try to create the response but the request class is empty")
              << std::endl;
    exit(1);
  }
  _response.build(this->getRequest());
}
const std::string Client::getResponseToString() const {
  return _response.getResponse();
}
const Response &Client::getResponse() const { return _response; }

void Client::sendResponse() {
  long bytesSent;
  std::string response = getResponseToString();

  bytesSent = write(getFd(), response.c_str() + getDataSent(),
                    response.size() - getDataSent());
  std::cout << "ByteSent : " << bytesSent << std::endl;
  if (bytesSent < 0) {
    setWaitingStatus(READING);
    setDataSent(-1);
  } else if (bytesSent + getDataSent() == static_cast<long>(response.size()) ||
             bytesSent < 0) {
    std::cout << ("------ Server Response sent to client ------\n")
              << std::endl;
    setWaitingStatus(READING);
    setDataSent(0);
  } else {
    setDataSent(getDataSent() + bytesSent);
    setWaitingStatus(WRITING);
  }
}

int Client::getDataSent() const { return _dataSent; }
void Client::setDataSent(int dataSent) { this->_dataSent = dataSent; }

FD_WAITING_FOR Client::getWaitingStatus() const { return this->_waitFor; }
void Client::setWaitingStatus(FD_WAITING_FOR status) {
  this->_waitFor = status;
}
