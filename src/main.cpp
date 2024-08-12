#include "../inc/Server.hpp"
#include "../inc/TCPSocket.hpp"
#include "../inc/Config.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <configuration file>" << std::endl;
        return 1;
    }

    // Load and parse the configuration file
    std::vector<ServerConfig> serverConfigs;
    try
    {
        Config config(argv[1]);
        serverConfigs = config.getServerConfigs();
        //
        // Print the configuration details
        Config::printConfigs(serverConfigs);

        // Create sockets based on the configurations
        std::vector<TCPSocket*> sockets = createSockets(serverConfigs);

        // Initialize the server with the sockets and start listening
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
