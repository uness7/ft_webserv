#pragma once

#include "Client.hpp"
#include "Request.hpp"
#include "TCPSocket.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <map>

#if __linux__
  #include <sys/epoll.h>
#elif __APPLE__
  #include <sys/event.h>
#endif

class Server {
private:
  std::vector<TCPSocket *> _sockets;
  std::map<unsigned short, Client *> _clients;
  int _event_fd;

  void startToListenClients();
  void acceptConnection(TCPSocket *s);
  void handleResponse(Client *);

  void removeClient(int keyFD);
  TCPSocket *getSocketByFD(int targetFD) const;
  void closeAllSockets();

#if __linux__
  void handleClientRequest(int fd, struct epoll_event *ev);
  static void updateEpoll(int epollFD, short action, int targetFD, struct epoll_event *ev);
#elif __APPLE__
  void handleClientRequest(int fd);
  static void updateKqueue(int kqFD, short action, int targetFD);
#endif
public:
  Server(std::vector<TCPSocket *> sockets);
  ~Server();
  void runServers();
};
