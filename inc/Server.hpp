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

class Server {
private:
  std::vector<TCPSocket *> _sockets;

  void startToListenClients();
  void acceptConnection(TCPSocket *s);
  void closeAllSockets() const;
  bool isServerSocketFD(int);
  void handleResponse(Client *);
  void removeClient(int keyFD);

public:
  std::map<unsigned short, Client *> _clients;

  Server(std::vector<TCPSocket *> &);
  ~Server();
  void runServers();
};
