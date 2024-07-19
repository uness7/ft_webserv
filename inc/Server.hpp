#pragma once

#include "Client.hpp"
#include "Request.hpp"
#include "TCPSocket.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <map>
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
  void sendResponse(Client *);
  void startToListenClients();
  void acceptConnection(TCPSocket *s);
  void closeAllSockets() const;
  bool isServerSocketFD(int);
  int readRequest(Client *);
  void handleResponse(Client *);
  void removeClient(int keyFD);

public:
  std::map<unsigned short, Client *> _clients;

  Server(std::vector<TCPSocket *> &);
  ~Server();
  void runServers();
};
