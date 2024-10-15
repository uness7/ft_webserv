#pragma once

#include "Client.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <iostream>
#include <map>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

struct STATUS_CODE {
  unsigned short code;
  std::string status;
};

class Client;

class Response {
private:
  std::string _value;
  STATUS_CODE _statusCode;
  std::string _contentType;
  std::string _buffer;
  Client *_client;
  std::vector<std::string> _cookies;
  LocationConfig _target;

  void build();
  void buildError();
  void handleCGI();
  void handleStaticFiles(void);
  void finalizeHTMLResponse(void);
  void generateAutoIndex();
  void updateResponse(unsigned short statusCode, std::string contentType,
                      std::string buffer);

public:
  Response();
  Response(Client *);
  Response(const Response &);
  Response &operator=(const Response &);
  ~Response();

  const std::string getResponse() const;

  void setStatusCode(STATUS_CODE);
  void setStatusCode(unsigned short code);
  STATUS_CODE getStatusCode() const;
  std::string getStatusToString() const;

  static const std::map<unsigned short, std::string> &getStatusCodes();
};
