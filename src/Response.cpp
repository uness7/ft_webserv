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

Response::Response()
    : _value(""), _statusCode(), _contentType(""), _buffer(""), _client(NULL) {}

Response::Response(Client *client)
    : _value(""), _statusCode(), _contentType(""), _buffer(""),
      _client(client) {
  this->build();
}

Response::Response(const Response &cp) { *this = cp; }

Response &Response::operator=(const Response &rhs) {
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

std::vector<std::string> splitString(const std::string &str) {
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

void Response::updateResponse(unsigned short statusCode,
                              std::string contentType, std::string buffer) {
  setStatusCode(statusCode);
  _contentType = contentType;
  _buffer = buffer;
}

void Response::buildError() {
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

void Response::buildPath(LocationConfig &target, short index_max) {
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

void Response::build(void) {
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

void Response::handleCGI(void) {
  CGIResponse cgi = CGIResponse(this->_client);
  std::string resp = cgi.execute();

  if (!resp.empty())
    updateResponse(200, "text/html", resp);
  else {
    buildError();
  }
}

void Response::handleStaticFiles(void) {
  Request &request = _client->getRequest();
  std::string newPath = request.getPath();
  std::string mimetype = request.getMimeType();
  std::ifstream inFile(std::string("." + newPath).c_str());
  std::stringstream buffer;

  if (inFile.is_open()) {
    buffer << inFile.rdbuf();
    inFile.close();
    updateResponse(200, request.getMimeType(), buffer.str());
  } else {
    buildError();
  }
}

void Response::finalizeHTMLResponse(void) {
  if (_buffer.empty()) {
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
     << "\nContent-Length: " << _buffer.size() << "\n\n"
     << _buffer;
  this->_value = ss.str();
}

void Response::setStatusCode(unsigned short code) {
  _statusCode.code = code;
  switch (code) {
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
  case 413:
    _statusCode.status = "Payload Too Large";
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

const std::string Response::getResponse() const { return this->_value; }
void Response::setStatusCode(STATUS_CODE statusCode) {
  this->_statusCode.code = statusCode.code;
  this->_statusCode.status = statusCode.status;
}

STATUS_CODE Response::getStatusCode() const { return this->_statusCode; }

std::string Response::getStatusToString() const {
  std::stringstream ss;
  ss << _statusCode.code << " " << _statusCode.status;
  return ss.str();
}
