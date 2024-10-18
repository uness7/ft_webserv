#pragma once
#include "Config.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <ostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <vector>

#define BUFFER_SIZE 4096
typedef unsigned char Byte;
typedef std::vector<Byte> Bytes;

class Request {
private:
  Bytes _data;
  Bytes _body;
  std::string _method;
  std::string _path;
  std::string _httpv;
  std::string _query;
  std::string _mimetype;
  std::map<std::string, std::string> _headers;
  long long _contentLength;

  ServerConfig _config;
  LocationConfig _target;
  short _statusCode;

  bool isValidClientMaxBody() const;
  void saveHeaderLine(std::string &);
  bool canResolvePath();
  bool areHeadersValid();
  void readBody(unsigned int, Bytes &, size_t);
  Bytes seekCRLF(Bytes const &request, Bytes::size_type &index);
  Bytes getNextChunk(unsigned int fd) const;

public:
  Request(ServerConfig);
  Request(const Request &);
  Request &operator=(const Request &);
  ~Request();

  long read(unsigned int);

  std::string getMethod() const;
  std::string getPath() const;
  std::string getHttpv() const;
  std::string getMimeType() const;
  std::string getQuery() const;
  short getStatusCode() const;
  Bytes &getBody();
  std::string getHeaderField(std::string) const;
  long long getContentLength() const;
  LocationConfig getPathLocation() const;

  void setHeaders(std::string &);
  void appendToBody();
  void setMethod(std::string);
  void setPath(std::string);
  void setMimeType();
};
