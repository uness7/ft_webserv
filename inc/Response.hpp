#pragma once

#include "Request.hpp"
#include <iostream>

struct STATUS_CODE {
  unsigned short code;
  std::string status;
};

class Response {
private:
  std::string _value;
  STATUS_CODE _statusCode;
  std::string _contentType;
  std::string _buffer;


public:
  Response();
  Response(const Request &request);
  Response(const Response &);
  Response &operator=(const Response &);
  ~Response();

  void build(const Request &);

  const std::string getResponse() const;

  void setStatusCode(STATUS_CODE);
  void setStatusCode(unsigned short code);
  STATUS_CODE getStatusCode() const;
  std::string getStatusToString() const;
};
