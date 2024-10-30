#include "Client.hpp"

Client::Client(unsigned short fd, ServerConfig config)
    : _fd(fd), _dataSent(0), _request(config), _config(config) {
	this->_response = NULL;
}

Client::Client(const Client &cp) : _fd(0), _dataSent(0), _request(cp._config) {
	*this = cp;
}

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

Client::~Client() {
    if (_response != NULL) {
        delete _response;
        _response = NULL;
    }
}

void Client::clear() {
	_request = Request(_config);
	_dataSent = 0;
	if (_response != NULL) {
		delete _response;
		_response = NULL;
	}
}

unsigned short Client::getFd() const { return this->_fd; }

void Client::setFd(unsigned short fd) { this->_fd = fd; }

Request &Client::getRequest() { return this->_request; }

long Client::readRequest() { return _request.read(getFd()); }

const std::string Client::getResponseToString() const {
	return _response->getResponse();
}

const Response *Client::getResponse() const { return _response; }

void Client::sendResponse() {
	if (_response != NULL) {
        delete _response;
		_response = NULL;
	}
    if (getDataSent() == 0) {
	    _response = new Response(this);
    } else {
        std::cout << "Datasent: " << getDataSent() << " all -> " << getResponseToString().size() << std::endl;
    }

	std::string response = getResponseToString();

	long long bytesSent = send(getFd(), response.c_str() + getDataSent(),
			 response.size() - getDataSent(), 0);

	if (bytesSent <= 0)
		setDataSent(-1);
	else if (bytesSent + getDataSent() ==
		 static_cast<long>(response.size()))
		setDataSent(0);
	else
		setDataSent(getDataSent() + bytesSent);
}

long Client::getDataSent() const { return _dataSent; }

void Client::setDataSent(long dataSent) { this->_dataSent = dataSent; }

const ServerConfig Client::getConfig() const { return this->_config; }
