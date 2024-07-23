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
TCPSocket *Server::getSocketByFD(int targetFD) const {
  for (size_t i = 0; i < _sockets.size(); i++) {
    if (_sockets[i]->getSocketFD() == targetFD)
      return _sockets[i];
  }
  return NULL;
}
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
  
	struct  epoll_event ev;
	ev.data.fd = newClient;
	ev.events = EPOLLIN;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, newClient, &ev) == -1) {
		exitWithFailure("epoll ctl problem");
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
	<< inet_ntoa(_sockets[i]->getSocketAdress().sin_addr) // Convert the IP address to a string
	<< " PORT: " << ntohs(_sockets[i]->getSocketAdress().sin_port) // Convert the port number to host byte order
	<< " ***\n";
	log(ss.str());


	struct  epoll_event ev;
	ev.data.fd = _sockets[i]->getSocketFD();
	ev.events = EPOLLIN;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _sockets[i]->getSocketFD(), &ev) == -1) {
		exitWithFailure("epoll ctl problem");
	}
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
	int MAX_EVENT = 200;
	struct epoll_event events[MAX_EVENT];

	_epoll_fd = epoll_create1(0); 
	if (_epoll_fd == -1)
	        exitWithFailure("Error with Epoll_fd");

	// Initialize server sockets to listen for client connections
	startToListenClients();

	while (true)
	{
	  if (stopListening)
	    break;

	  log("Waiting !");
	  // Poll the sockets to check for events
	  int nfds = epoll_wait(_epoll_fd, events, MAX_EVENT, -1);
	  if (stopListening)
	    break;
	  if (nfds == -1)
	        exitWithFailure("Error with epoll_wait");
	  log("New events !");

	  // Iterate over pollfds to handle events
	  for (int i = 0; i < nfds; i++) {
		  int fd_triggered = events[i].data.fd;
		  std::cout << "Fd: " << fd_triggered << " for " << events[i].events << std::endl;
		  if (events[i].events & EPOLLIN)
		  {
			  if (_clients.count(fd_triggered))
			  {
			  	Client *client = _clients.at(fd_triggered);
				byteReceived = client->readRequest();
				if (byteReceived > 0)
				{
					events[i].events = EPOLLOUT;
					if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd_triggered, &events[i]) == -1) {
						exitWithFailure("epoll ctl_mod problem");
					}
					 
				} else if (byteReceived == 0){
					this->removeClient(fd_triggered);
				} else if (byteReceived < 0 && errno != EWOULDBLOCK && errno != EAGAIN) {
					log(strerror(errno));
					log("Failed to read bytes from client socket connection");
					this->removeClient(fd_triggered);
				}

			  } else {
				  TCPSocket *server = getSocketByFD(fd_triggered);
				  std::cout << "server: " << server->getSocketFD() << std::endl;
				  acceptConnection(server);
			  }
		  } else if (_clients.count(fd_triggered) && events[i].events & EPOLLOUT) {
			  Client *client = _clients.at(fd_triggered);
			  handleResponse(client);
		  } else 
			  log("Unexpected log !");
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
  if (client->getDataSent() <= 0) {
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
  // TODO REMOVE CLIENT FROM EPOLL EVENT
  if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, keyFD, NULL) == -1) {
	  exitWithFailure("epoll ctl_del problem");
  }
  close(keyFD);
  _clients.erase(element);
  delete element->second;
}
