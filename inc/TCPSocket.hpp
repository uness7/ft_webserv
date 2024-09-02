/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TCPSocket.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzioual <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 14:54:34 by yzioual           #+#    #+#             */
/*   Updated: 2024/09/02 14:55:16 by yzioual          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../inc/Config.hpp"

#include <algorithm>
#include <arpa/inet.h>
#include <cstdlib>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class TCPSocket
{
	private:
		ServerConfig _serverConfig;
		int _socketFD;
		std::string _ipAddress; 
		int _port;
		struct sockaddr_in _socketAddress; 
		unsigned int _socketAddressLength;
		TCPSocket(); 

	public:
		TCPSocket(const ServerConfig&);
		TCPSocket(const TCPSocket &);
		TCPSocket &operator=(const TCPSocket &);
		~TCPSocket();

		void initSocket(); 
		void closeServer() const; 
		int getSocketFD() const;
		int getPort() const;
		std::string getIpAddress() const;
		struct sockaddr_in &getSocketAdress();
		unsigned int &getSocketAddressLength();
		ServerConfig &getServerConfig();
		class CreateSocketException : public std::exception {
			public:
				const char * what() const throw() {
					return "Cannot create socket";
				}
		};
		class InitSocketException : public std::exception {
			public:
				const char * what() const throw() {
					return "Cannot init socket";
				}
		};
};

std::vector<TCPSocket*> createSockets(const std::vector<ServerConfig>& serverConfigs);
