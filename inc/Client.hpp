#pragma once

#include "Request.hpp"

enum FD_WAITING_FOR { READING, WRITING };

class Client {
private:
  unsigned short _fd;
  Request _request;
  FD_WAITING_FOR _waitFor;
  int _dataSent;

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
  void setRequest(std::string buffer);

  FD_WAITING_FOR getWaitingStatus() const;
  void setWaitingStatus(FD_WAITING_FOR);
};
