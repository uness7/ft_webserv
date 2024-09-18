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
	std::ostringstream 	lineStream;
	long	bytesRead = get_next_line(socketFd, lineStream);
	if (lineStream.str().size() == 1 && lineStream.str()[0] == '\n')
		return 0;
	if (bytesRead < 0 && lineStream.str().empty())
		return bytesRead;
	std::istringstream iss(lineStream.str());
	std::string method;
	std::string path;
	std::string httpv;
	iss >> method >> path >> httpv;
	setMethod(method);
	setPath(path);
    _httpv = httpv;
	_state = HEADER;
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

long	Request::readFromSocket(unsigned int socketFd, ServerConfig &config)
{
    std::ifstream client;
    long bytesRead = handleFirstLineHeader(socketFd);
    if (bytesRead <= 0)
        return bytesRead;

    std::ostringstream lineStream;

    while (_state == HEADER)
    {
        long	lineRead = get_next_line(socketFd, lineStream);
        if (lineRead < 0 && lineStream.str().empty())
            return lineRead;
        std::string line = lineStream.str();

        if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos) {
            _state = BODY;
            break;
        }
        bytesRead += lineRead;
        saveHeaderLine(line);
    }

    if (!checkHeaderLocation(config)) {
        get_next_line(-1, lineStream);
        return bytesRead;
    }

    if (_contentLength == 0)
        return bytesRead;
    _body = std::vector<char>(_contentLength);
    size_t totalRead = 0;

    while (static_cast<long long>(totalRead) < _contentLength)
    {
        long lineRead = get_next_line(socketFd, lineStream);
        if (lineRead < 0)
            return lineRead;
        bytesRead += lineRead;
        std::string chunk = lineStream.str() + "\n";
        if (chunk.size() == 0)
            break;
        std::copy(chunk.begin(), chunk.begin() + chunk.size(), _body.begin() + totalRead);
        totalRead += chunk.size();
    }
    return bytesRead;
}

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

long	Request::get_next_line(int fd, std::ostringstream &oss)
{
	static char buffer[BUFFER_SIZE];
	static ssize_t bytesRead = 0;
	static size_t currentPos = 0;
	oss.str("");
	if (fd < 0)
	{
		memset(&buffer, 0, BUFFER_SIZE);
		bytesRead = 0;
		currentPos = 0;
		return 0;
	}

	while (true) {
		if (static_cast<ssize_t>(currentPos) >= bytesRead) {
			bytesRead = recv(fd, buffer, BUFFER_SIZE, 0);
			currentPos = 0;
			if (bytesRead == -1) {
				if (!oss.str().empty())
					return oss.str().size();
				return -1;
			}
			if (bytesRead == 0) {
				break;
			}
		}

		while (static_cast<ssize_t>(currentPos) < bytesRead) {
			if (buffer[currentPos] == '\n' && buffer[currentPos-1] == '\r') {
				currentPos++;
				return oss.str().size();
			}
			oss << buffer[currentPos++];
		}
	}
	return oss.str().size();
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
