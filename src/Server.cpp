#include "../inc/Server.hpp"
#include <algorithm>
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sys/types.h>
#include <utility>
#include <vector>

static volatile bool stopListening = false;

void log(const std::string &message) { std::cout << message << std::endl; }

void handleSignal(int sig) {
  if (sig == SIGINT)
    stopListening = true;
}

Server::Server(std::vector<TCPSocket *> s) : _sockets(s), _clients() {
  std::signal(SIGINT, handleSignal);
#if __APPLE__
  std::signal(SIGPIPE, SIG_IGN);
#endif
}

Server::~Server() {
    clearServer();
}

TCPSocket *Server::getSocketByFD(int targetFD) const {
  for (size_t i = 0; i < _sockets.size(); i++) {
    if (_sockets[i]->getSocketFD() == targetFD)
      return _sockets[i];
  }
  return NULL;
}

void Server::acceptConnection(TCPSocket *s) {
  int newClient = accept(s->getSocketFD(), (sockaddr *)&s->getSocketAddress(),
                         &s->getSocketAddressLength());
  if (newClient < 0 || fcntl(newClient, F_SETFL, O_NONBLOCK) < 0)
    throw AcceptConnectionException(s->getSocketAddressToString());

  Client *n = new Client(newClient, s->getServerConfig());
  _clients.insert(std::make_pair(newClient, n));

  std::cout << "[NEW CLIENT]: FD -> " << newClient << " on "
            << s->getIpAddress() << ":" << s->getPort() << std::endl;

#if __linux__
  Server::updateEpoll(_event_fd, EPOLL_CTL_ADD, newClient, NULL);
#elif __APPLE__
  updateKqueue(_event_fd, EV_ADD, EVFILT_READ, newClient);
#endif
}

#if __linux__
void	Server::updateEpoll(int epollFD, short action, int targetFD, struct epoll_event *ev)
{
	if (action == EPOLL_CTL_ADD) {
		if (!ev) {
			struct epoll_event event;
			event.data.fd = targetFD;
			event.events = EPOLLIN;
			if (epoll_ctl(epollFD, EPOLL_CTL_ADD, targetFD, &event) == -1) {
				log("epoll ctl problem");
			}
		} else {
			ev->events = EPOLLIN;
			if (epoll_ctl(epollFD, EPOLL_CTL_MOD, targetFD, ev) == -1) {
				log("epoll ctl problem");
			}
		}
	} else if (action == EPOLL_CTL_MOD) {
		ev->events = EPOLLOUT;
		if (epoll_ctl(epollFD, EPOLL_CTL_MOD, targetFD, ev) == -1) {
			log("epoll ctl_mod problem");
		}
	} else if (action == EPOLL_CTL_DEL) {
		if (epoll_ctl(epollFD, EPOLL_CTL_DEL, targetFD, NULL) == -1) {
			log("epoll ctl_del problem");
		}
	}
}

#elif __APPLE__
void Server::updateKqueue(int kqFD, short action, short envfilt, int targetFD) {
  struct kevent event;
  if (action == EV_ADD) {
    EV_SET(&event, targetFD, envfilt, EV_ADD | EV_ENABLE, 0, 0, nullptr);
    if (kevent(kqFD, &event, 1, nullptr, 0, nullptr) == -1) {
      log("Failed to add event from kqueue");
        log(strerror(errno));
    }
  } else if (action == EV_DELETE) {
    EV_SET(&event, targetFD, envfilt, EV_DELETE, 0, 0, nullptr);
    if (kevent(kqFD, &event, 1, nullptr, 0, nullptr) == -1) {
      log("Failed to delete event from kqueue");
        log(strerror(errno));
    }
  }
}
#endif

void Server::startToListenClients() {
  for (size_t i = 0; i < _sockets.size(); i++) {
    try {
      _sockets[i]->initSocket();

      if (listen(_sockets[i]->getSocketFD(), 100) < 0)
		throw TCPSocket::SocketInitException("Impossible to listen socket to address ", _sockets[i]->getSocketAddressToString());

      std::ostringstream ss;
      ss << "\n*** Listening on ADDRESS: "
         << inet_ntoa(_sockets[i]
                          ->getSocketAddress()
                          .sin_addr) << " PORT: "
         << ntohs(_sockets[i]
                      ->getSocketAddress()
                      .sin_port) << " ***\n";
      log(ss.str());
      if (fcntl(_sockets[i]->getSocketFD(), F_SETFL, O_NONBLOCK) < 0)
		throw TCPSocket::SocketInitException("Failed to set non-blocking mode for client socket ", _sockets[i]->getSocketAddressToString());

#if __linux__
      Server::updateEpoll(_event_fd, EPOLL_CTL_ADD, _sockets[i]->getSocketFD(),
                          NULL);
#elif __APPLE__
      updateKqueue(_event_fd, EV_ADD, EVFILT_READ, _sockets[i]->getSocketFD());
#endif
    } catch (const std::exception &e) {
      throw;
    }
  }
}


#if __linux__
void Server::runServers(void) {
    const int MAX_EVENT = 1000;
    struct epoll_event events[MAX_EVENT];

    _event_fd = epoll_create1(0);
    if (_event_fd == -1)
      throw std::runtime_error("Error with epoll_wait");

    startToListenClients();

    while (true) {
        if (stopListening)
          break;

        int nfds = epoll_wait(_event_fd, events, MAX_EVENT, -1);
        if (stopListening)
          break;
        if (nfds == -1)
          throw std::runtime_error("Error with epoll_wait");

        for (int i = 0; i < nfds; i++) {
          int fd_triggered = events[i].data.fd;
          short ev = events[i].events;
          if (ev & EPOLLHUP || ev & EPOLLERR) {
            this->removeClient(fd_triggered);
          } else if (ev & EPOLLIN) {
            if (_clients.count(fd_triggered))
	    {
  	      std::cout << fd_triggered << " socket can read" << std::endl;
              handleClientRequest(fd_triggered, &events[i]);
	    }
            else {
              try {
                TCPSocket *server = getSocketByFD(fd_triggered);
                if (!server)
                  throw std::runtime_error("Server not found");
                acceptConnection(server);
              } catch (std::exception &e) {
                std::cerr << e.what() << std::endl;
              }
            }
          } else if (_clients.count(fd_triggered) && ev & EPOLLOUT) {
                Client *client = _clients.at(fd_triggered);
                handleResponse(client, &events[i]);
          }
        }
    }
}
#elif __APPLE__
void Server::runServers(void) {

  _event_fd = kqueue();
  if (_event_fd == -1)
    throw std::runtime_error("Error with kqueue");
  struct kevent events[1000];

  startToListenClients();

  while (true) {
    if (stopListening)
      break;

    int nfds = kevent(_event_fd, nullptr, 0, events, 1000, nullptr);
    if (nfds == -1) {
      std::cout << "ERROR ON KEVENT" << std::endl;
      if (errno == EINTR)
        continue;
      throw std::runtime_error("Error with kevent");
    }
    if (stopListening)
      break;

    for (int i = 0; i < nfds; i++) {
      int fd_triggered = events[i].ident;
      short ev = events[i].filter;
      if (_clients.count(fd_triggered) && events[i].flags & EV_ERROR)
        this->removeClient(fd_triggered);
      else if (ev == EVFILT_READ) {
        if (_clients.count(fd_triggered)) {
          handleClientRequest(fd_triggered);
        } else {
          TCPSocket *server = getSocketByFD(fd_triggered);
          if (server)
            acceptConnection(server);
        }
      } else if (_clients.count(fd_triggered) && ev == EVFILT_WRITE) {
        Client *client = _clients.at(fd_triggered);
        handleResponse(client);
      }
    }
  }
  clearServer();
}
#endif

#if __linux__
void Server::handleClientRequest(int fd, struct epoll_event *ev) {
  Client *client = _clients.at(fd);
  int byteReceived = client->readRequest();
  std::cout << "byteReceived " << byteReceived << std::endl;
  if (byteReceived > 0) {
    Server::updateEpoll(_event_fd, EPOLL_CTL_MOD, fd, ev);
  } else if (byteReceived == 0)
	removeClient(fd);
}
#elif __APPLE__
void Server::handleClientRequest(int fd) {
  Client *client = _clients.at(fd);
  int byteReceived = client->readRequest();
  std::cout << "byteReceived " << byteReceived << std::endl;
  if (byteReceived > 0) {
    Server::updateKqueue(_event_fd, EV_DELETE , EVFILT_READ, fd);
    Server::updateKqueue(_event_fd, EV_ADD, EVFILT_WRITE, fd);
  } else if (byteReceived == 0)
	removeClient(fd);
}
#endif

#if __linux__
void Server::handleResponse(Client *client, struct epoll_event *ev) {
    client->sendResponse();
    std::string conn = client->getRequest().getHeaderField("connection");
    if (client->getDataSent() < 0 || conn.compare(0, 5, "close") == 0)
    {
	std::cout << client->getFd() << " will be closed with status connection " << conn << " -> dataSent: " << client->getDataSent() << std::endl;
        removeClient(client->getFd());
    }
    else
    {
        updateEpoll(_event_fd, EPOLL_CTL_ADD, client->getFd(), ev);
        client->clear();
    }
}
#elif __APPLE__ 
void Server::handleResponse(Client *client) {
    client->sendResponse();
    std::string conn = client->getRequest().getHeaderField("connection");
    // std::cout << "response: " << client->getDataSent() << std::endl;
    // std::cout << "conn: " << conn << std::endl;
    if (!client->getDataSent())
        removeClient(client->getFd());
    else if (client->getDataSent() < 0 || conn.compare(0, 5, "close") == 0)
        removeClient(client->getFd());
    else
    {
        std::cout << "test\n"; 
        updateKqueue(_event_fd, EV_DELETE, EVFILT_WRITE, client->getFd());
        std::cout << "test\n"; 
        updateKqueue(_event_fd, EV_ADD, EVFILT_READ, client->getFd());
        client->clear();
    }
    // exit(0);
}
#endif

void Server::clearServer() {
  std::map<unsigned short, Client *>::iterator it;
  for (it = _clients.begin(); it != _clients.end(); it++) {
    std::cout << "Removed client fd -> " << it->first << std::endl;
    delete it->second;
  }

  for (size_t i = 0; i < _sockets.size(); i++) {
    delete _sockets[i];
  }
}

void Server::removeClient(int keyFD)
{
	std::map<unsigned short, Client *>::iterator element = _clients.find(keyFD);
	if (element == _clients.end())
		return;
#if __linux__
	Server::updateEpoll(_event_fd, EPOLL_CTL_DEL, keyFD, NULL);
#elif __APPLE__
	Server::updateKqueue(_event_fd, EV_DELETE, EVFILT_WRITE, keyFD);
#endif
	std::cout << "[REMOVE CLIENT]: FD -> " << keyFD << " on "
		<< element->second->getConfig().listen << "::"  << element->second->getConfig().port << std::endl;
	close(keyFD);
    	if (element->second != NULL)
		delete element->second;
	_clients.erase(element);
}
