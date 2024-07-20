#pragma once

#include "Request.hpp"
#include "Response.hpp"
#include <unistd.h>

enum FD_WAITING_FOR { READING, WRITING };
#define BUFFER_SIZE 30720

class Client {
private:
  unsigned short _fd;
  FD_WAITING_FOR _waitFor;
  int _dataSent;
  Request _request;
  Response _response;

public:
  Client(unsigned short, FD_WAITING_FOR);
  Client(const Client &);
  Client &operator=(const Client &);
  ~Client();

  unsigned short getFd() const;
  void setFd(unsigned short);

  int getDataSent() const;
  void setDataSent(int);

  const Request &getRequest() const;
  int readRequest();

  void buildResponse();
  const std::string getResponseToString() const;
  const Response &getResponse() const;
  void sendResponse();

  FD_WAITING_FOR getWaitingStatus() const;
  void setWaitingStatus(FD_WAITING_FOR);
};
