// Program to test leaking TCP connections.

// build:  gcc -Wall -Wextra -pedantic -std=c++17 -lstdc++ x.cc

#include <cstring>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace
{

const std::uint16_t Server_port{54321};

const unsigned Num_connections{3}; // Must be a multiple of 3.

const auto IPv4_loopback_addr_nbo{inet_addr("127.0.0.1")};

void error(const char *msg)
{
  perror(msg);
  std::exit(1);
}

} // end anonymous namespace

int main()
{
  int listen_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_sock_fd < 0) 
    error("ERROR creating listen socket");

  sockaddr_in serv_addr;
  std::memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = IPv4_loopback_addr_nbo;
  serv_addr.sin_port = htons(Server_port);
  if (bind(listen_sock_fd, reinterpret_cast<sockaddr *>(&serv_addr), sizeof(serv_addr)) < 0) 
    error("ERROR on binding listen socket");
  listen(listen_sock_fd, 1);

  int client_sock_fd[Num_connections], server_sock_fd[Num_connections];

  for (unsigned i{0}; i < Num_connections; ++i) {
    client_sock_fd[i] = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock_fd[i] < 0) {
      std::fprintf(stderr, "error for connection %u\n", i);
      error("ERROR creating connection socket");
    }
    if (connect(client_sock_fd[i], reinterpret_cast<struct sockaddr *>(&serv_addr), sizeof(serv_addr)) < 0) {
      std::fprintf(stderr, "error for connection %u\n", i);
      error("ERROR in connect() call");
    }

    sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    server_sock_fd[i] = accept(listen_sock_fd, reinterpret_cast<sockaddr *>(&cli_addr), &clilen);
    if (server_sock_fd[i] < 0) {
      std::fprintf(stderr, "error for connection %u\n", i);
      error("ERROR in accept() call");
    }
  }

  unsigned i{0};

  std::printf("Hit enter to close client sockets index 0 to %u:", (Num_connections / 3) - 1);
  fgetc(stdin);

  for (; i < (Num_connections / 3); ++i) {
    if (close(client_sock_fd[i])) {
      std::fprintf(stderr, "error for connection %u\n", i);
      error("ERROR in close() call for client side");
    }
  }

  std::printf("Hit enter to close server sockets index %u to %u:", Num_connections / 3, (2 * (Num_connections / 3)) - 1);
  fgetc(stdin);

  for (; i < (2 * (Num_connections / 3)); ++i) {
    if (close(server_sock_fd[i])) {
      std::fprintf(stderr, "error for connection %u\n", i);
      error("ERROR in close() call for server side");
    }
  }

  std::printf("Hit enter to close remaining connection sockets:");
  fgetc(stdin);

  i = 0;

  for (; i < (Num_connections / 3); ++i) {
    if (close(server_sock_fd[i])) {
      std::fprintf(stderr, "error for connection %u\n", i);
      error("ERROR in close() call for server side");
    }
  }

  for (; i < (2 * (Num_connections / 3)); ++i) {
    if (close(client_sock_fd[i])) {
      std::fprintf(stderr, "error for connection %u\n", i);
      error("ERROR in close() call for client side");
    }
  }

  for (; i < Num_connections; ++i) {
    if (close(client_sock_fd[i])) {
      std::fprintf(stderr, "error for connection %u\n", i);
      error("ERROR in close() call for client side");
    }
    if (close(server_sock_fd[i])) {
      std::fprintf(stderr, "error for connection %u\n", i);
      error("ERROR in close() call for server side");
    }
  }

  return 0; 
}
