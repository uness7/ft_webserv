#pragma once

#include "Request.hpp"
#include <iostream>

class Response {
private:
  std::string _value;


public:
  Response();
  Response(const Request &request);
  Response(const Response &);
  Response &operator=(const Response &);
  ~Response();

  void build(const Request &);

  const std::string getResponse() const;
};
