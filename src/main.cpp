#include "../inc/Config.hpp"
#include "../inc/Server.hpp"
#include "../inc/TCPSocket.hpp"

int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <configuration file>"
			  << std::endl;
		return 1;
	}
	std::vector<ServerConfig> serverConfigs;
	try {
		Config config(argv[1]);
		serverConfigs = config.getServerConfigs();
		Config::printConfigs(serverConfigs);
		std::vector<TCPSocket *> sockets = createSockets(serverConfigs);
		Server server(sockets);
		server.runServers();
	} catch (const std::exception &e) {
		std::cerr << "error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
