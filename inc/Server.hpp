#pragma once

#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "Request.hpp"

#define BUFFER_SIZE 30720

class Server
{
private:
    std::string _ipAddress;
    int _port;
    int _socket;
    int _newSocket;
    struct sockaddr_in _socketAddress;
    unsigned int _socketAddressLength;

public:
    Server(std::string ipAddress, int port);
    ~Server();
    bool startServer();
    void closeServer() const;
    void startListen();
    void acceptConnection(int &);
    std::string buildResponse(std::string);
    void sendResponse(std::string);
};