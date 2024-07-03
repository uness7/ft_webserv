#pragma once
#include <iostream>
#include <sstream>
#include <fstream>

enum HTTP_METHOD
{
	GET,
	POST,
	DELETE,
	UNKNOWN,
};

class Request
{
	private:
		HTTP_METHOD _method;
		std::string _path;
		HTTP_METHOD strToHttpMethod(std::string) const;

	public:
		Request(std::string);
		~Request();

		std::string getMethod() const;
		std::string getPath() const;

		/* Member Functions */
		std::string	handleRequest(const Request & req) ;
};
