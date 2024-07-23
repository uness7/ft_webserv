#pragma once

#include "Client.hpp"
#include "Request.hpp"
#include "TCPSocket.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <map>

class Server {
private:
  std::vector<TCPSocket *> _sockets;  // Vector to store pointers to TCPSocket instances
  std::map<unsigned short, Client *> _clients;  // Map to store connected clients
  int _epoll_fd;

  void startToListenClients();
  void acceptConnection(TCPSocket *s);
  void closeAllSockets() const;
  bool isServerSocketFD(int);
  void handleResponse(Client *);
  void removeClient(int keyFD);
  TCPSocket *getSocketByFD(int targetFD) const;

public:
  Server(std::vector<TCPSocket *> sockets);
  ~Server();
  void runServers();
};
