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

  void initCgiEnv();
  void setArgv();
  void clear();

  void runProcess();
  std::string getScriptResult();

public:
  CGIResponse(Client *client, std::string &cgi_path);

  std::string exec(unsigned short &statusCode);
};

#endif
