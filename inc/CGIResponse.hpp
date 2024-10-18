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
#include <sys/_types/_pid_t.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include "../inc/Client.hpp"
#include "Config.hpp"

class Client;

class CGIResponse {
private:
  Client *_client;
  std::string _cgiPath;
  std::string _scriptPath;
  std::map<std::string, std::string> _envMap;
  char **_envp;
  char *_argv[3];
  pid_t _pid;
  int _pipe_in[2];
  int _pipe_out[2];
  unsigned short _statusCode;

  bool canExecScript();
  void setCgiEnv();
  void setArgv();
  void clear();

  void runProcess();

public:
  CGIResponse(Client *client, std::string &cgi_path);

  std::string exec(unsigned short &statusCode);
};

#endif
