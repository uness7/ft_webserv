#include <iostream>
#include "Server.hpp"

int main(void)
{
	Server server("0.0.0.0", 8080);
	server.startListen();
	return 0;
}
