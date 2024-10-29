#pragma once

#include <sys/socket.h>
#include <sys/unistd.h>
#include <unistd.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cstddef>

#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"

#define BUFFER_SIZE 4096

class Response;

class Client {
       private:
	unsigned short _fd;
	long _dataSent;
	Request _request;
	Response *_response;
	ServerConfig _config;

       public:
	Client(unsigned short, ServerConfig);
	Client(const Client &);
	Client &operator=(const Client &);
	~Client();

	unsigned short getFd() const;
	void setFd(unsigned short);
	long getDataSent() const;
	void setDataSent(long);
	Request &getRequest();
	long readRequest();
	const std::string getResponseToString() const;
	const Response *getResponse() const;
	void sendResponse();
	const ServerConfig getConfig() const;
	void clear();
};
