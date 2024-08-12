#include "../inc/TCPSocket.hpp"
#include <algorithm>

void exitWithFailure(std::string s, int port)
{
  std::cerr << port << " -> " << s << std::endl;
  exit(1);
}

// Constructor for TCPSocket
TCPSocket::TCPSocket(const ServerConfig &serverConfig)
    : _serverConfig(serverConfig), _socketFD(-1), _ipAddress(serverConfig.listen), _port(serverConfig.port), _socketAddress(), _socketAddressLength(sizeof(_socketAddress))
{
  _socketAddress.sin_family = AF_INET;
  _socketAddress.sin_port = htons(_port);
  _socketAddress.sin_addr.s_addr = inet_addr(_ipAddress.c_str());
}

TCPSocket::TCPSocket(const TCPSocket &cp) { *this = cp; }

TCPSocket &TCPSocket::operator=(const TCPSocket &rhs)
{
  if (this != &rhs)
  {
    _ipAddress = rhs._ipAddress;
    _socketFD = rhs._socketFD;
    _port = rhs._port;
    _socketAddress = rhs._socketAddress;
    _socketAddressLength = rhs._socketAddressLength;
  }
  return *this;
}

TCPSocket::~TCPSocket()
{
  std::cout << "Server closed " << _port << std::endl;
  closeServer();
}

int TCPSocket::getSocketFD() const { return this->_socketFD; }
int TCPSocket::getPort() const { return this->_port; }
std::string TCPSocket::getIpAddress() const { return this->_ipAddress; }
struct sockaddr_in &TCPSocket::getSocketAdress()
{
  return this->_socketAddress;
}
unsigned int &TCPSocket::getSocketAddressLength()
{
  return this->_socketAddressLength;
}

ServerConfig &TCPSocket::getServerConfig() { return this->_serverConfig; }

void TCPSocket::closeServer() const { close(_socketFD); }

void TCPSocket::initSocket()
{
  // Create a new socket
  _socketFD = socket(AF_INET, SOCK_STREAM, 0);
  if (_socketFD < 0)
    throw CreateSocketException();

  // Option value to set for socket options
  int opt = 1;

  // Set socket options to allow reuse of local addresses
  if (setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    throw InitSocketException();

  // Bind the socket to the specified address and port
  if (bind(_socketFD, (sockaddr *)&_socketAddress, _socketAddressLength) < 0)
    throw InitSocketException();
}

// Function to create sockets based on the server configurations
std::vector<TCPSocket *> createSockets(const std::vector<ServerConfig> &serverConfigs)
{
  std::vector<TCPSocket *> sockets;
  // Iterate through each server configuration
  for (std::vector<ServerConfig>::const_iterator it = serverConfigs.begin(); it != serverConfigs.end(); ++it)
  {
    // Create a new TCPSocket with the given IP address and port from the configuration
    sockets.push_back(new TCPSocket(*it));
  }
  return sockets;
}
