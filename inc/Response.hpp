/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzioual <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 14:54:21 by yzioual           #+#    #+#             */
/*   Updated: 2024/09/02 14:54:23 by yzioual          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "Request.hpp"
# include "Client.hpp"
# include <iostream>
# include <cstring>
# include <string>
# include <cstdlib>
# include <algorithm>
# include <map>


struct STATUS_CODE {
	unsigned short code;
	std::string status;
};

class Client;

class Response
{
	private:
		std::string 				_value;
		STATUS_CODE 				_statusCode;
		std::string 				_contentType;
		std::string 				_buffer;
		Client 					    *_client;
		std::map<std::string, std::string>	envMap;

		void	build();
		void	buildError();
		bool	buildPath();
        	void    updateResponse(unsigned short statusCode, std::string contentType, std::string buffer);

	public:
		Response();
		Response(Client *);
		Response(const Response &);
		Response &operator=(const Response &);
		~Response();

		const std::string	getResponse() const;
		void 		setStatusCode(STATUS_CODE);
		void		setStatusCode(unsigned short code);
		void		handleCGI(void);
		void		handleStaticFiles(void);
		void		finalizeHTMLResponse(void) ;
		STATUS_CODE 	getStatusCode() const;
		std::string 	getStatusToString() const;
};
