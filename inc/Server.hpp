#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <algorithm>
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <exception>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "Client.hpp"
#include "Request.hpp"
#include "TCPSocket.hpp"

#if __linux__
#include <sys/epoll.h>
#elif __APPLE__
#include <sys/event.h>
#endif

class Server {
       private:
	std::vector<TCPSocket *> _sockets;
	std::map<unsigned short, Client *> _clients;
	int _event_fd;

	void startToListenClients();
	void acceptConnection(TCPSocket *s);
#if __linux__
	void handleResponse(Client *, struct epoll_event *ev);
#elif __APPLE__
	void handleResponse(Client *);
#endif

	void removeClient(int keyFD);
	TCPSocket *getSocketByFD(int targetFD) const;
	Server(void);

#if __linux__
	void handleClientRequest(int fd, struct epoll_event *ev);
#elif __APPLE__
	void handleClientRequest(int fd);
#endif
       public:
	Server(std::vector<TCPSocket *> sockets);
	~Server();
	void runServers();
	void clearServer();

	class ServerException : public std::exception {
	       protected:
		std::string msg;

	       public:
		ServerException(const std::string &msg = "")
		    : std::exception() {
			this->msg = msg;
		}
		virtual ~ServerException() throw() {}
		virtual const char *what() const throw() {
			return this->msg.c_str();
		}
	};

	class AcceptConnectionException : public ServerException {
	       public:
		AcceptConnectionException(std::string address)
		    : ServerException() {
			std::ostringstream ss;

			ss << "Failed to accept client on address " << address;

			this->msg = ss.str();
		}
	};
};
