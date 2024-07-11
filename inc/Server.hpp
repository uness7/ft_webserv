#pragma once

#include "Request.hpp"
#include "TCPSocket.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#define BUFFER_SIZE 30720

class Server {
private:
  std::vector<TCPSocket> _sockets;
  std::string buildResponse(std::string);
  void sendResponse(const TCPSocket s, std::string response);
  void initToListen();
  void acceptConnection(TCPSocket &s);
  void closeAllSockets() const;

public:
  Server(std::vector<TCPSocket> &);
  ~Server();
  void startListen();
};
