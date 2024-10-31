#include "TCPSocket.hpp"

TCPSocket::TCPSocket(const ServerConfig &serverConfig)
    : _serverConfig(serverConfig),
      _socketFD(-1),
      _ipAddress(serverConfig.listen),
      _port(serverConfig.port),
      _socketAddress(),
      _socketAddressLength(0) {
	struct addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	std::stringstream ss;
	ss << _port;
	if (getaddrinfo(_ipAddress.c_str(), ss.str().c_str(), &hints,
			&_socketAddress) != 0) {
		throw SocketInitException("Failed to get address info for ",
					  getSocketAddressToString());
	}
	_socketAddressLength =
	    _socketAddress->ai_addrlen;
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
	freeaddrinfo(_socketAddress);
}

int TCPSocket::getSocketFD() const { return _socketFD; }

int TCPSocket::getPort() const { return _port; }

std::string TCPSocket::getIpAddress() const { return _ipAddress; }

ServerConfig &TCPSocket::getServerConfig() { return _serverConfig; }

std::string TCPSocket::getSocketAddressToString() const {
	std::stringstream ss;
	ss << _ipAddress << ":" << _port;
	return ss.str();
}

struct sockaddr_in *TCPSocket::getSocketAddress() const {
	return (struct sockaddr_in *)_socketAddress->ai_addr;
}

socklen_t &TCPSocket::getSocketAddressLength() { return _socketAddressLength; }

void TCPSocket::closeServer() const { close(_socketFD); }

void TCPSocket::initSocket() {
	_socketFD =
	    socket(_socketAddress->ai_family, _socketAddress->ai_socktype,
		   _socketAddress->ai_protocol);
	if (_socketFD < 0)
		throw SocketInitException("Failed to create socket on address ",
					  getSocketAddressToString());

	int opt = 1;
	if (setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) <
	    0)
		throw SocketInitException("Impossible to reuse address ",
					  getSocketAddressToString());

	if (bind(_socketFD, _socketAddress->ai_addr, _socketAddressLength) < 0)
		throw SocketInitException(
		    "Impossible to bind socket to address ",
		    getSocketAddressToString());
}

std::vector<TCPSocket *> createSockets(
    const std::vector<ServerConfig> &serverConfigs) {
	std::vector<TCPSocket *> sockets;
	for (size_t i = 0; i < serverConfigs.size(); ++i) {
		sockets.push_back(new TCPSocket(serverConfigs[i]));
	}
	return sockets;
}

