#include "Server.hpp"

static volatile bool stopListening = false;

void log(const std::string &message) { std::cout << message << std::endl; }

void handleSignal(int sig) {
	if (sig == SIGINT) stopListening = true;
}

Server::Server(std::vector<TCPSocket *> s) : _sockets(s), _clients() {
	std::signal(SIGINT, handleSignal);
}
Server::~Server() { clearServer(); }

TCPSocket *Server::getSocketByFD(int targetFD) const {
	for (size_t i = 0; i < _sockets.size(); i++) {
		if (_sockets[i]->getSocketFD() == targetFD) return _sockets[i];
	}
	return NULL;
}

void Server::acceptConnection(TCPSocket *s) {
	int newClient =
	    accept(s->getSocketFD(), (sockaddr *)s->getSocketAddress(),
		   &s->getSocketAddressLength());
	if (newClient < 0 || fcntl(newClient, F_SETFL, O_NONBLOCK) < 0)
		throw AcceptConnectionException(s->getSocketAddressToString());

	Client *n = new Client(newClient, s->getServerConfig());
	_clients.insert(std::make_pair(newClient, n));

	std::cout << "[NEW CLIENT]: FD -> " << newClient << " on "
		  << s->getIpAddress() << ":" << s->getPort() << std::endl;
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = newClient;
	if (epoll_ctl(_event_fd, EPOLL_CTL_ADD, newClient, &ev) == -1)
		log("epoll ctl problem (sockets)");
}

void Server::startToListenClients() {
	for (size_t i = 0; i < _sockets.size(); i++) {
		try {
			_sockets[i]->initSocket();

			if (listen(_sockets[i]->getSocketFD(), 100) < 0)
				throw TCPSocket::SocketInitException(
				    "Impossible to listen socket to address ",
				    _sockets[i]->getSocketAddressToString());

			std::ostringstream ss;
			ss << "\n*** Listening on ADDRESS: "
			   << _sockets[i]->getSocketAddressToString()
			   << " ***\n";
			log(ss.str());
			if (fcntl(_sockets[i]->getSocketFD(), F_SETFL,
				  O_NONBLOCK) < 0)
				throw TCPSocket::SocketInitException(
				    "Failed to set non-blocking mode for "
				    "client socket ",
				    _sockets[i]->getSocketAddressToString());

			struct epoll_event ev;
			ev.events = EPOLLIN;
			ev.data.fd = _sockets[i]->getSocketFD();
			if (epoll_ctl(_event_fd, EPOLL_CTL_ADD,
				      _sockets[i]->getSocketFD(), &ev) == -1)
				log("epoll ctl problem (sockets)");
		} catch (const std::exception &e) {
			throw;
		}
	}
}

void Server::runServers(void) {
	const int MAX_EVENT = 1000;
	struct epoll_event events[MAX_EVENT];

	_event_fd = epoll_create(MAX_EVENT);
	if (_event_fd == -1) throw std::runtime_error("Error with epoll_wait");

	startToListenClients();

	while (true) {
		if (stopListening) break;

		int nfds = epoll_wait(_event_fd, events, MAX_EVENT, -1);
		if (stopListening) break;
		if (nfds == -1)
			throw std::runtime_error("Error with epoll_wait");

		for (int i = 0; i < nfds; i++) {
			int fd_triggered = events[i].data.fd;
			short ev = events[i].events;
			if (ev & EPOLLHUP || ev & EPOLLERR) {
				this->removeClient(fd_triggered);
			} else if (ev & EPOLLIN) {
				if (_clients.count(fd_triggered)) {
					handleClientRequest(fd_triggered,
							    &events[i]);
				} else {
					try {
						TCPSocket *server =
						    getSocketByFD(fd_triggered);
						if (!server)
							throw std::
							    runtime_error(
								"Server not "
								"found");
						acceptConnection(server);
					} catch (std::exception &e) {
						std::cerr << e.what()
							  << std::endl;
					}
				}
			} else if (_clients.count(fd_triggered) &&
				   ev & EPOLLOUT) {
				Client *client = _clients.at(fd_triggered);
				handleResponse(client, &events[i]);
			}
		}
	}
}

void Server::handleClientRequest(int fd, struct epoll_event *ev) {
	Client *client = _clients.at(fd);
	int byteReceived = client->readRequest();
	if (byteReceived == 0)
		removeClient(fd);
	else if (byteReceived > 0) {
		ev->events = EPOLLOUT;
		if (epoll_ctl(_event_fd, EPOLL_CTL_MOD, fd, ev) == -1)
			log("epoll ctl mod problem");
	}
}

void Server::handleResponse(Client *client, struct epoll_event *ev) {
	client->sendResponse();
	if (client->getDataSent() < 0) {
		removeClient(client->getFd());
		return;
	}
	std::string connection =
	    client->getRequest().getHeaderField("connection");
	if (connection != "keep-alive") {
		removeClient(client->getFd());
	} else if (client->getDataSent() == 0) {
		ev->events = EPOLLIN;
		if (epoll_ctl(_event_fd, EPOLL_CTL_MOD, client->getFd(), ev) ==
		    -1) {
			log("epoll ctl mod to re send problem");
			removeClient(client->getFd());
			return;
		}
		client->clear();
	}
}

void Server::clearServer() {
	std::map<unsigned short, Client *>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++) {
		std::cout << "Removed client fd -> " << it->first << std::endl;
		delete it->second;
	}

	_clients.clear();

	for (size_t i = 0; i < _sockets.size(); i++) {
		delete _sockets[i];
	}
	_sockets.clear();
}

void Server::removeClient(int keyFD) {
	std::map<unsigned short, Client *>::iterator element =
	    _clients.find(keyFD);
	if (element == _clients.end()) return;
	if (epoll_ctl(_event_fd, EPOLL_CTL_DEL, keyFD, NULL) == -1) {
		log("epoll ctl_del problem");
	}
	std::cout << "[REMOVE CLIENT]: FD -> " << keyFD << " on "
		  << element->second->getConfig().listen
		  << "::" << element->second->getConfig().port << std::endl;
	close(keyFD);
	delete element->second;
	_clients.erase(element);
}
