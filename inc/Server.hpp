#pragma once

#include <algorithm>
#include <arpa/inet.h>
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <sstream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>
#include <vector>

#include "Client.hpp"
#include "Request.hpp"
#include "TCPSocket.hpp"

# define BLOCK_INDEF -1
# define MAX_EVENT 100


class Server {
private:
  std::vector<TCPSocket *> _sockets;
  std::map<unsigned short, Client *> _clients;
  int _event_fd;

  void startToListenClients();
  void acceptConnection(TCPSocket *s);
  void handleResponse(Client *, struct epoll_event *ev);
  void removeClient(int keyFD);
  TCPSocket *getSocketByFD(int targetFD) const;
  void handleConnections(int nfds, struct epoll_event events[]);
  void onNewConnections(int fd);
  void handleClientRequest(int fd, struct epoll_event *ev);

public:
  Server(std::vector<TCPSocket *> sockets);
  ~Server();
  void runServers();
  void clearServer();

  class ServerException : public std::exception {
  protected:
    std::string msg;

  public:
    ServerException(const std::string &msg = "") : std::exception() {
      this->msg = msg;
    }
    virtual ~ServerException() throw() {}
    virtual const char *what() const throw() { return this->msg.c_str(); }
  };

  class AcceptConnectionException : public ServerException {
  public:
    AcceptConnectionException(std::string address) : ServerException() {
      std::ostringstream ss;

      ss << "Failed to accept client on address " << address;

      this->msg = ss.str();
    }
  };
};
