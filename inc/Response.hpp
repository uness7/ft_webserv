#pragma once

#include "Request.hpp"
#include <iostream>

class Response {
private:
  std::string _value;

public:
  Response();
  ~Response();

  void build(const Request &);

  const std::string getResponse() const;
};
