#pragma once
#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

class TCPSocket {
private:
  int _socketFD;
  std::string _ipAddress;
  int _port;
  std::vector<int> _clientsFD;
  struct sockaddr_in _socketAddress;
  unsigned int _socketAddressLength;
  TCPSocket();

public:
  TCPSocket(std::string, unsigned int);
  TCPSocket(const TCPSocket &);
  TCPSocket &operator=(const TCPSocket &);
  ~TCPSocket();
  bool initSocket();
  void closeServer() const;

  // GETTERS
  int getSocketFD() const;
  const std::vector<int> &getClientsFD() const;
  int getPort() const;
  std::string getIpAddress() const;
  struct sockaddr_in &getSocketAdress();
  unsigned int &getSocketAddressLength();

  // SETTERS
  void addClient(int);
  void removeClient(int);
};
