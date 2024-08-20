#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <map>
# include "Config.hpp"

class Utils
{
	public:
		static std::string	vectorToString(const std::vector<std::string>& vec);
		static void 		printLocationConfigMap(const std::map<std::string, LocationConfig>& myMap);
		static std::string     	getCgiScriptForKey(const ServerConfig& serverConfig, const std::string& key);


};

#endif 

