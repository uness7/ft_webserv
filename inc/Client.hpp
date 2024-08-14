#pragma once

#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <unistd.h>
#include <cstdlib>
#include <cstring>

class Response;

class Client {
private:
  unsigned short _fd;
  int _dataSent;
  Request _request;
  Response *_response;
  ServerConfig _config;

public:
  Client(unsigned short, ServerConfig);
  Client(const Client &);
  Client &operator=(const Client &);
  ~Client();

  unsigned short getFd() const;
  void setFd(unsigned short);

  int getDataSent() const;
  void setDataSent(int);

  Request &getRequest();
  int readRequest();

  const std::string getResponseToString() const;
  const Response *getResponse() const;
  void sendResponse();
  
  const ServerConfig getConfig() const;
};
