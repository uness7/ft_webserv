/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TCPSocket.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzioual <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 14:52:03 by yzioual           #+#    #+#             */
/*   Updated: 2024/09/02 14:52:30 by yzioual          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/TCPSocket.hpp"

#include <sstream>

TCPSocket::TCPSocket(const ServerConfig &serverConfig)
    : _serverConfig(serverConfig),
      _socketFD(-1),
      _ipAddress(serverConfig.listen),
      _port(serverConfig.port),
      _socketAddress(),
      _socketAddressLength(sizeof(_socketAddress)) {
	_socketAddress.sin_family = AF_INET;
	_socketAddress.sin_port = htons(_port);
	_socketAddress.sin_addr.s_addr = inet_addr(_ipAddress.c_str());
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

std::string TCPSocket::getSocketAddressToString() {
	std::ostringstream address;
	address << getIpAddress() << "::" << getPort();
	return address.str();
}
struct sockaddr_in &TCPSocket::getSocketAddress() {
	return this->_socketAddress;
}

unsigned int &TCPSocket::getSocketAddressLength() {
	return this->_socketAddressLength;
}

ServerConfig &TCPSocket::getServerConfig() { return this->_serverConfig; }

void TCPSocket::closeServer() const { close(_socketFD); }

void TCPSocket::initSocket() {
	_socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFD < 0)
		throw SocketInitException("Failed to create socket on address ",
					  getSocketAddressToString());
	int opt = 1;
	if (setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) <
	    0)
		throw SocketInitException("Impossible to reuse address ",
					  getSocketAddressToString());

	if (bind(_socketFD, (sockaddr *)&_socketAddress, _socketAddressLength) <
	    0)
		throw SocketInitException(
		    "Impossible to bind socket to address ",
		    getSocketAddressToString());
}

std::vector<TCPSocket *> createSockets(
    const std::vector<ServerConfig> &serverConfigs) {
	std::vector<TCPSocket *> sockets;
	for (std::vector<ServerConfig>::const_iterator it =
		 serverConfigs.begin();
	     it != serverConfigs.end(); ++it)
		sockets.push_back(new TCPSocket(*it));
	return sockets;
}
