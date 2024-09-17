/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzioual <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 14:52:43 by yzioual           #+#    #+#             */
/*   Updated: 2024/09/17 11:57:24 by otourabi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/Utils.hpp"
# define BUFFER_SIZE 2048

std::string     Utils::getCgiScriptForKey(const ServerConfig& serverConfig, const std::string& key)
{
        std::map<std::string, LocationConfig>::const_iterator it = serverConfig.locations.find(key);
        if (it != serverConfig.locations.end())
                return it->second.cgi_script;
        return "";
}

long	Utils::get_next_line(int fd, std::ostringstream &oss)
{
	static char buffer[BUFFER_SIZE];
	static ssize_t bytesRead = 0;
	static size_t currentPos = 0;
	oss.str("");
	if (fd < 0)
	{
		memset(&buffer, 0, BUFFER_SIZE);
		bytesRead = 0;
		currentPos = 0;
		return 0;
	}

	while (true) {
		if (static_cast<ssize_t>(currentPos) >= bytesRead) {
			bytesRead = recv(fd, buffer, BUFFER_SIZE, 0);
			currentPos = 0;
			if (bytesRead == -1) {
				return -1;
			}
			if (bytesRead == 0) {
				break;
			}
		}

		while (static_cast<ssize_t>(currentPos) < bytesRead) {
			if (buffer[currentPos] == '\n' && buffer[currentPos-1] == '\r') {
				currentPos++;
				return oss.str().size();
			}
			oss << buffer[currentPos++];
		}
	}
	return oss.str().size();
}

std::string	Utils::vectorToString(const std::vector<std::string> &vec)
{
	std::string result = "[";

	for (size_t i = 0; i < vec.size(); ++i) {
		result += vec[i];
		if (i != vec.size() - 1) {
			result += ", ";
		}
	}
	result += "]";
	return result;
}

void	Utils::printLocationConfigMap(const std::map<std::string, LocationConfig> &myMap)
{
	std::map<std::string, LocationConfig>::const_iterator it;

	std::cout << "---------------------------------------------------------------"
		"-----------------"
		<< std::endl;
	for (it = myMap.begin(); it != myMap.end(); ++it) {
		std::cout << "Key: " << it->first << std::endl;
		std::cout << "  Root: " << it->second.root << std::endl;
		std::cout << "  Index: " << it->second.index << std::endl;
		std::cout << "  Autoindex: " << (it->second.autoindex ? "true" : "false")
			<< std::endl;
		std::cout << "  Limit Except: " << vectorToString(it->second.limit_except)
			<< std::endl;
		std::cout << "  Upload Store: " << it->second.upload_store << std::endl;
		std::cout << "  Allowed Methods: "
			<< vectorToString(it->second.allowed_methods) << std::endl;
		std::cout << "  CGI Script: " << it->second.cgi_script << std::endl;
		std::cout << "-----------------------------------" << std::endl;
	}
}
