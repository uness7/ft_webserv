/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzioual <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 14:23:14 by yzioual           #+#    #+#             */
/*   Updated: 2024/09/02 14:23:17 by yzioual          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Client.hpp"

Client::Client(unsigned short fd, ServerConfig config)
    : _fd(fd), _dataSent(0), _request(), _config(config) {
  this->_response = new Response();
}

Client::Client(const Client &cp) : _fd(0), _request() { *this = cp; }

Client &Client::operator=(const Client &rhs) {
  if (this != &rhs) {
    this->setFd(rhs._fd);
    this->_request = rhs._request;
    this->_dataSent = rhs._dataSent;
    this->_response = rhs._response;
    this->_config = rhs._config;
  }
  return *this;
}

Client::~Client() {}

unsigned short Client::getFd() const { return this->_fd; }

void Client::setFd(unsigned short fd) { this->_fd = fd; }

Request &Client::getRequest() { return this->_request; }

long Client::readRequest() {
  _request = Request();
  return _request.readFromSocket(getFd());
}

const std::string Client::getResponseToString() const {
  return _response->getResponse();
}

const Response *Client::getResponse() const { return _response; }

void Client::sendResponse() {
  long bytesSent;
  _response = new Response(this);
  std::string response = getResponseToString();

  bytesSent = send(getFd(), response.c_str() + getDataSent(),
                    response.size() - getDataSent(), 0);
  if(bytesSent <= 0)
    setDataSent(-1);
  else if (bytesSent + getDataSent() == static_cast<long>(response.size()))
    setDataSent(0);
  else
    setDataSent(getDataSent() + bytesSent);
}

long Client::getDataSent() const { return _dataSent; }

void Client::setDataSent(long dataSent) { this->_dataSent = dataSent; }

const ServerConfig Client::getConfig() const { return this->_config; }
