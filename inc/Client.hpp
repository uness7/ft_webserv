/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzioual <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 14:54:10 by yzioual           #+#    #+#             */
/*   Updated: 2024/09/02 14:54:11 by yzioual          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "Config.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include <sys/unistd.h>
# include <sys/socket.h>
# include <unistd.h>
# include <cstdlib>
# include <cstring>
# include <cerrno>
# include <cstring>

# define BUFFER_SIZE 4096

class Response;

class Client
{
	private:
		unsigned short 	_fd;
		long 		_dataSent;
		Request 	_request;
		Response *	_response;
		ServerConfig 	_config;

	public:
		Client(unsigned short, ServerConfig);
		Client(const Client &);
		Client &operator=(const Client &);
		~Client();

		unsigned short 		getFd() const;
		void 			setFd(unsigned short);
		long 			getDataSent() const;
		void			setDataSent(long);
		Request&		getRequest();
	    long 			readRequest();
		const std::string 	getResponseToString() const;
		const Response 		*getResponse() const;
		void			sendResponse();
		const ServerConfig 	getConfig() const;
		void              clear();
};
