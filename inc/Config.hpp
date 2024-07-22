#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <map>

struct LocationConfig
{
    std::string root;
    std::string index;
    bool autoindex;
    std::vector<std::string> limit_except;
    std::string upload_store;
};

struct ServerConfig
{
    std::string listen;
    unsigned int port;
    std::string server_name;
    std::string error_page;
    std::string client_max_body_size;
    std::map<std::string, LocationConfig> locations;
};

class Config
{
public:
    Config(const std::string &fileName);
    ~Config();

    std::vector<ServerConfig> getServerConfigs() const;
    static void printConfigs(const std::vector<ServerConfig>& serverConfigs);
private:
    std::vector<ServerConfig> serverConfigs;

    void parseConfigFile(const std::string &fileName);
    void parseServer(std::ifstream &configFile, ServerConfig &serverConfig);
    void parseServerLine(std::ifstream &configFile, const std::string &line, ServerConfig &serverConfig);
    std::string extractValue(const std::string &line, const std::string &key);
    std::string extractLocationPath(const std::string &line);
    void parseListen(const std::string &line, ServerConfig &serverConfig);
    void parseLocation(std::ifstream &configFile, LocationConfig &locationConfig);
    void parseLocationLine(const std::string &line, LocationConfig &locationConfig);
    static void printServerConfig(const ServerConfig& serverConfig);
    static void printLocationConfig(const std::string& locationPath, const LocationConfig& locationConfig);
};
