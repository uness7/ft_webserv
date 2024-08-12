#pragma once

#include "../inc/Config.hpp"

#include <arpa/inet.h>
#include <cstdlib>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// Class that manages the TCP socket operations
class TCPSocket
{
private:
  ServerConfig _serverConfig; // Server configuration
  int _socketFD; // File descriptor for the socket
  std::string _ipAddress; // IP address of the socket
  int _port; // Port number of the socket
  struct sockaddr_in _socketAddress; // Socket address structure
  unsigned int _socketAddressLength; // Length of the socket address structure
  TCPSocket(); // Default constructor (private to prevent use)

public:
  // Constructors and Destructor
  TCPSocket(const ServerConfig&);
  TCPSocket(const TCPSocket &);
  TCPSocket &operator=(const TCPSocket &);
  ~TCPSocket();

  // Member functions
  void initSocket(); // Function to initialize the socket
  void closeServer() const; // Function to close the socket

  // Getters
  int getSocketFD() const;
  int getPort() const;
  std::string getIpAddress() const;
  struct sockaddr_in &getSocketAdress();
  unsigned int &getSocketAddressLength();
  ServerConfig &getServerConfig();


  class CreateSocketException : public std::exception {
  public:
    const char * what() const throw() {
      return "Cannot create socket";
    }
  };
  class InitSocketException : public std::exception {
  public:
    const char * what() const throw() {
      return "Cannot init socket";
    }
  };
};

// Function to create sockets based on server configurations
std::vector<TCPSocket*> createSockets(const std::vector<ServerConfig>& serverConfigs);
