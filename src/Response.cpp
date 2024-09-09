/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzioual <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 14:26:18 by yzioual           #+#    #+#             */
/*   Updated: 2024/09/02 14:26:20 by yzioual          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

Response	&Response::operator=(const Response &rhs)
{
	if (this != &rhs) {
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

	if (std::getline(iss, part, ' ')) {
		result.push_back(part);
		if (std::getline(iss, part))
			result.push_back(part);
	}
	return result;
}

void Response::updateResponse(unsigned short statusCode, std::string contentType, std::string buffer)
{
	setStatusCode(statusCode);
	_contentType = contentType;
	_buffer = buffer;
}

void Response::buildError()
{
	Request &request = _client->getRequest();

	if (_client->getConfig().error_page.empty())
		return;
	std::vector<std::string> error_page =
		splitString(_client->getConfig().error_page);
	if (error_page.size() != 2)
		return;
	std::string newPath = error_page[1];
	_client->getRequest().setPath(newPath);
	std::stringstream buffer;
	std::ifstream inFile(std::string("." + request.getPath()).c_str());
	buffer << inFile.rdbuf();
	inFile.close();
	updateResponse(atoi(error_page[0].c_str()), request.getMimeType(),
			buffer.str());
	return;
}

void	Response::buildPath(LocationConfig &target, short index_max)
{
	Request &request = _client->getRequest();
	std::string path = request.getPath();
	std::string end_s = path.substr(index_max);
	if (!target.root.empty() &&
			target.root.compare(1, target.root.size() - 1, end_s, 0,
				target.root.size() - 1) == 0)
		return;
	if (end_s.empty() && !target.index.empty())
		request.setPath(target.root + "/" + target.index);
	else
		request.setPath(target.root + "/" + end_s);
}

void Response::build(void)
{
	Request &request = _client->getRequest();
	ServerConfig config = _client->getConfig();
	LocationConfig target;
	std::string path = request.getPath();
	short index_max =
		config.getLocationByPathRequested(request.getPath(), target);

	if (index_max == -1) {
		buildError();
		return finalizeHTMLResponse();
	}

	std::vector<std::string> &allowed_methods = target.allowed_methods;
	if (allowed_methods.size() &&
			std::find(allowed_methods.begin(), allowed_methods.end(),
				request.getMethod()) == allowed_methods.end())
		updateResponse(405, "text/plain", "Method Not Allowed");
	else if (config.client_max_body_size < request.getContentLength())
		updateResponse(413, "text/plain", "Payload Too Large");
	else {
		buildPath(target, index_max);
		if (request.isCGI())
			handleCGI();
		else
			handleStaticFiles();
	}
	finalizeHTMLResponse();
}

void	Response::handleCGI(void)
{
	CGIResponse	cgi = CGIResponse(this->_client);
	std::string	resp = cgi.execute();

	if (!resp.empty())
	{
		std::string	headers;
		std::string	body;
		std::size_t	pos = resp.find("\r\n\r\n");

		if (pos != std::string::npos)
		{
			headers = resp.substr(0, pos);
			body = resp.substr(pos + 4);
		}
		else
			body = resp;
		std::istringstream 	headerStream(headers);
		std::string 		line;

		while (std::getline(headerStream, line))
			if (line.find("Set-Cookie:") == 0)
				_cookies.push_back(line);
		updateResponse(200, "text/html", resp);
	}
	else 
		buildError();
}

void	Response::handleStaticFiles(void)
{
	Request &request = _client->getRequest();
	std::string newPath = request.getPath();
	std::string mimetype = request.getMimeType();
	std::ifstream inFile(std::string("." + newPath).c_str());
	std::stringstream buffer;

	if (inFile.is_open())
	{
		buffer << inFile.rdbuf();
		inFile.close();
		updateResponse(200, request.getMimeType(), buffer.str());
	}
	else
		buildError();
}

void	Response::finalizeHTMLResponse(void) {
	if (_buffer.empty())
	{
		std::string errorDefault =
			std::string("<!DOCTYPE html>"
					"<html lang='en'>"
					"<head>"
					"<meta charset='UTF-8'>"
					"<title>404 - Page Not Found</title>"
					"<style>"
					"body {"
					"    font-family: Arial, sans-serif;"
					"    background-color: #f8f9fa;"
					"    color: #333;"
					"    text-align: center;"
					"    padding: 50px;"
					"}"
					"h1 {"
					"    font-size: 50px;"
					"}"
					"p {"
					"    font-size: 20px;"
					"}"
					"</style>"
					"</head>"
					"<body>"
					"<h1>404 - Page Not Found</h1>"
					"<p>Sorry, the page you are looking for does not exist.</p>"
					"</body>"
					"</html>");
		updateResponse(404, "text/html", errorDefault);
	}
	std::ostringstream ss;
	ss << "HTTP/1.1 " << this->getStatusToString()
		<< "\nContent-Type: " << _contentType
		<< "\nContent-Length: " << _buffer.size() << "\n\n";
	for (std::vector<std::string>::iterator it = _cookies.begin(); it != _cookies.end(); ++it)
		ss << *it;
	ss << "\r\n" << _buffer;
	this->_value = ss.str();
}

const std::map<unsigned short, std::string>& Response::getStatusCodes()
{
	static std::map<unsigned short, std::string> statusCodes;

	if (statusCodes.empty())
	{
		statusCodes[200] = "OK";
		statusCodes[201] = "Created";
		statusCodes[202] = "Accepted";
		statusCodes[204] = "No Content";
		statusCodes[301] = "Moved Permanently";
		statusCodes[302] = "Found";
		statusCodes[304] = "Not Modified";
		statusCodes[400] = "Bad Request";
		statusCodes[401] = "Unauthorized";
		statusCodes[403] = "Forbidden";
		statusCodes[404] = "Not Found";
		statusCodes[405] = "Method Not Allowed";
		statusCodes[413] = "Payload Too Large";
		statusCodes[500] = "Internal Server Error";
		statusCodes[501] = "Not Implemented";
		statusCodes[502] = "Bad Gateway";
		statusCodes[503] = "Service Unavailable";
		statusCodes[504] = "Gateway Timeout";
	}
	return statusCodes;
}

void	Response::setStatusCode(unsigned short code)
{
	const std::map<unsigned short, std::string>& statusCodes = getStatusCodes();
	std::map<unsigned short, std::string>::const_iterator it = statusCodes.find(code);
	if (it != statusCodes.end()) {
		_statusCode.code = code;
		_statusCode.status = it->second;
	} else {
		_statusCode.code = 500;
		_statusCode.status = "Internal Server Error";
	}
}

const std::string Response::getResponse() const
{
	return this->_value;
}

STATUS_CODE Response::getStatusCode() const 
{
	return this->_statusCode;
}

std::string Response::getStatusToString() const {
	std::stringstream ss;
	ss << _statusCode.code << " " << _statusCode.status;
	return ss.str();
}
