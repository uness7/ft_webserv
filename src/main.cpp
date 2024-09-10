/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzioual <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 14:24:45 by yzioual           #+#    #+#             */
/*   Updated: 2024/09/02 14:25:08 by yzioual          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"
#include "../inc/TCPSocket.hpp"
#include "../inc/Config.hpp"

void	createHTMLFile(std::vector<TCPSocket *> &vec, std::string &fileName)
{
	std::ofstream htmlFile(fileName.c_str());
	if (!htmlFile.is_open())
	{
		std::cerr << "Error: Could not open file " << fileName << " for writing." << std::endl;
		return;
	}

	htmlFile << "<html>\n";
	htmlFile << "<head>\n";
	htmlFile << "<title>Available Servers</title>\n";
	htmlFile << "</head>\n";
	htmlFile << "<body>\n";
	htmlFile << "<h1>Available Servers</h1>\n";
	htmlFile << "<ul>\n";

	/*  struct ServerConfig
  4 {
  3         std::string                             listen;
  2         unsigned int                            port;
  1         std::string                             server_name;
41          std::string                             error_page;                                                                                   
  1         long long                               client_max_body_size;
  2         std::map<std::string, LocationConfig>   locations;
  3         short getLocationByPathRequested(std::string path, LocationConfig &target);
  4 }; */

	for (size_t i = 1; i < vec.size(); ++i)
	{
		htmlFile << "<li><a href=\"http://" 
			<< "localhost"
			<< ":" 
			<< vec[i]->getServerConfig().port
			<< "\">"
			<< vec[i]->getServerConfig().server_name
			<< "</a></li>\n";
	}

	htmlFile << "</ul>\n";
	htmlFile << "</body>\n";
	htmlFile << "</html>\n";
	htmlFile.close();
}

int	main(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <configuration file>" << std::endl;
		return 1;
	}
	std::vector<ServerConfig> serverConfigs;
	try
	{
		Config config(argv[1]);
		serverConfigs = config.getServerConfigs();
		Config::printConfigs(serverConfigs);
		std::vector<TCPSocket*> sockets = createSockets(serverConfigs);

		//
		std::string	name = "home.html";
		createHTMLFile(sockets, name);

		Server server(sockets);
		server.runServers();
	}
	catch (const std::exception &e)
	{
		std::cerr << "error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
