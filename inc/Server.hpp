#pragma once

#include "Client.hpp"
#include "Request.hpp"
#include "TCPSocket.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/event.h>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <map>

class Server
{
private:
  std::vector<TCPSocket *> _sockets;           // Vector to store pointers to TCPSocket instances
  std::map<unsigned short, Client *> _clients; // Map to store connected clients
  int _kqueue_fd;

  void startToListenClients();
  void acceptConnection(TCPSocket *s);
  void handleClientRequest(int fd);
  void handleResponse(Client *);

  void removeClient(int keyFD);
  TCPSocket *getSocketByFD(int targetFD) const;
  bool isServerSocketFD(int);
  void closeAllSockets();

  static void updateKqueue(int kqFD, short action, int targetFD);

public:
  Server(std::vector<TCPSocket *> sockets);
  ~Server();
  void runServers();
};
