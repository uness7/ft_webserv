#include "../inc/Server.hpp"
#include "../inc/TCPSocket.hpp"

int main(void) {
  std::vector<TCPSocket> s;
  s.push_back(TCPSocket("0.0.0.0", 8000));
  s.push_back(TCPSocket("0.0.0.0", 8010));
  s.push_back(TCPSocket("0.0.0.0", 8020));
  Server server(s);
  server.startListen();
  return 0;
}
