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
  std::vector<TCPSocket *> _sockets;
  std::string buildResponse(const Request);
  void sendResponse(int, std::string response);
  void startToListenClients();
  void acceptConnection(TCPSocket *s);
  void closeAllSockets() const;
  bool isServerSocketFD(int);

public:
  Server(std::vector<TCPSocket *> &);
  ~Server();
  void runServers();
};
