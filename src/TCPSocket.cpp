#include "../inc/TCPSocket.hpp"
#include <algorithm>

void exitWithFailure(std::string s, int port) {
  std::cerr << port << " -> " << s << std::endl;
  exit(1);
}

TCPSocket::TCPSocket(std::string ipAddress, unsigned int port)
    : _ipAddress(ipAddress), _port(port), _socketAddress(),
      _socketAddressLength(sizeof(_socketAddress)) {
  _socketAddress.sin_family = AF_INET;
  _socketAddress.sin_port = htons(_port);
  _socketAddress.sin_addr.s_addr = inet_addr(ipAddress.c_str());
  // std::cout << "Server start " << _port << std::endl;
}

TCPSocket::TCPSocket(const TCPSocket &cp) { *this = cp; }

TCPSocket &TCPSocket::operator=(const TCPSocket &rhs) {
  if (this != &rhs) {
    _ipAddress = rhs._ipAddress;
    _socketFD = rhs._socketFD;
    _port = rhs._port;
    _socketAddress = rhs._socketAddress;
    _socketAddressLength = rhs._socketAddressLength;
  }
  return *this;
}

TCPSocket::~TCPSocket() {
  std::cout << "Server closed " << _port << std::endl;
  closeServer();
}

int TCPSocket::getSocketFD() const { return this->_socketFD; }
int TCPSocket::getPort() const { return this->_port; }
std::string TCPSocket::getIpAddress() const { return this->_ipAddress; }
struct sockaddr_in &TCPSocket::getSocketAdress() {
  return this->_socketAddress;
}
unsigned int &TCPSocket::getSocketAddressLength() {
  return this->_socketAddressLength;
}

void TCPSocket::closeServer() const { close(_socketFD); }

bool TCPSocket::initSocket() {
  _socketFD = socket(AF_INET, SOCK_STREAM, 0);
  if (_socketFD < 0) {
    exitWithFailure("Cannot create socket", _port);
    return false;
  }
  int opt = 1;
  if (setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    exitWithFailure("Setsockopt SO_REUSEADDR failed", _port);
    return false;
  }

  if (setsockopt(_socketFD, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
    exitWithFailure("Setsockopt SO_REUSEPORT failed", _port);
    return false;
  }
  if (bind(_socketFD, (sockaddr *)&_socketAddress, _socketAddressLength) < 0) {
    exitWithFailure("Cannot connect socket to address", _port);
    return false;
  }
  if (fcntl(_socketFD, F_SETFL, O_NONBLOCK) < 0) {
    exitWithFailure("Failed to set non-blocking mode for client socket", _port);
    return false;
  }
  return true;
}
