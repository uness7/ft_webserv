/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzioual <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 14:54:15 by yzioual           #+#    #+#             */
/*   Updated: 2024/09/02 14:54:17 by yzioual          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

struct LocationConfig {
	std::string root;
	std::string index;
	bool autoindex;
	std::vector<std::string> limit_except;
	std::string upload_store;
	std::vector<std::string> allowed_methods;
	std::string cgi_script;
	std::string redirect;
	long long client_max_body_size;
	std::map<std::string, std::string> content;
};

struct ServerConfig {
	std::map<std::string, std::string> fields;
	std::string listen;
	unsigned int port;
	std::string server_name;
	std::string error_page;
	long long client_max_body_size;
	std::map<std::string, LocationConfig> locations;
	short getLocationByPathRequested(std::string path,
					 LocationConfig &target);
};

class Config {
       public:
	Config(const std::string &fileName);
	~Config();

	std::vector<ServerConfig> getServerConfigs() const;
	static void printConfigs(
	    const std::vector<ServerConfig> &serverConfigs);

       private:
	std::vector<ServerConfig> _serverConfigs;

	void parseConfigFile(const std::string &fileName);
	void parseServer(std::ifstream &configFile, ServerConfig &serverConfig);
	void parseServerLine(std::ifstream &configFile, const std::string &line,
			     ServerConfig &serverConfig);
	std::string extractValue(const std::string &line,
				 const std::string &key);
	std::map<std::string, std::string> extractUnknownValue(
	    const std::string &line);
	std::string extractLocationPath(const std::string &line);
	void parseListen(const std::string &line, ServerConfig &serverConfig);
	void parseLocation(std::ifstream &configFile,
			   LocationConfig &locationConfig);
	void parseLocationLine(const std::string &line,
			       LocationConfig &locationConfig);
	static void printServerConfig(const ServerConfig &serverConfig);
	static void printLocationConfig(const std::string &locationPath,
					const LocationConfig &locationConfig);

	std::string vectorToString(const std::vector<std::string> &vec);
	void printLocationConfigMap(
	    const std::map<std::string, LocationConfig> &myMap);
};
