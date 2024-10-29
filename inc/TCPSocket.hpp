#pragma once

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

#include "Config.hpp"
#include <netdb.h>

class TCPSocket
{
	private:
		ServerConfig 	_serverConfig;
		int		_socketFD;
		std::string 	_ipAddress;
		int 		_port;
		struct addrinfo 	*_socketAddress;
		unsigned int 		_socketAddressLength;
		TCPSocket();

       public:
		TCPSocket(const ServerConfig &);
		TCPSocket(const TCPSocket &);
		TCPSocket &operator=(const TCPSocket &);
		~TCPSocket();
		void	initSocket();
		void 	closeServer() const;
		int 	getSocketFD() const;
		int 	getPort() const;
		std::string getIpAddress() const;
		struct sockaddr_in *getSocketAddress() const;
		std::string getSocketAddressToString() const;
		unsigned int &getSocketAddressLength();
		ServerConfig &getServerConfig();

	class SocketException : public std::exception {
	       protected:
		std::string msg;

	       public:
		SocketException(const std::string &msg = "")
		    : std::exception() {
			this->msg = msg;
		}
		virtual ~SocketException() throw() {}
		virtual const char *what() const throw() {
			return this->msg.c_str();
		}
	};

	class SocketInitException : public SocketException {
	       public:
		SocketInitException(std::string errorMessage,
				    std::string address)
		    : SocketException() {
			std::ostringstream ss;

			ss << errorMessage << address;

			this->msg = ss.str();
		}
	};
};

std::vector<TCPSocket *> createSockets(
    const std::vector<ServerConfig> &serverConfigs);
