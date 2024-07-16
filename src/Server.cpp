#include "../inc/Server.hpp"
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sys/poll.h>
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

Server::Server(std::vector<TCPSocket *> &s) : _sockets(s) {
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
  s->addClient(newClient);
  if (fcntl(newClient, F_SETFL, O_NONBLOCK) < 0)
    exitWithFailure("Failed to set non-blocking mode for client socket");
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
    log("====== Waiting for a new connection ======\n");
    if (stopListening)
      break;

    pollfds.clear();
    for (size_t i = 0; i < _sockets.size(); i++) {
      struct pollfd pfd = {.fd = _sockets[i]->getSocketFD(), .events = POLLIN};
      pollfds.push_back(pfd);
      const std::vector<int> &clients = _sockets[i]->getClientsFD();
      for (size_t j = 0; j < clients.size(); j++) {
        struct pollfd cpfd = {.fd = clients[j], .events = POLLIN};
        pollfds.push_back(cpfd);
      }
    }
    int ret = poll(pollfds.data(), pollfds.size(), -1);
    if (stopListening)
      break;
    if (ret < 0)
      exitWithFailure("Poll error");

    for (size_t i = 0; i < pollfds.size(); i++) {
      if (pollfds[i].revents & POLLIN) {
        if (isServerSocketFD(
                pollfds[i]
                    .fd)) { // If the current socket is from server and
                            // has an event POLLIN (Incomming
                            // connection), we accept connections from clients
          acceptConnection(_sockets[i]);
        } else { // We respond to the new client stored and ready for read and
                 // write function.
          int clientFD = pollfds[i].fd;
          char buffer[BUFFER_SIZE];
          memset(&buffer, 0, BUFFER_SIZE);
          byteReceived = read(clientFD, buffer, BUFFER_SIZE);
          if (byteReceived > 0) {
            Request req(buffer);
            sendResponse(clientFD, buildResponse(req));
            std::cout << req << std::endl;
            if (req.getConnection() != "keep-alive") {
              close(clientFD);
              for (size_t i = 0; i < _sockets.size(); i++) {
                _sockets[i]->removeClient(clientFD);
              }
            }
          } else if (byteReceived == 0) {
            close(clientFD);
            for (size_t i = 0; i < _sockets.size(); i++) {
              _sockets[i]->removeClient(clientFD);
            }
          } else if (byteReceived < 0 && errno != EWOULDBLOCK) {
            exitWithFailure(
                "Failed to read bytes from client socket connection");
          }
        }
      }
    }
  }
  closeAllSockets();
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

ssize_t writeAll(int fd, const char *buf, size_t count) {
  size_t totalWritten = 0;
  struct pollfd pfd = {.fd = fd, .events = POLLOUT, .revents = 0};
  poll(&pfd, 1, -1);
  while (totalWritten < count) {
    if (pfd.revents & POLLOUT) {
      ssize_t bytesWritten =
          write(fd, buf + totalWritten, count - totalWritten);
      if (bytesWritten < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
          std::cout << "Write function wait until the fd is ready to send datas"
                    << std::endl;
          poll(&pfd, 1, -1);
        } else {
          std::cerr << "error on write func -> " << strerror(errno)
                    << std::endl;
          return -1;
        }
      } else {
        totalWritten += bytesWritten;
      }
    }
  }
  return totalWritten;
}

void Server::sendResponse(int clientFD, std::string response) {
  long bytesSent;

  bytesSent = writeAll(clientFD, response.c_str(), response.size());
  if (bytesSent == static_cast<long>(response.size()))
    log("------ Server Response sent to client ------\n");
  else
    log("Error sending response to client");
}
