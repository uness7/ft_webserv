#include "Utils.hpp"
#include <iostream>

std::string     Utils::vectorToString(const std::vector<std::string>& vec)
{
        std::string     result = "[";

        for (size_t i = 0; i < vec.size(); ++i) {
                result += vec[i];
                if (i != vec.size() - 1) {
                        result += ", ";
                }
        }
        result += "]";
        return result;
}

void    Utils::printLocationConfigMap(const std::map<std::string, LocationConfig>& myMap)
{
        std::map<std::string, LocationConfig>::const_iterator it;

	std::cout << "--------------------------------------------------------------------------------" << std::endl;
        for (it = myMap.begin(); it != myMap.end(); ++it) {
                std::cout << "Key: " << it->first << std::endl;
                std::cout << "  Root: " << it->second.root << std::endl;
                std::cout << "  Index: " << it->second.index << std::endl;
                std::cout << "  Autoindex: " << (it->second.autoindex ? "true" : "false") << std::endl;
                std::cout << "  Limit Except: " << vectorToString(it->second.limit_except) << std::endl;
                std::cout << "  Upload Store: " << it->second.upload_store << std::endl;
                std::cout << "  Allowed Methods: " << vectorToString(it->second.allowed_methods) << std::endl;
                std::cout << "  CGI Script: " << it->second.cgi_script << std::endl;
                std::cout << "-----------------------------------" << std::endl;
        }
}
