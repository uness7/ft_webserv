#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <map>

// Struct to store location-specific configurations
struct LocationConfig {
    std::string root; // Root directory for the location
    std::string index; // Index file for the location
    bool autoindex; // Autoindex flag
    std::vector<std::string> limit_except; // HTTP methods to limit
    std::string upload_store; // Directory to store uploads
    std::vector<std::string> allowed_methods;
};

// Struct to store server-specific configurations
struct ServerConfig {
    std::string listen; // IP address to listen on
    unsigned int port; // Port number
    std::string server_name; // Server name
    std::string error_page; // Default error page
    std::string client_max_body_size; // Maximum client body size
    std::map<std::string, LocationConfig> locations; // Map of locations
};

// Class that parses and stores the configuration file
class Config {
public:
    // Constructor and Destructor
    Config(const std::string &fileName);
    ~Config();

    // Member functions
    std::vector<ServerConfig> getServerConfigs() const; // Function to get server configurations
    static void printConfigs(const std::vector<ServerConfig>& serverConfigs); // Function to print configurations

private:
    std::vector<ServerConfig> _serverConfigs; // Vector to store server configurations

    // Private member functions
    void parseConfigFile(const std::string &fileName); // Function to parse the configuration file
    void parseServer(std::ifstream &configFile, ServerConfig &serverConfig); // Function to parse a server block
    void parseServerLine(std::ifstream &configFile, const std::string &line, ServerConfig &serverConfig); // Function to parse a server line
    std::string extractValue(const std::string &line, const std::string &key); // Function to extract value from a line
    std::string extractLocationPath(const std::string &line); // Function to extract location path
    void parseListen(const std::string &line, ServerConfig &serverConfig); // Function to parse listen directive
    void parseLocation(std::ifstream &configFile, LocationConfig &locationConfig); // Function to parse a location block
    void parseLocationLine(const std::string &line, LocationConfig &locationConfig); // Function to parse a location line
    static void printServerConfig(const ServerConfig& serverConfig); // Function to print server configuration
    static void printLocationConfig(const std::string& locationPath, const LocationConfig& locationConfig); // Function to print location configuration
};
