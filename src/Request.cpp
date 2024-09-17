/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzioual <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 14:14:41 by yzioual           #+#    #+#             */
/*   Updated: 2024/09/17 13:30:42 by otourabi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Request.hpp"

Request::Request()
	:
	_method(""),
	_path(""),
	_query(""),
	_mimetype(""),
	_body(),
	_headers(),
	_contentLength(0),
	_state(EMPTY)
{
	/* Default Constructor */
}

Request::~Request()
{
	/* Deconstructor */
}

Request::Request(const Request &cp)
{
	*this = cp;
}

Request	&Request::operator=(const Request &rhs)
{
	if (this != &rhs)
	{
		this->_path = rhs.getPath();
		this->_method = rhs.getMethod();
		this->_mimetype = rhs.getMimeType();
		this->_query = rhs._query;
		this->_body = rhs._body;
		this->_headers = rhs._headers;
		this->_contentLength = rhs._contentLength;
		this->_state = rhs._state;
	}
	return *this;
}

long	Request::handleFirstLineHeader(unsigned int socketFd)
{
	if (_state != EMPTY)
		return 1;
	std::cout << "STATE1: " << _state << std::endl;
	std::ostringstream 	lineStream;
	long			bytesRead = Utils::get_next_line(socketFd, lineStream);

	std::cout << bytesRead << " -> =" << lineStream.str() << "=" << std::endl;
	if (lineStream.str().size() == 1 && lineStream.str()[0] == '\n')
		return 0;
	if (bytesRead < 0 && lineStream.str().empty())
		return bytesRead;
	std::istringstream iss(lineStream.str());
	std::string method;
	std::string path;
	iss >> method >> path;
	setMethod(method);
	setPath(path);
	_state = HEADER;
	std::cout << "STATE2: " << _state << std::endl;
	return bytesRead;
}

void	Request::saveHeaderLine(std::string &line)
{
	std::size_t	found = line.find(":");

	if (found != std::string::npos)
	{
		std::string key = line.substr(0, found);
		std::string value = line.substr(found + 1);
		if (value[0] == ' ')
		  value.erase(0, 1);
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
		_headers.insert(std::make_pair(key, value));
		if (key == "content-length")
			_contentLength = atoll(value.c_str());
	}
}

bool	Request::checkHeaderLocation(ServerConfig &config)
{
	LocationConfig	target;

	short found = config.getLocationByPathRequested(_path, target);
	if (found < 0 || config.client_max_body_size < _contentLength)
		return false;

	if (getMethod() == "HEAD")
		return true;
	std::vector<std::string> &allowed_methods = target.allowed_methods;
	if (allowed_methods.size() &&
			std::find(
				allowed_methods.begin(),
				allowed_methods.end(),
				getMethod()) == allowed_methods.end())
		return false;
	return true;
}

void 	Request::parseHeader(std::string &header) {
}

long	Request::readFromSocket(unsigned int socketFd, ServerConfig &config) {
	(void)config;
	char buffer[4096];
	ssize_t bytesRead;	
	std::ostringstream req;

	while ((bytesRead = recv(socketFd, buffer, 4096, 0)) > 0) {
		ssize_t i = 0;
		while (i < bytesRead) {
			req << buffer[i++];
		}
	}

	std::size_t position = req.str().find("\r\n\r\n");
	if (position == std::string::npos)
		return -1;
	std::string header = req.str().substr(0, position);
	parseHeader(header);

	std::cout << "position: " << position << std::endl;
	std::cout << "bytesRead: " << bytesRead << std::endl;
	std::cout << "req: " << req.str() << std::endl;
	return 10;
}

//long	Request::readFromSocket(unsigned int socketFd, ServerConfig &config)
//{
//	std::ifstream client;
//	long bytesRead = handleFirstLineHeader(socketFd);
//	if (bytesRead <= 0)
//		return bytesRead;
//
//	std::ostringstream lineStream;
//
//	while (_state == HEADER)
//	{
//		long	lineRead = Utils::get_next_line(socketFd, lineStream);
//
//		
//		if (lineRead < 0 && lineStream.str().empty())
//			return lineRead;
//		std::string line = lineStream.str();
//		std::cout << "STATE3: " << _state << std::endl;
//		std::cout << "line: " << lineStream.str() << std::endl;
//
//		if (line.empty() ||
//				line.find_first_not_of(" \t\r\n") == std::string::npos) {
//			_state = BODY;
//			break;
//		}
//		bytesRead += lineRead;
//		saveHeaderLine(line);
//	}
//	std::cout << "STATE4: " << _state << std::endl;
//
//
//	if (!checkHeaderLocation(config)) {
//		Utils::get_next_line(-1, lineStream);
//		return bytesRead;
//	}
//	std::cout << "STATE5: " << _state << std::endl;
//	std::cout << "bytesRead: " << bytesRead << std::endl;
//
//	if (_contentLength == 0)
//		return bytesRead;
//	std::cout << "STATE6: " << _state << std::endl;
//	std::cout << "bytesRead: " << bytesRead << std::endl;
//	_body = std::vector<char>(_contentLength);
//	size_t totalRead = 0;
//
//	while (static_cast<long long>(totalRead) < _contentLength)
//	{
//		long lineRead = Utils::get_next_line(socketFd, lineStream);
//		if (lineRead < 0)
//			return lineRead;
//		bytesRead += lineRead;
//		std::string chunk = lineStream.str() + "\n";
//		if (chunk.size() == 0)
//			break;
//		std::copy(chunk.begin(), chunk.begin() + chunk.size(), _body.begin() + totalRead);
//		totalRead += chunk.size();
//	}
//	return bytesRead;
//}

void	Request::setMethod(std::string s)
{
	this->_method = s;
}

void	Request::setPath(std::string s)
{
	std::size_t found = s.find("?");

	if (found != std::string::npos)
	{
		this->_path = s.substr(0, found);
		this->_query = s.substr(found + 1);
	}
	else
		this->_path = s;
	setMimeType();
}


void	Request::setMimeType()
{
	if (_path.length() >= 5 && _path.substr(_path.length() - 5) == ".html") {
		_mimetype = "text/html";
	} else if (_path.length() >= 4 &&
			_path.substr(_path.length() - 4) == ".css") {
		_mimetype = "text/css";
	} else if (_path.length() >= 4 &&
			_path.substr(_path.length() - 4) == ".jpg") {
		_mimetype = "image/jpg";
	} else if (_path.length() >= 3 && _path.substr(_path.length() - 3) == ".py") {
		_mimetype = "application/python";
	} else if (_path.length() >= 3 && _path.substr(_path.length() - 3) == ".js") {
		_mimetype = "application/javascript";
	} else if (_path.length() >= 4 &&
			_path.substr(_path.length() - 4) == ".mp4") {
		_mimetype = "video/mp4";
	} else {
		_mimetype = "text/plain";
	}
}

bool	Request::isCGI() const
{
	return getMimeType() == "application/python" ? true : false;
}

/* Getters Methods */

std::string Request::getMethod() const { return this->_method; }

long long Request::getContentLength() const { return this->_contentLength; }

std::string Request::getPath() const { return this->_path; }

std::string Request::getMimeType() const { return this->_mimetype; }

std::string Request::getQuery() const { return this->_query; }

std::vector<char> Request::getBody() const { return this->_body; }

std::string	Request::getHeaderField(std::string field) const
{
	if (_headers.count(field))
	{
  		std::string target = _headers.at(field);
   		return target;
	}
	return "";
}
