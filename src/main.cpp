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
		Server server(sockets);
		server.runServers();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
