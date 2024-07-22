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

// Global variable to control the server shutdown
static volatile bool stopListening = false;

// Log a message to the standard output
void log(const std::string &message) { std::cout << message << std::endl; }

// Exit the program with a failure message
void exitWithFailure(std::string s) {
  std::cerr << s << std::endl;
  exit(1);
}

// Signal handler to stop the server on SIGINT (Ctrl+C)
void handleSignal(int sig) {
  if (sig == SIGINT)
    stopListening = true;
}

// Constructor for Server class
Server::Server(std::vector<TCPSocket *> s) : _sockets(s), _clients() {
  std::signal(SIGINT, handleSignal);
}

// Destructor for Server class
Server::~Server() {}

// Accept a new client connection
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
       << inet_ntoa(_sockets[i]->getSocketAdress().sin_addr) // Convert the IP address to a string
       << " PORT: " << ntohs(_sockets[i]->getSocketAdress().sin_port) // Convert the port number to host byte order
       << " ***\n";
    log(ss.str());
  }
}


// Print the file descriptors in the poll vector
void printVec(std::vector<pollfd> vec) {
  std::cout << "POLLFDS -> " << vec.size() << std::endl;
  for (size_t i = 0; i < vec.size(); i++) {
    std::cout << "\t" << vec[i].fd << std::endl;
  }
}

// Check if the file descriptor belongs to a server socket
bool Server::isServerSocketFD(int fd) {
  for (size_t i = 0; i < _sockets.size(); i++) {
    if (_sockets[i]->getSocketFD() == fd)
      return true;
  }
  return false;
}

// Main loop to manage all sockets
void Server::runServers(void)
{
  int byteReceived;

  // Initialize server sockets to listen for client connections
  startToListenClients();

  std::vector<pollfd> pollfds;

  while (true)
  {
    if (stopListening)
      break;

    // Clear the pollfds vector for the new polling cycle
    pollfds.clear();

    // Add server sockets to the pollfds vector
    for (size_t i = 0; i < _sockets.size(); i++) {
      struct pollfd pfd = {.fd = _sockets[i]->getSocketFD(), .events = POLLIN};
      pollfds.push_back(pfd);
    }

    // Add client sockets to the pollfds vector
    std::map<unsigned short, Client *>::const_iterator it;
    for (it = _clients.begin(); it != _clients.end(); it++)
    {
      const short ev = it->second->getWaitingStatus() == READING ? POLLIN : POLLOUT;
      struct pollfd cpfd = {.fd = it->first, .events = ev};
      pollfds.push_back(cpfd);
    }

    // Poll the sockets to check for events
    int ret = poll(pollfds.data(), pollfds.size(), -1);
    if (stopListening)
      break;
    if (ret < 0)
      exitWithFailure("Poll error");

    // Iterate over pollfds to handle events
    for (size_t i = 0; i < pollfds.size(); i++) {
      if (pollfds[i].revents & POLLIN) {  // If there's data to read
        if (isServerSocketFD(pollfds[i].fd)) {
          // Accept a new client connection if the event is on a server socket
          acceptConnection(_sockets[i]);
        } else if (_clients.count(pollfds[i].fd)) {
          // Read request from the client if the event is on a client socket
          Client *client = _clients.at(pollfds[i].fd);
          if (!client)
            exitWithFailure("Client doesn't exist !");
          byteReceived = client->readRequest();
          if (byteReceived == 0) {
            // Remove the client if the connection is closed
            this->removeClient(pollfds[i].fd);
          } else if (byteReceived < 0 && errno != EWOULDBLOCK) {
            log(strerror(errno));
            log("Failed to read bytes from client socket connection");
            this->removeClient(pollfds[i].fd);
          }
        }
      } else if (pollfds[i].revents & POLLOUT) {  // If the socket is ready to write
        Client *client = _clients.at(pollfds[i].fd);
        if (!client)
          exitWithFailure("Client doesn't exist !");
        // Send response to the client
        handleResponse(client);
      } else if (_clients.count(pollfds[i].fd) &&
                 (pollfds[i].revents & POLLERR ||
                  pollfds[i].revents & POLLHUP ||
                  pollfds[i].revents & POLLNVAL)) {
        // Handle errors on the client socket
        log("This fd has an error !");
        removeClient(pollfds[i].fd);
      } else {
        log("Unknown");
        std::cout << pollfds[i].fd << " -> " << pollfds[i].events << " -> "
                  << pollfds[i].revents << std::endl;
      }
    }
  }

  // Print information about remaining clients before closing all sockets
  std::cout << "Clients:" << std::endl;
  std::map<unsigned short, Client *>::iterator it;
  for (it = _clients.begin(); it != _clients.end(); it++) {
    std::cout << it->second->getFd() << std::endl;
    std::cout << it->second->getDataSent() << std::endl;
    std::cout << it->second->getRequest() << std::endl;
  }

  // Close all server sockets
  closeAllSockets();
}


// Handle client response
void Server::handleResponse(Client *client) {
  client->sendResponse();
  if (client->getDataSent() == 0) {
    removeClient(client->getFd());
  }
}

// Close all server sockets
void Server::closeAllSockets() const {
  for (size_t i = 0; i < _sockets.size(); i++)
    delete _sockets[i];
}

// Remove a client from the server
void Server::removeClient(int keyFD) {
  std::map<unsigned short, Client *>::iterator element = _clients.find(keyFD);
  if (element == _clients.end())
    return;
  close(keyFD);
  _clients.erase(element);
  delete element->second;
}
