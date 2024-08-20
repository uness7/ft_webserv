#include "../inc/Request.hpp"
#include <iterator>
#include <sstream>
#include <cctype>
#include <algorithm>

Request::Request(std::string entireRequest)
    : _method(""), _path(""), _query(""), _mimetype(""), _body(""), _headers(), _data(entireRequest) {
    parseData();
}

Request::~Request()
{
	//
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
	}
	return *this;
}

void	Request::parseData(void)
{
	std::stringstream	request(_data);
	std::string 		method;
	std::string 		path;
	std::string 		line;
	std::stringstream 	body;
	bool 			isBody = false;

	request >> method;
	request >> path;
	setPath(path);
	setMethod(method);
	std::getline(request, line);
	while (std::getline(request, line))
	{
		if (isBody)
			body << line;
		else
		{
			std::size_t	found = line.find(":");
			if (found == std::string::npos)
			{
				isBody = true;
				continue;
			}
			std::string	key = line.substr(0, found);
			std::string 	value = line.substr(found+1);
            		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
			_headers.insert(std::make_pair(key, value));
		} 
	}
	std::string b = body.str();
	if (!b.empty())
		_body = b;
}

void	Request::setMethod(std::string s)
{
	this->_method = s;
}

void	Request::setPath(std::string s)
{
	std::size_t	found = s.find("?");

	if (found != std::string::npos)
	{
		this->_path = s.substr(0, found);
		this->_query =  s.substr(found + 1);
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

std::string	Request::getMethod() const
{
	return this->_method;
}

std::string	Request::getPath() const
{
	return this->_path;
}

std::string	Request::getMimeType() const
{
	return this->_mimetype;
}

std::string	Request::getQuery() const
{
	return this->_query;
}

std::string	Request::getBody() const
{
	return this->_body;
}

std::string	Request::getHeaderField(std::string field) const
{
	if (_headers.count(field))
		return _headers.at(field);
	return "";
}

std::ostream	&operator<<(std::ostream &out, const Request &req)
{
	out << req._data << std::endl;
	return out;
}
