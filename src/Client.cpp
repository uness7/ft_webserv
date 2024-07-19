#include "../inc/Client.hpp"

Client::Client(unsigned short fd, FD_WAITING_FOR status)
    : _fd(fd), _request(""), _waitFor(status), _dataSent(0) {}
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
void Client::setRequest(std::string buffer) {
  this->_request = Request(buffer);
}

int Client::getDataSent() const { return _dataSent; }
void Client::setDataSent(int dataSent) { this->_dataSent = dataSent; }

FD_WAITING_FOR Client::getWaitingStatus() const { return this->_waitFor; }
void Client::setWaitingStatus(FD_WAITING_FOR status) {
  this->_waitFor = status;
}
