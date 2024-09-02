#pragma once
# include <fstream>
# include <iostream>
# include <map>
# include <ostream>
# include <sstream>
# include <string>
# include <vector>
# include <algorithm>
# include <cctype>
# include <fstream>
# include <iterator>
# include "Utils.hpp"

class Request {
private:
  std::string _method;
  std::string _path;
  std::string _query;
  std::string _mimetype;
  std::vector<char> _body;
  std::map<std::string, std::string> _headers;

public:
  std::string _data;
  Request();
  Request(const Request &);
  Request &operator=(const Request &);
  ~Request();

  long readFromSocket(unsigned int);
  long handleFirstLineHeader(unsigned int);
  void saveHeaderLine(std::string &);

  std::string getMethod() const;
  std::string getPath() const;
  std::string getMimeType() const;
  std::string getQuery() const;
  std::vector<char> getBody() const;
  std::string getHeaderField(std::string) const;

  bool isCGI() const;

  void setHeaders(std::string &);
  void appendToBody();
  void setMethod(std::string);
  void setPath(std::string);
  void setMimeType();
};

std::ostream &operator<<(std::ostream &out, const Request &);
