#include "../inc/Client.hpp"

Client::Client(unsigned short fd)
    : _fd(fd), _dataSent(0), _request(""), _response() {}
Client::Client(const Client &cp) : _fd(0), _request("") { *this = cp; }
Client &Client::operator=(const Client &rhs)
{
  if (this != &rhs)
  {
    this->setFd(rhs._fd);
    this->_request = rhs._request;
    this->_dataSent = rhs._dataSent;
  }
  return *this;
}
Client::~Client() {}

unsigned short Client::getFd() const { return this->_fd; }
void Client::setFd(unsigned short fd) { this->_fd = fd; }

const Request &Client::getRequest() const { return this->_request; }
int Client::readRequest()
{
  char buffer[BUFFER_SIZE];
  memset(&buffer, 0, BUFFER_SIZE);
  int byteReceived = read(getFd(), buffer, BUFFER_SIZE);
  if (byteReceived > 0)
  {
    _request = Request(buffer);
    _response = Response(this->getRequest());
  }
  return byteReceived;
}

const std::string Client::getResponseToString() const
{
  return _response.getResponse();
}
const Response &Client::getResponse() const { return _response; }

void Client::sendResponse()
{
  long bytesSent;
  std::string response = getResponseToString();

  bytesSent = write(getFd(), response.c_str() + getDataSent(),
                    response.size() - getDataSent());
  if (bytesSent + getDataSent() == static_cast<long>(response.size()))
  {
    setDataSent(0);
  }
  else
  {
    setDataSent(getDataSent() + bytesSent);
  }
}

int Client::getDataSent() const { return _dataSent; }
void Client::setDataSent(int dataSent) { this->_dataSent = dataSent; }
