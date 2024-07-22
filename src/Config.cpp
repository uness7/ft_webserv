#include "../inc/Config.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

// Constructor that parses the configuration file
Config::Config(const std::string &fileName) { parseConfigFile(fileName); }

Config::~Config() {}

// Function to parse the configuration file
void Config::parseConfigFile(const std::string &fileName)
{
    std::ifstream configFile(fileName.c_str());
    std::string line;

    if (!configFile.is_open())
        throw std::runtime_error("Unable to open configuration file: " + fileName);

    while (std::getline(configFile, line))
    {
        if (line.find("server {") != std::string::npos)
        {
            ServerConfig serverConfig;
            parseServer(configFile, serverConfig);
            serverConfigs.push_back(serverConfig);
        }
    }
}

// Function to parse a server block
void Config::parseServer(std::ifstream &configFile, ServerConfig &serverConfig)
{
    std::string line;
    while (std::getline(configFile, line) && line.find("}") == std::string::npos)
        parseServerLine(configFile, line, serverConfig);
}

// Function to parse individual lines within a server block
void Config::parseServerLine(std::ifstream &configFile, const std::string &line, ServerConfig &serverConfig)
{
    if (line.find("listen") != std::string::npos)
        parseListen(line, serverConfig);
    else if (line.find("server_name") != std::string::npos)
        serverConfig.server_name = extractValue(line, "server_name");
    else if (line.find("error_page") != std::string::npos)
        serverConfig.error_page = extractValue(line, "error_page");
    else if (line.find("client_max_body_size") != std::string::npos)
        serverConfig.client_max_body_size = extractValue(line, "client_max_body_size");
    else if (line.find("location") != std::string::npos)
    {
        std::string locationPath = extractLocationPath(line);
        LocationConfig locationConfig;
        parseLocation(configFile, locationConfig);
        serverConfig.locations[locationPath] = locationConfig;
    }
}

// Function to extract the value after a keyword in a line
std::string Config::extractValue(const std::string &line, const std::string &key)
{
    size_t pos = line.find(key) + key.length();
    std::string value = line.substr(pos);
    value.erase(0, value.find_first_not_of(" \t\n\r\f\v"));
    value.erase(value.find_last_not_of(" \t\n\r\f\v") + 1);
    return value;
}

// Function to extract the path from a location directive
std::string Config::extractLocationPath(const std::string &line)
{
    size_t pos = line.find("location") + 8;
    size_t openBracePos = line.find("{");
    std::string locationPath = line.substr(pos, openBracePos - pos);
    locationPath.erase(0, locationPath.find_first_not_of(" \t\n\r\f\v"));
    locationPath.erase(locationPath.find_last_not_of(" \t\n\r\f\v") + 1);
    return locationPath;
}

// Function to parse the listen directive
void Config::parseListen(const std::string &line, ServerConfig &serverConfig)
{
    size_t pos = line.find("listen") + 6;
    size_t colonPos = line.find(":");
    size_t semicolonPos = line.find(";");
    serverConfig.listen = line.substr(pos + 1, colonPos - pos - 1);
    std::string portStr = line.substr(colonPos + 1, semicolonPos - colonPos - 1);
    serverConfig.listen.erase(0, serverConfig.listen.find_first_not_of(" \t\n\r\f\v"));
    serverConfig.listen.erase(serverConfig.listen.find_last_not_of(" \t\n\r\f\v") + 1);
    portStr.erase(0, portStr.find_first_not_of(" \t\n\r\f\v"));
    portStr.erase(portStr.find_last_not_of(" \t\n\r\f\v") + 1);
    std::istringstream(portStr) >> serverConfig.port;
    std::cout << "Parsed IP: " << serverConfig.listen << " Port: " << serverConfig.port << std::endl;
}

// Function to parse a location block
void Config::parseLocation(std::ifstream &configFile, LocationConfig &locationConfig){
    std::string line;
    while (std::getline(configFile, line) && line.find("}") == std::string::npos)
        parseLocationLine(line, locationConfig);
}

// Function to parse individual lines within a location block
void Config::parseLocationLine(const std::string &line, LocationConfig &locationConfig)
{
    if (line.find("root") != std::string::npos)
        locationConfig.root = extractValue(line, "root");
    else if (line.find("index") != std::string::npos)
        locationConfig.index = extractValue(line, "index");
    else if (line.find("autoindex") != std::string::npos)
    {
        std::string value = extractValue(line, "autoindex");
        locationConfig.autoindex = (value == "on");
    }
    else if (line.find("limit_except") != std::string::npos)
    {
        std::string methods = extractValue(line, "limit_except");
        std::istringstream iss(methods);
        std::string method;
        while (iss >> method)
            locationConfig.limit_except.push_back(method);
    }
    else if (line.find("upload_store") != std::string::npos)
        locationConfig.upload_store = extractValue(line, "upload_store");
}

// Function to return the server configurations
std::vector<ServerConfig> Config::getServerConfigs() const { return serverConfigs; }

// Function to print the configuration details of each server
void Config::printConfigs(const std::vector<ServerConfig>& serverConfigs)
{
    for (std::vector<ServerConfig>::const_iterator it = serverConfigs.begin(); it != serverConfigs.end(); ++it)
        printServerConfig(*it);
}

// Helper function to print individual server configurations
void Config::printServerConfig(const ServerConfig& serverConfig)
{
    std::cout << "Server listening on " << serverConfig.listen << " Port: " << serverConfig.port << std::endl;
    if (!serverConfig.server_name.empty())
        std::cout << "Server name: " << serverConfig.server_name << std::endl;
    if (!serverConfig.error_page.empty())
        std::cout << "Error page: " << serverConfig.error_page << std::endl;
    if (!serverConfig.client_max_body_size.empty())
        std::cout << "Client max body size: " << serverConfig.client_max_body_size << std::endl;
    for (std::map<std::string, LocationConfig>::const_iterator loc_it = serverConfig.locations.begin(); loc_it != serverConfig.locations.end(); ++loc_it)
        printLocationConfig(loc_it->first, loc_it->second);
}

// Helper function to print individual location configurations
void Config::printLocationConfig(const std::string& locationPath, const LocationConfig& locationConfig)
{
    std::cout << "Location: " << locationPath << std::endl;
    std::cout << "  Root: " << locationConfig.root << std::endl;
    if (!locationConfig.index.empty())
        std::cout << "  Index: " << locationConfig.index << std::endl;
    std::cout << "  Autoindex: " << (locationConfig.autoindex ? "on" : "off") << std::endl;
    if (!locationConfig.limit_except.empty())
    {
        std::cout << "  Limit except: ";
        for (std::vector<std::string>::const_iterator method_it = locationConfig.limit_except.begin(); method_it != locationConfig.limit_except.end(); ++method_it)
            std::cout << *method_it << " ";
        std::cout << std::endl;
    }
    if (!locationConfig.upload_store.empty())
        std::cout << "  Upload store: " << locationConfig.upload_store << std::endl;
}
