#include "Request.hpp"

Request::Request(std::string body)
{
	std::stringstream request(body);
	std::string method;
	std::string path;

	request >> method;
	request >> path;
	_method = strToHttpMethod(method);
	_path = path;
	std::cout << "METHOD: " << method << " -> " << _method << std::endl;
	std::cout << "PATH: " << _path << std::endl;
}

Request::~Request() {}

HTTP_METHOD Request::strToHttpMethod(std::string s) const
{
	if (s == "GET")
		return GET;
	else if (s == "POST")
		return POST;
	else if (s == "DELETE")
		return DELETE;
	return UNKNOWN;
}

std::string Request::getMethod() const
{
	if (_method == GET)
		return "GET";
	else if (_method == POST)
		return "POST";
	else if (_method == DELETE)
		return "DELETE";
	return "UNKNOWN";
}

std::string Request::getPath() const { return this->_path; }
