#include "Server.hpp"
#include <cstring>
#include <cstdlib>

void log(const std::string &message)
{
    std::cout << message << std::endl;
}

void exitWithFailure(std::string s)
{
    std::cerr << s << std::endl;
    exit(1);
}

Server::Server(std::string ipAddress, int port) : _ipAddress(ipAddress), _port(port), _newSocket(),
                                                  _socketAddress(),
                                                  _socketAddressLength(sizeof(_socketAddress))
{
    _socketAddress.sin_family = AF_INET;
    _socketAddress.sin_port = htons(_port);
    _socketAddress.sin_addr.s_addr = inet_addr(ipAddress.c_str());
    if (!startServer())
        exitWithFailure("Failed to start server with PORT: ");
}

Server::~Server()
{
    closeServer();
    std::cout << "Server closed" << std::endl;
}

void Server::closeServer() const
{
    close(_socket);
    close(_newSocket);
    exit(0);
}

bool Server::startServer()
{
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0)
    {
        exitWithFailure("Cannot create socket");
        return false;
    }

    if (bind(_socket, (sockaddr *)&_socketAddress, _socketAddressLength) < 0)
    {
        exitWithFailure("Cannot connect socket to address");
        return false;
    }

    return true;
}

void Server::acceptConnection(int &newSocket)
{
    newSocket = accept(_socket, (sockaddr *)&_socketAddress, &_socketAddressLength);
    if (newSocket < 0)
    {
        std::ostringstream ss;
        ss << "Server failed to accept incoming connection from ADDRESS: "
           << inet_ntoa(_socketAddress.sin_addr) << "; PORT: "
           << ntohs(_socketAddress.sin_port);
        exitWithFailure(ss.str());
    }
}

void Server::startListen()
{
    if (listen(_socket, 20) < 0)
        exitWithFailure("Socket listen failed");

    std::ostringstream ss;
    ss << "\n*** Listening on ADDRESS: "
       << inet_ntoa(_socketAddress.sin_addr)
       << " PORT: " << ntohs(_socketAddress.sin_port)
       << " ***\n\n";
    log(ss.str());

    int byteReceived;
    // WE NEED TO ADD POLL FUNCTION TO MAKE THE SERVER NON-BLOCKING
    while (true)
    {
        log("====== Waiting for a new connection ======\n\n\n");
        acceptConnection(_newSocket);
        char buffer[BUFFER_SIZE];
        memset(&buffer, 0, BUFFER_SIZE);
        byteReceived = read(_newSocket, buffer, BUFFER_SIZE);
        if (byteReceived < 0)
            exitWithFailure("Failed to read bytes from client socket connection");

        std::ostringstream ss;
        ss << "------ Received Request from client ------\n\n"
           << buffer << std::endl;
        log(ss.str());

        sendResponse(buildResponse(std::string(buffer)));

        close(_newSocket);
    }
}

std::string Server::buildResponse(std::string req)
{
    Request r(req);

    std::ostringstream html;
    html << "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :)</p><p>You requested our server with method -> " << r.getMethod() << "</p> <p>Your requested path is " << r.getPath() << " </p></body></html>";
    std::string htmlFile = html.str();
    std::ostringstream ss;
    ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << htmlFile.size() << "\n\n"
       << htmlFile;

    return ss.str();
}

void Server::sendResponse(std::string response)
{
    long bytesSent;

    bytesSent = write(_newSocket, response.c_str(), response.size());

    if (bytesSent == static_cast<long>(response.size()))
    {
        log("------ Server Response sent to client ------\n\n");
    }
    else
    {
        log("Error sending response to client");
    }
}
