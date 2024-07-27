#pragma once

#include "Request.hpp"
#include "Response.hpp"
#include <unistd.h>
#include <cstdlib>
#include <cstring>

#define BUFFER_SIZE 30720

class Client {
private:
  unsigned short _fd;
  int _dataSent;
  Request _request;
  Response _response;

public:
  Client(unsigned short);
  Client(const Client &);
  Client &operator=(const Client &);
  ~Client();

  unsigned short getFd() const;
  void setFd(unsigned short);

  int getDataSent() const;
  void setDataSent(int);

  const Request &getRequest() const;
  int readRequest();

  const std::string getResponseToString() const;
  const Response &getResponse() const;
  void sendResponse();

};
