#pragma once
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>

class Request {
private:
  std::string _data;
  std::string _method;
  std::string _path;
  std::string _mimetype;
  std::string _version;
  std::string _host;
  std::string _userAgent;
  std::string _connection;

  void parseData();

public:
  Request(std::string);
  Request(const Request &);
  Request &operator=(const Request &);
  ~Request();

  // GETTERS
  std::string getMethod() const;
  std::string getPath() const;
  std::string getMimeType() const;
  std::string getConnection() const;

  // SETTERS
  void setMethod(std::string);
  void setPath(std::string);
  void setMimeType();
  void setHost(std::string);
  void setVersion(std::string);
  void setUserAgent(std::string);
  void setConnection(std::string);
};

std::ostream &operator<<(std::ostream &out, const Request &);
