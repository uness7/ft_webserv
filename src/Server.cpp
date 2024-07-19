#include "../inc/Server.hpp"
#include <algorithm>
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
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

Server::Server(std::vector<TCPSocket *> &s) : _sockets(s), _clients() {
  std::signal(SIGINT, handleSignal);
}

Server::~Server() {}

/*
   Store and set the client fd to non-blockant
*/
void Server::acceptConnection(TCPSocket *s) {
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
}

/*
  Init servers socket to listen clients
*/
void Server::startToListenClients() {
  for (size_t i = 0; i < _sockets.size(); i++) {
    _sockets[i]->initSocket();
    if (listen(_sockets[i]->getSocketFD(), 0) < 0)
      exitWithFailure("Socket listen failed");
    std::ostringstream ss;
    ss << "\n*** Listening on ADDRESS: "
       << inet_ntoa(_sockets[i]->getSocketAdress().sin_addr)
       << " PORT: " << ntohs(_sockets[i]->getSocketAdress().sin_port)
       << " ***\n";
    log(ss.str());
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

  startToListenClients();

  std::vector<pollfd> pollfds;

  while (true) {
    if (stopListening)
      break;

    pollfds.clear();
    for (size_t i = 0; i < _sockets.size(); i++) {
      struct pollfd pfd = {.fd = _sockets[i]->getSocketFD(), .events = POLLIN};
      pollfds.push_back(pfd);
    }
    std::map<unsigned short, Client *>::const_iterator it;
    for (it = _clients.begin(); it != _clients.end(); it++) {
      const short ev =
          it->second->getWaitingStatus() == READING ? POLLIN : POLLOUT;
      struct pollfd cpfd = {.fd = it->first, .events = ev};
      pollfds.push_back(cpfd);
    }
    int ret = poll(pollfds.data(), pollfds.size(), 500);
    if (stopListening)
      break;
    if (ret < 0)
      exitWithFailure("Poll error");

    for (size_t i = 0; i < pollfds.size(); i++) {
      if (pollfds[i].revents & POLLIN) {
        if (isServerSocketFD(pollfds[i].fd)) {
          acceptConnection(_sockets[i]);
        } else if (_clients.count(pollfds[i].fd)) {
          Client *client = _clients.at(pollfds[i].fd);
          if (!client)
            exitWithFailure("Client doesn't exist !");
          byteReceived = readRequest(client);
          if (byteReceived == 0) {
            this->removeClient(pollfds[i].fd);
          } else if (byteReceived < 0 && errno != EWOULDBLOCK) {
            log(strerror(errno));
            log("Failed to read bytes from client socket connection");
            this->removeClient(pollfds[i].fd);
          }
        }
      } else if (pollfds[i].revents & POLLOUT) {
        Client *client = _clients.at(pollfds[i].fd);
        if (!client)
          exitWithFailure("Client doesn't exist !");
        handleResponse(client);
      } else if (_clients.count(pollfds[i].fd) &&
                 (pollfds[i].revents & POLLERR ||
                  pollfds[i].revents & POLLHUP ||
                  pollfds[i].revents & POLLNVAL)) {
        log("This fd has error !");
        removeClient(pollfds[i].fd);
      } else {
        log("Unknown");
        std::cout << pollfds[i].fd << " -> " << pollfds[i].events << " -> "
                  << pollfds[i].revents << std::endl;
      }
    }
  }
  closeAllSockets();
}

void Server::handleResponse(Client *client) {
  sendResponse(client);
  if (client->getDataSent() == 0
      /* && client->getRequest().getConnection().compare("keep-alive") == 0*/) {
    removeClient(client->getFd());
  }
}

int Server::readRequest(Client *client) {
  char buffer[BUFFER_SIZE];
  memset(&buffer, 0, BUFFER_SIZE);
  int byteReceived = read(client->getFd(), buffer, BUFFER_SIZE);
  client->setRequest(buffer);
  client->setWaitingStatus(WRITING);
  return byteReceived;
}

void Server::closeAllSockets() const {
  for (size_t i = 0; i < _sockets.size(); i++)
    delete _sockets[i];
}

std::string Server::buildResponse(const Request r) {
  std::string path = r.getPath();
  std::ifstream inFile(
      std::string("." + path)
          .c_str()); // Problem when path is a directory ("/" | "/static")
  std::stringstream buffer;
  if (inFile.is_open()) {
    buffer << inFile.rdbuf();
    inFile.close();
  } else {
    std::cerr << "Unable to open file " << path << std::endl;
    return "HTTP/1.1 302 Found\nLocation: /static/index.html\n\n";
  }
  std::string fileRequested = buffer.str();
  std::ostringstream ss;
  ss << "HTTP/1.1 200 OK\nContent-Type: " << r.getMimeType()
     << "\nContent-Length: " << fileRequested.size() << "\n\n"
     << fileRequested;
  return ss.str();
}

void Server::sendResponse(Client *client) {
  long bytesSent;
  std::string response = buildResponse(client->getRequest());

  bytesSent = write(client->getFd(), response.c_str() + client->getDataSent(),
                    response.size() - client->getDataSent());
  if (bytesSent + client->getDataSent() == static_cast<long>(response.size())) {
    log("------ Server Response sent to client ------\n");
    client->setWaitingStatus(READING);
    client->setDataSent(0);
  } else {
    log("---- Server need multiple time to send fully response ----");
    client->setDataSent(client->getDataSent() + bytesSent);
    client->setWaitingStatus(WRITING);
  }
}

void Server::removeClient(int keyFD) {
  std::map<unsigned short, Client *>::iterator element = _clients.find(keyFD);
  if (element == _clients.end())
    return;
  close(keyFD);
  _clients.erase(element);
  delete element->second;
}
