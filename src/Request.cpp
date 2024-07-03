#include "Request.hpp"

Request::Request(std::string body)
{
	std::stringstream	request(body);
	std::string 		method;
	std::string 		path;

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

std::string	Request::getMethod() const
{
	if (_method == GET)
		return "GET";
	else if (_method == POST)
		return "POST";
	else if (_method == DELETE)
		return "DELETE";
	return "UNKNOWN";
}

std::string	Request::getPath() const { return this->_path; }


std::string	Request::handleRequest(const Request &r)
{
	(void) r;
	std::string		filePath = "./static/index.html";
	std::ifstream		inFile(filePath.c_str());
	std::stringstream	buffer;	

	if (inFile.is_open())
	{
		buffer << inFile.rdbuf();
		inFile.close();
	}
	else
	{
		std::cerr << "Unable to open file" << filePath << std::endl;
		return "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: 0\n\n";
	}
	std::string	htmlContent = buffer.str();
	std::ostringstream	ss;
	ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " 
		<< htmlContent.size() << "\n\n"
		<< htmlContent;
    	return ss.str();
}

