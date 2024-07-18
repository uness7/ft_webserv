#include "../inc/Server.hpp"
#include "../inc/Config.hpp"
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <cstring>
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

Server::Server(std::vector<TCPSocket> &s) : _sockets(s) {
  std::signal(SIGINT, handleSignal);
}

Server::~Server() {}

void Server::acceptConnection(TCPSocket &s) {
  s._newSocket =
      accept(s._socket, (sockaddr *)&s._socketAddress, &s._socketAddressLength);
  if (s._newSocket < 0) {
    std::ostringstream ss;
    ss << "Server failed to accept incoming connection from ADDRESS: "
       << inet_ntoa(s._socketAddress.sin_addr)
       << "; PORT: " << ntohs(s._socketAddress.sin_port);
    exitWithFailure(ss.str());
  }
}
void Server::initToListen() {
  for (size_t i = 0; i < _sockets.size(); i++) {
    _sockets[i].initSocket();
    if (listen(_sockets[i]._socket, 20) < 0)
      exitWithFailure("Socket listen failed");
    std::ostringstream ss;
    ss << "\n*** Listening on ADDRESS: "
       << inet_ntoa(_sockets[i]._socketAddress.sin_addr)
       << " PORT: " << ntohs(_sockets[i]._socketAddress.sin_port) << " ***\n\n";
    log(ss.str());
  }
}

void Server::startListen(void) {
  int byteReceived;

  initToListen();

  struct pollfd fds[_sockets.size()];
  for (size_t i = 0; i < _sockets.size(); i++) {
    fds[i].fd = _sockets[i]._socket;
    fds[i].events = POLLIN;
  }

  while (true) {
    log("====== Waiting for a new connection ======\n");
    if (stopListening)
      break;
    int ret = poll(fds, _sockets.size(), -1);
    if (stopListening)
      break;
    if (ret < 0)
      exitWithFailure("Poll error");

    for (size_t i = 0; i < _sockets.size(); i++) {
      if (fds[i].revents & POLLIN) {
        acceptConnection(_sockets[i]);
        std::cout << _sockets[i]._newSocket << std::endl;
        char buffer[BUFFER_SIZE];
        memset(&buffer, 0, BUFFER_SIZE);
        byteReceived = read(_sockets[i]._newSocket, buffer, BUFFER_SIZE);
        if (byteReceived < 0)
          exitWithFailure("Failed to read bytes from client socket connection");

        std::ostringstream ss;
        ss << "------ Received Request from client ------\n"
           << buffer << std::endl;
        log(ss.str());

        sendResponse(_sockets[i], buildResponse(std::string(buffer)));

        close(_sockets[i]._newSocket);
      }
    }
  }
  closeAllSockets();
}

void Server::closeAllSockets() const {
  for (size_t i = 0; i < _sockets.size(); i++)
    _sockets[i].closeServer();
}

std::string get_mime_type(const std::string &file_path) {
  if (file_path.length() >= 5 &&
      file_path.substr(file_path.length() - 5) == ".html") {
    return "text/html";
  } else if (file_path.length() >= 4 &&
             file_path.substr(file_path.length() - 4) == ".css") {
    return "text/css";
  } else if (file_path.length() >= 4 &&
             file_path.substr(file_path.length() - 4) == ".jpg") {
    return "image/jpg";
  } else if (file_path.length() >= 3 &&
             file_path.substr(file_path.length() - 3) == ".js") {
    return "application/javascript";
  } else {
    return "text/plain";
  }
}

std::string Server::buildResponse(std::string req) {
  Request r(req);
  std::string path = r.getPath();
  std::string mimetype = get_mime_type(path);
  std::ifstream inFile(
      std::string("." + path)
          .c_str()); // Problem when path is a directory ("/" | "/static")
  std::stringstream buffer;
  if (inFile.is_open()) {
    buffer << inFile.rdbuf();
    inFile.close();
  } else {
    std::cerr << "Unable to open file " << path << std::endl;
    return "HTTP/1.1 302 Found\nLocation: /static/index.html\n\n"; // Redirect
                                                                   // the
                                                                   // browser
                                                                   // to
                                                                   // "/static/index.html"
                                                                   // path to
                                                                   // display
                                                                   // the
                                                                   // error
                                                                   // page.
  }
  std::string fileRequested = buffer.str();
  std::ostringstream ss;
  ss << "HTTP/1.1 200 OK\nContent-Type: " << mimetype
     << "\nContent-Length: " << fileRequested.size() << "\n\n"
     << fileRequested;
  return ss.str();
}

void Server::sendResponse(const TCPSocket s, std::string response) {
  long bytesSent;

  bytesSent = write(s._newSocket, response.c_str(), response.size());
  if (bytesSent == static_cast<long>(response.size()))
    log("------ Server Response sent to client ------\n");
  else
    log("Error sending response to client");
}
