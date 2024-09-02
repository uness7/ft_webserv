/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzioual <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 14:26:25 by yzioual           #+#    #+#             */
/*   Updated: 2024/09/02 14:31:02 by yzioual          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

static volatile bool	stopListening = false;

void	log(const std::string &message) { std::cout << message << std::endl; }

void	exitWithFailure(std::string s)
{
	std::cerr << s << std::endl;
	exit(1);
}

void	handleSignal(int sig)
{
	if (sig == SIGINT)
		stopListening = true;
}

Server::Server(std::vector<TCPSocket *> s) : _sockets(s), _clients()
{
	std::signal(SIGINT, handleSignal);
}

Server::~Server()
{
	//
}

TCPSocket	*Server::getSocketByFD(int targetFD) const
{
	for (size_t i = 0; i < _sockets.size(); i++)
	{
		if (_sockets[i]->getSocketFD() == targetFD)
			return _sockets[i];
	}
	return NULL;
}

void	Server::acceptConnection(TCPSocket *s)
{
	int newClient = accept(s->getSocketFD(), (sockaddr *)&s->getSocketAdress(), &s->getSocketAddressLength());
	if (newClient < 0)
	{
		std::ostringstream ss;
		ss << "Server failed to accept incoming connection from ADDRESS: "
		   << inet_ntoa(s->getSocketAdress().sin_addr)
		   << "; PORT: " << ntohs(s->getSocketAdress().sin_port);
		exitWithFailure(ss.str());
	}
	if (fcntl(newClient, F_SETFL, O_NONBLOCK) < 0)
		exitWithFailure("Failed to set non-blocking mode for client socket");
	Client *n = new Client(newClient, s->getServerConfig());
	_clients.insert(std::make_pair(newClient, n));
	std::cout << "[NEW CLIENT]: FD -> " << newClient << " on " << s->getIpAddress() << ":" << s->getPort() << std::endl;
	Server::updateEpoll(_epoll_fd, EPOLL_CTL_ADD, newClient, NULL);
}

void	Server::updateEpoll(int epollFD, short action, int targetFD, struct epoll_event *ev)
{
	if (action == EPOLL_CTL_ADD)
	{
		struct epoll_event event;
		event.data.fd = targetFD;
		event.events = EPOLLIN;
		if (epoll_ctl(epollFD, EPOLL_CTL_ADD, targetFD, &event) == -1)
			exitWithFailure("epoll ctl problem");
	}
	else if (action == EPOLL_CTL_MOD)
	{
		ev->events = EPOLLOUT;
		if (epoll_ctl(epollFD, EPOLL_CTL_MOD, targetFD, ev) == -1)
			exitWithFailure("epoll ctl_mod problem");
	}
	else if (action == EPOLL_CTL_DEL)
		if (epoll_ctl(epollFD, EPOLL_CTL_DEL, targetFD, NULL) == -1)
			exitWithFailure("epoll ctl_del problem");
}

void	Server::startToListenClients()
{
	for (size_t i = 0; i < _sockets.size(); i++)
	{
		try
		{
			_sockets[i]->initSocket();
			if (listen(_sockets[i]->getSocketFD(), 0) < 0)
				exitWithFailure("Socket listen failed");
			std::ostringstream ss;
			ss << "\n*** Listening on ADDRESS: "
			   << inet_ntoa(_sockets[i]->getSocketAdress().sin_addr)		  
			   << " PORT: " << ntohs(_sockets[i]->getSocketAdress().sin_port) 
			   << " ***\n";
			log(ss.str());
			if (fcntl(_sockets[i]->getSocketFD(), F_SETFL, O_NONBLOCK) < 0)
				exitWithFailure("Failed to set non-blocking mode for client socket");
			Server::updateEpoll(_epoll_fd, EPOLL_CTL_ADD, _sockets[i]->getSocketFD(), NULL);
		}
		catch (const std::exception &e)
		{
			throw;
		}
	}
}

void	Server::handleClientRequest(int fd, struct epoll_event *ev)
{
	Client *client = _clients.at(fd);
	int byteReceived = client->readRequest();
	if (byteReceived > 0)
		Server::updateEpoll(_epoll_fd, EPOLL_CTL_MOD, fd, ev);
	else
		this->removeClient(fd);
}

void	Server::runServers(void)
{
	struct epoll_event events[MAX_EVENT];

	_epoll_fd = epoll_create1(0);
	if (_epoll_fd == -1)
		exitWithFailure("Error with Epoll_fd");
	startToListenClients();
	while (true)
	{
		if (stopListening)
			break;
		int nfds = epoll_wait(_epoll_fd, events, MAX_EVENT, -1);
		if (stopListening)
			break;
		if (nfds == -1)
			exitWithFailure("Error with epoll_wait");
		for (int i = 0; i < nfds; i++)
		{
			int fd_triggered = events[i].data.fd;
			short ev = events[i].events;
			if (ev & EPOLLHUP || ev & EPOLLERR)
			{
				std::cout << "Error on client: " << fd_triggered << std::endl;
				this->removeClient(fd_triggered);
			}
			else if (ev & EPOLLIN)
			{
				if (_clients.count(fd_triggered))
					handleClientRequest(fd_triggered, &events[i]);
				else
				{
					TCPSocket *server = getSocketByFD(fd_triggered);
					acceptConnection(server);
				}
			}
			else if (_clients.count(fd_triggered) && ev & EPOLLOUT)
			{
				Client *client = _clients.at(fd_triggered);
				handleResponse(client);
			}
		}
	}
	closeAllSockets();
}

void	Server::handleResponse(Client *client)
{
	client->sendResponse();
	std::string conn = client->getRequest().getHeaderField("Connection");
	if (client->getDataSent() <= 0 && conn.compare(" keep-alive") != 0)
		removeClient(client->getFd());
}

void	Server::closeAllSockets()
{
	for (size_t i = 0; i < _sockets.size(); i++)
		delete _sockets[i];
	std::map<unsigned short, Client *>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++)
		delete it->second;
}

void	Server::removeClient(int keyFD)
{
	std::map<unsigned short, Client *>::iterator element = _clients.find(keyFD);
	if (element == _clients.end())
		return;
	Server::updateEpoll(_epoll_fd, EPOLL_CTL_DEL, keyFD, NULL);
	std::cout << "[REMOVE CLIENT]: FD -> " << keyFD << " on " << element->second->getConfig().port << std::endl;
	close(keyFD);
	_clients.erase(element);
	delete element->second;
}
