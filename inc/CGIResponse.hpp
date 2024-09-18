/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzioual <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 14:54:01 by yzioual           #+#    #+#             */
/*   Updated: 2024/09/10 15:04:13 by yzioual          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIRESPONSE_HPP
#define CGIRESPONSE_HPP

# include <string>
# include <map>
# include <../inc/Client.hpp>
# include <unistd.h>
# include <sys/wait.h>
# include <iostream>
# include <sstream>
# include <cstring>
# include <cstdlib>
# include <cstdio>
# include <unistd.h>
# include <iomanip>
# include <vector>

class Client;

class CGIResponse
{
	public:
	        CGIResponse(Client *client);

		void		setCgiEnv();
		std::string	execute();

	private:
        	Client 					*_client;
		std::string 				_cgiPath;
		std::string 				_scriptPath;
		std::map<std::string, std::string>	_envMap;
};

#endif
