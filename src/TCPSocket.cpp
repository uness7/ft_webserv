#include "../inc/TCPSocket.hpp"

void exitWithFailure(std::string s, int port) {
  std::cerr << port << " -> " << s << std::endl;
  exit(1);
}

TCPSocket::TCPSocket(std::string ipAddress, unsigned int port)
    : _ipAddress(ipAddress), _port(port), _newSocket(), _socketAddress(),
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
    _newSocket = rhs._newSocket;
    _socket = rhs._socket;
    _port = rhs._port;
    _socketAddress = rhs._socketAddress;
    _socketAddressLength = rhs._socketAddressLength;
  }
  return *this;
}

TCPSocket::~TCPSocket() {
  // std::cout << "Server closed " << _port << std::endl;
}

void TCPSocket::closeServer() const {
  close(_socket);
  close(_newSocket);
}

bool TCPSocket::initSocket() {
  _socket = socket(AF_INET, SOCK_STREAM, 0);
  if (_socket < 0) {
    exitWithFailure("Cannot create socket", _port);
    return false;
  }
  int opt = 1;
  if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    exitWithFailure("Setsockopt SO_REUSEADDR failed", _port);
    return false;
  }

  if (setsockopt(_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
    exitWithFailure("Setsockopt SO_REUSEPORT failed", _port);
    return false;
  }
  if (bind(_socket, (sockaddr *)&_socketAddress, _socketAddressLength) < 0) {
    exitWithFailure("Cannot connect socket to address", _port);
    return false;
  }
  return true;
}
