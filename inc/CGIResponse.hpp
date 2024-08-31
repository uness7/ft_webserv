#ifndef CGIRESPONSE_HPP
#define CGIRESPONSE_HPP

# include <string>
# include <map>
# include <Client.hpp>
# include <unistd.h>
# include <sys/wait.h>
# include <iostream>
# include <sstream>
# include <cstring>
# include <cstdlib>
# include <cstdio>
# include <unistd.h>
# include "Utils.hpp"

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
