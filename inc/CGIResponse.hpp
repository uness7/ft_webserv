#ifndef CGIRESPONSE_HPP
#define CGIRESPONSE_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include "../inc/Client.hpp"
#include "Config.hpp"

class Client;

class CGIResponse {
public:
  CGIResponse(Client *client, std::string &cgi_path);

  void setCgiEnv();
  std::string execute();

private:
  Client *_client;
  std::string _cgiPath;
  std::string _scriptPath;
  std::map<std::string, std::string> _envMap;

  bool canExecFile();
};

#endif
