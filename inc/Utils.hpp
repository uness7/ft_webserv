/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzioual <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 14:54:27 by yzioual           #+#    #+#             */
/*   Updated: 2024/09/02 14:54:28 by yzioual          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
# include "Config.hpp"

class Utils
{
	public:
		static std::string	vectorToString(const std::vector<std::string>& vec);
		static void 		printLocationConfigMap(const std::map<std::string, LocationConfig>& myMap);
		static std::string     	getCgiScriptForKey(const ServerConfig& serverConfig, const std::string& key);
		static long get_next_line(int fd, std::ostringstream &oss);
};

#endif
