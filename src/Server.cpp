#include "../inc/Server.hpp"
#include <algorithm>
#include <cerrno>
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sys/event.h>
#include <sys/poll.h>
#include <utility>
#include <vector>

static volatile bool stopListening = false;

void log(const std::string &message) { std::cout << message << std::endl; }

void exitWithFailure(std::string s) {
  std::cerr << s << std::endl;
  exit(1);
}

void handleSignal(int sig) {
  if (sig == SIGINT)
    stopListening = true;
}

Server::Server(std::vector<TCPSocket *> &s)
    : _sockets(s), _kqueue_fd(-1), _clients() {
  std::signal(SIGINT, handleSignal);
  std::signal(SIGPIPE, SIG_IGN); // Ignore SIGPIPE
}

Server::~Server() {}
TCPSocket *Server::getSocketByFD(int targetFD) const {
  for (size_t i = 0; i < _sockets.size(); i++) {
    if (_sockets[i]->getSocketFD() == targetFD)
      return _sockets[i];
  }
  return nullptr;
}

/*
   Store and set the client fd to non-blockant
*/
void Server::acceptConnection(TCPSocket *s) {
  if (s == nullptr)
    return;
  int newClient = accept(s->getSocketFD(), (sockaddr *)&s->getSocketAdress(),
                         &s->getSocketAddressLength());
  if (newClient < 0) {
    std::ostringstream ss;
    ss << "Server failed to accept incoming connection from ADDRESS: "
       << inet_ntoa(s->getSocketAdress().sin_addr)
       << "; PORT: " << ntohs(s->getSocketAdress().sin_port);
    exitWithFailure(ss.str());
  }
  if (fcntl(newClient, F_SETFL, O_NONBLOCK) < 0)
    exitWithFailure("Failed to set non-blocking mode for client socket");
  Client *n = new Client(newClient, READING);
  _clients.insert(std::make_pair(newClient, n));

  struct kevent event;
  EV_SET(&event, newClient, EVFILT_READ, EV_ADD, 0, 0, nullptr);
  if (kevent(_kqueue_fd, &event, 1, nullptr, 0, nullptr) == -1)
    exitWithFailure(
        "Error occured when we try to add an event to kevent (client)");
}
/*
  Init servers socket to listen clients
*/
void Server::startToListenClients() {
  for (size_t i = 0; i < _sockets.size(); i++) {
    _sockets[i]->initSocket();
    std::cout << "Server fd: " << _sockets[i]->getSocketFD() << std::endl;
    if (listen(_sockets[i]->getSocketFD(), 0) < 0)
      exitWithFailure("Socket listen failed");
    std::ostringstream ss;
    ss << "\n*** Listening on ADDRESS: "
       << inet_ntoa(_sockets[i]->getSocketAdress().sin_addr)
       << " PORT: " << ntohs(_sockets[i]->getSocketAdress().sin_port)
       << " ***\n";
    log(ss.str());


    struct kevent event;
    EV_SET(&event, _sockets[i]->getSocketFD(), EVFILT_READ, EV_ADD, 0, 0,
           nullptr);
    if (kevent(_kqueue_fd, &event, 1, nullptr, 0, nullptr) == -1)
      exitWithFailure(
          "Error occured when we try to add an event to kevent (TCPSocket)");
  }
}

void printVec(std::vector<pollfd> vec) {
  std::cout << "POLLFDS -> " << vec.size() << std::endl;
  for (size_t i = 0; i < vec.size(); i++) {
    std::cout << "\t" << vec[i].fd << std::endl;
  }
}

/*
 * Checks whether the fd comes from servers and not client
 */
bool Server::isServerSocketFD(int fd) {
  for (size_t i = 0; i < _sockets.size(); i++) {
    if (_sockets[i]->getSocketFD() == fd)
      return true;
  }
  return false;
}

/*
 * Main loop that manages all sockets
 */
void Server::runServers(void) {
  int byteReceived;

  _kqueue_fd = kqueue();
  if (_kqueue_fd == -1)
    exitWithFailure("Error with kqueue()");
  struct kevent events[1000];

  startToListenClients();

  while (true) {
    if (stopListening)
      break;

    int nfds = kevent(_kqueue_fd, nullptr, 0, events, 1000, nullptr);
    if (nfds == -1) {
      if (errno == EINTR)
        continue; // handle signal interruption
      exitWithFailure("Error with kevent() in loop");
    }

    if (stopListening)
      break;
    for (int i = 0; i < nfds; i++) {
      if (_clients.count(events[i].ident) && events[i].flags & EV_ERROR) {
        log("Fd event error");
        removeClient(events[i].ident);
      } else if (events[i].filter == EVFILT_READ) {
        if (_clients.count(events[i].ident)) {
          Client *client = _clients.at(events[i].ident);
          byteReceived = client->readRequest();
          if (byteReceived > 0) {
            struct kevent evSet;
            EV_SET(&evSet, events[i].ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0,
                   0, nullptr);
            if (kevent(_kqueue_fd, &evSet, 1, nullptr, 0, nullptr) == -1) {
              exitWithFailure("Failed to modify client socket in kqueue");
            }

          } else if (byteReceived == 0) {
            this->removeClient(events[i].ident);
          } else if (byteReceived < 0 && errno != EWOULDBLOCK &&
                     errno != EAGAIN) {
            log(strerror(errno));
            log("Failed to read bytes from client socket connection");
            this->removeClient(events[i].ident);
          }

        } else {
          TCPSocket *server = getSocketByFD(events[i].ident);
          acceptConnection(server);
        }
      } else if (_clients.count(events[i].ident) &&
                 events[i].filter == EVFILT_WRITE) {
        Client *client = _clients.at(events[i].ident);
        if (!client)
          exitWithFailure("Client doesn't exist !");
        handleResponse(client);
      } else {
        log("Does not match nothing");
        std::cout << events[i].ident << " -> " << events[i].flags << " -> "
                  << events[i].filter << std::endl;
      }
    }
  }
  //   std::cout << "Clients:" << std::endl;
  //  std::map<unsigned short, Client *>::iterator it;
  //  for (it = _clients.begin(); it != _clients.end(); it++) {
  //    std::cout << it->second->getFd() << std::endl;
  //    std::cout << it->second->getDataSent() << std::endl;
  //    std::cout << it->second->getRequest() << std::endl;
  //  }
  closeAllSockets();
}

void Server::handleResponse(Client *client) {
  client->sendResponse();
  if (client->getDataSent() == -1) {
    if (errno == EPIPE || errno == ECONNRESET) {
      log("Client disconnected or reset connection");
      removeClient(client->getFd());
    } else {
      log("Failed to send response to client");
      log(strerror(errno));
    }
  }
  if (client->getDataSent() <= 0
      /* && client->getRequest().getConnection().compare("keep-alive") == 0*/) {
    removeClient(client->getFd());
  }
}

void Server::closeAllSockets() const {
  for (size_t i = 0; i < _sockets.size(); i++)
    delete _sockets[i];
}

void Server::removeClient(int keyFD) {
  std::map<unsigned short, Client *>::iterator element = _clients.find(keyFD);
  if (element == _clients.end())
    return;
  struct kevent event;
  EV_SET(&event, keyFD, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
  if (kevent(_kqueue_fd, &event, 1, nullptr, 0, nullptr) == -1) {
    log("Failed to delete event from kqueue");
  }

  close(keyFD);
  _clients.erase(element);
  delete element->second;
}
