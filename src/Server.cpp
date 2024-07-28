#include "../inc/Server.hpp"
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <sys/event.h>

// Global variable to control the server shutdown
static volatile bool stopListening = false;

// Log a message to the standard output
void log(const std::string &message) { std::cout << message << std::endl; }

// Exit the program with a failure message
void exitWithFailure(std::string s)
{
  std::cerr << s << std::endl;
  exit(1);
}

// Signal handler to stop the server on SIGINT (Ctrl+C)
void handleSignal(int sig)
{
  if (sig == SIGINT)
    stopListening = true;
}

// Constructor for Server class
Server::Server(std::vector<TCPSocket *> s) : _sockets(s), _clients()
{
  std::signal(SIGINT, handleSignal);
  std::signal(SIGPIPE, SIG_IGN); // Ignore SIGPIPE
}

// Destructor for Server class
Server::~Server() {}
TCPSocket *Server::getSocketByFD(int targetFD) const
{
  for (size_t i = 0; i < _sockets.size(); i++)
  {
    if (_sockets[i]->getSocketFD() == targetFD)
      return _sockets[i];
  }
  return NULL;
}
// Accept a new client connection
void Server::acceptConnection(TCPSocket *s)
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
  //Server::updateEpoll(_epoll_fd, EPOLL_CTL_ADD, newClient, NULL);
  updateKqueue(_kqueue_fd, EV_ADD, newClient);
}

void Server::updateKqueue(int kqFD, short action, int targetFD)
{
  if (action == EV_ADD)
  {
    struct kevent event;
    EV_SET(&event, targetFD, EVFILT_READ, EV_ADD, 0, 0, nullptr);
    if (kevent(kqFD, &event, 1, nullptr, 0, nullptr) == -1)
    {
      exitWithFailure("kevent ev add problem");
    }
  }
  else if (action == (EV_ADD | EV_ENABLE))
  {
    struct kevent evSet;
    EV_SET(&evSet, targetFD, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0,
           0, nullptr);
    if (kevent(kqFD, &evSet, 1, nullptr, 0, nullptr) == -1) {
      exitWithFailure("Failed to modify client socket in kqueue");
    }
  }
  else if (action == EV_DELETE)
  {
    struct kevent event;
    EV_SET(&event, targetFD, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
    if (kevent(kqFD, &event, 1, nullptr, 0, nullptr) == -1) {
      log("Failed to delete event from kqueue");
    }
  }
}

// Initialize server sockets to listen to clients
void Server::startToListenClients()
{
  // Iterate over all the server sockets
  for (size_t i = 0; i < _sockets.size(); i++)
  {
    // Initialize the socket
    _sockets[i]->initSocket();

    // Set the socket to listen for incoming connections
    if (listen(_sockets[i]->getSocketFD(), 0) < 0)
      exitWithFailure("Socket listen failed");

    // Log the address and port the server is listening on
    std::ostringstream ss;
    ss << "\n*** Listening on ADDRESS: "
       << inet_ntoa(_sockets[i]->getSocketAdress().sin_addr)          // Convert the IP address to a string
       << " PORT: " << ntohs(_sockets[i]->getSocketAdress().sin_port) // Convert the port number to host byte order
       << " ***\n";
    log(ss.str());
    if (fcntl(_sockets[i]->getSocketFD(), F_SETFL, O_NONBLOCK) < 0)
      exitWithFailure("Failed to set non-blocking mode for client socket");
     updateKqueue(_kqueue_fd, EV_ADD, _sockets[i]->getSocketFD());
  }
}

// Check if the file descriptor belongs to a server socket
bool Server::isServerSocketFD(int fd)
{
  for (size_t i = 0; i < _sockets.size(); i++)
  {
    if (_sockets[i]->getSocketFD() == fd)
      return true;
  }
  return false;
}

void Server::handleClientRequest(int fd)
{
  Client *client = _clients.at(fd);
  int byteReceived = client->readRequest();
  if (byteReceived > 0)
  {
    Server::updateKqueue(_kqueue_fd, (EV_ADD | EV_ENABLE), fd);
  }
  else
  {
    this->removeClient(fd);
  }
}

// Main loop to manage all sockets
void Server::runServers(void)
{

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

    for (int i = 0; i < nfds; i++)
    {
      int fd_triggered = events[i].ident;
      short ev = events[i].filter;
      if (_clients.count(fd_triggered) && events[i].flags & EV_ERROR)
        this->removeClient(fd_triggered);
      else if (ev == EVFILT_READ)
      {
        if (_clients.count(fd_triggered)) {
          handleClientRequest(fd_triggered);
        }
        else
        {
          TCPSocket *server = getSocketByFD(fd_triggered);
          acceptConnection(server);
        }
      }
      else if (_clients.count(fd_triggered) && ev == EVFILT_WRITE) {
        Client *client = _clients.at(fd_triggered);
        handleResponse(client);
      }
    }
  }
  closeAllSockets();
}

// Handle client response
void Server::handleResponse(Client *client)
{
  client->sendResponse();
  
  if (client->getDataSent() <= 0)
  {
    removeClient(client->getFd());
  }
}

// Close all server sockets
void Server::closeAllSockets()
{
  for (size_t i = 0; i < _sockets.size(); i++)
  {
    delete _sockets[i];
  }
  std::map<unsigned short, Client *>::iterator it;
  for (it = _clients.begin(); it != _clients.end(); it++)
  {
    delete it->second;
  }
}

// Remove a client from the server
void Server::removeClient(int keyFD)
{
  std::map<unsigned short, Client *>::iterator element = _clients.find(keyFD);
  if (element == _clients.end())
    return;
  Server::updateKqueue(_kqueue_fd, EV_DELETE, keyFD);
  std::cout << "[REMOVE CLIENT]: FD -> " << keyFD << " on " << element->second->getRequest().getHost() << std::endl;
  close(keyFD);
  _clients.erase(element);
  delete element->second;
}
