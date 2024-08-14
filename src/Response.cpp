#include "../inc/Response.hpp"
#include "CGIResponse.hpp"

Response::Response() : _value(""), _statusCode(), _contentType(""), _buffer(""), _client(NULL) {}

Response::Response(Client *client) : _value(""), _statusCode(), _contentType(""), _buffer(""), _client(client)
{
	this->build();
}

Response::Response(const Response &cp)
{
	*this = cp;
}

Response &Response::operator=(const Response &rhs)
{
	if (this != &rhs)
	{
		this->_value = rhs._value;
		this->_statusCode = rhs.getStatusCode();
		this->_contentType = rhs._contentType;
		this->_buffer = rhs._buffer;
		this->_client = rhs._client;
	}
	return *this;
}

Response::~Response() {}

std::vector<std::string> splitString(const std::string &str)
{
	std::vector<std::string> result;
	std::istringstream iss(str);
	std::string part;

	if (std::getline(iss, part, ' '))
	{
		result.push_back(part);
		if (std::getline(iss, part))
		{
			result.push_back(part);
		}
	}
	return result;
}

void Response::buildError()
{
	Request &request = _client->getRequest();
	std::vector<std::string> error_page = splitString(_client->getConfig().error_page);
	if (error_page.size() != 2)
		return;
	std::string newPath = _client->getConfig().locations.begin()->second.root + error_page[1];
	_client->getRequest().setPath(newPath);
	std::stringstream buffer;
	std::ifstream inFile(std::string("." + request.getPath()).c_str());
	buffer << inFile.rdbuf();
	inFile.close();
	setStatusCode(atoi(error_page[0].c_str()));
	_buffer = buffer.str();
	_contentType = request.getMimeType();
}

void	Response::buildPath() {
	Request &request = _client->getRequest();
	std::map<std::string, LocationConfig> lc = _client->getConfig().locations;
	std::map<std::string, LocationConfig>::const_iterator it;
	std::string path = request.getPath();
	int path_max = -1;
	LocationConfig target;
	std::cout << path << std::endl;
	for (it = lc.begin(); it != lc.end(); it++)
	{

		int key_size = it->first.size();
		int found = it->first.compare(0, key_size, path, 0, key_size);
		if (found == 0 && path_max < key_size) {
			target = it->second;
			path_max = key_size;
		}
	}

	std::string end_s = path.substr(path_max);
	if (end_s.size() == 0)
		request.setPath(target.root + "/" + target.index);
	else	
		request.setPath(target.root + "/" + end_s);
	std::cout << request.getPath() << std::endl;

}

void Response::build()
{
	Request &request = _client->getRequest();
	std::string method = request.getMethod();
	std::vector<std::string> allowed_methods = _client->getConfig().locations.begin()->second.allowed_methods;

	if (std::find(allowed_methods.begin(), allowed_methods.end(), method) == allowed_methods.end())
	{
		setStatusCode(405);
		_contentType = "text/plain";
		_buffer = "Method Not Allowed";
	}
	else
	{
		std::string path = request.getPath();
		std::string server_root = _client->getConfig().locations.begin()->second.root;

		if (path == "/api/data")
		{
			_buffer = "{\"message\": \"This is dynamic data!\"}";
			setStatusCode(200);
			_contentType = "application/json";
		}
		else if (path == "/api/info")
		{
			_buffer = "{\"info\": \"This is some info!\"}";
			setStatusCode(200);
			_contentType = "application/json";
		}
		else
		{
			buildPath();

			std::string newPath = request.getPath();
			std::string mimetype = request.getMimeType();

			/*
			if (newPath == "/cgi-bin/form/index.html")
			{
				std::string	scriptPath;
				scriptPath = "/home/yzioual/Desktop/ft_webserv/cgi-bin/form/script.py";
				CGIResponse	cgiResponse(scriptPath);
				std::string	postData = request.getPostData();

				cgiResponse.setCgiEnv(envMap);
				const std::string	response = cgiResponse.execute(this->envMap, "/usr/bin/python3", scriptPath, "POST", postData);

				std::ostringstream httpResponse;
				httpResponse << "HTTP/1.1 200 OK\r\n";
				httpResponse << "Content-Type: text/html\r\n";
				httpResponse << "Content-Length: " << response.size() << "\r\n";
				httpResponse << "\r\n";
				httpResponse << response;
				this->_value = httpResponse.str();
				return;
			}
			*/

			std::ifstream inFile(std::string("." + newPath).c_str());
			std::stringstream buffer;
			if (inFile.is_open())
			{
				buffer << inFile.rdbuf();
				inFile.close();
				setStatusCode(200);
				_buffer = buffer.str();
				_contentType = request.getMimeType();
			}
			else
				buildError();
		}
	}

	std::ostringstream ss;
	ss << "HTTP/1.1 " << this->getStatusToString() << "\nContent-Type: " << _contentType
	   << "\nContent-Length: " << _buffer.size() << "\n\n"
	   << _buffer;
	this->_value = ss.str();
}

const std::string Response::getResponse() const
{
	return this->_value;
}

void Response::setStatusCode(unsigned short code)
{
	_statusCode.code = code;
	switch (code)
	{
	case 200:
		_statusCode.status = "OK";
		break;
	case 201:
		_statusCode.status = "Created";
		break;
	case 202:
		_statusCode.status = "Accepted";
		break;
	case 204:
		_statusCode.status = "No Content";
		break;
	case 301:
		_statusCode.status = "Moved Permanently";
		break;
	case 302:
		_statusCode.status = "Found";
		break;
	case 304:
		_statusCode.status = "Not Modified";
		break;
	case 400:
		_statusCode.status = "Bad Request";
		break;
	case 401:
		_statusCode.status = "Unauthorized";
		break;
	case 403:
		_statusCode.status = "Forbidden";
		break;
	case 404:
		_statusCode.status = "Not Found";
		break;
	case 405:
		_statusCode.status = "Method Not Allowed";
		break;
	case 500:
		_statusCode.status = "Internal Server Error";
		break;
	case 501:
		_statusCode.status = "Not Implemented";
		break;
	case 502:
		_statusCode.status = "Bad Gateway";
		break;
	case 503:
		_statusCode.status = "Service Unavailable";
		break;
	case 504:
		_statusCode.status = "Gateway Timeout";
		break;
	default:
		_statusCode.code = 500;
		_statusCode.status = "Internal Server Error";
		break;
	}
}

void Response::setStatusCode(STATUS_CODE statusCode)
{
	this->_statusCode.code = statusCode.code;
	this->_statusCode.status = statusCode.status;
}

STATUS_CODE Response::getStatusCode() const
{
	return this->_statusCode;
}

std::string Response::getStatusToString() const
{
	std::stringstream ss;
	ss << _statusCode.code << " " << _statusCode.status;
	return ss.str();
}
