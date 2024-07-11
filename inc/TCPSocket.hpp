#pragma once
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>


class TCPSocket {
private:
  TCPSocket();

public:
  TCPSocket(std::string, unsigned int);
  TCPSocket(const TCPSocket &);
  TCPSocket &operator=(const TCPSocket &);
  ~TCPSocket();
  int _socket;
  std::string _ipAddress;
  int _port;
  int _newSocket;
  struct sockaddr_in _socketAddress;
  unsigned int _socketAddressLength;
  bool initSocket();
  void closeServer() const;
};
