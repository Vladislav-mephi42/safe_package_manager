#ifndef SERVER_H
#define SERVER_H

#include "client.h"
#include <algorithm>
#include <arpa/inet.h>
#include <csignal>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <nlohmann/json.hpp>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using json = nlohmann::json;

#ifndef htonll
#define htonll(x) ((uint64_t)htonl((x) & 0xFFFFFFFF) << 32 | htonl((x) >> 32))
#endif
#ifndef ntohll
#define ntohll(x) ((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32 | ntohl((x) >> 32))
#endif

class Server_socket {
private:
  int server_socket;

public:
  Server_socket(size_t port, size_t sin_addr = INADDR_ANY) {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(sin_addr);
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
      throw std::runtime_error("socket creation error");
    }

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (bind(server_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
      close(server_socket);
      throw std::runtime_error("socket binding error");
    }
  }

  Server_socket(const Server_socket &) = delete;
  Server_socket &operator=(const Server_socket &) = delete;
  ~Server_socket() {
    if (server_socket >= 0) {
      close(server_socket);
    }
  }
  void manuly_close() {
    if (server_socket >= 0) {
      close(server_socket);
      server_socket = -1;
    }
  }
  Server_socket(Server_socket &&other) noexcept
      : server_socket(other.server_socket) {
    other.server_socket = -1;
  }
  Server_socket &operator=(Server_socket &&other) noexcept {
    if (this != &other) {
      if (server_socket >= 0)
        close(server_socket);
      server_socket = other.server_socket;
      other.server_socket = -1;
    }
    return *this;
  }
  int get_server_socket() const noexcept { return server_socket; }
  Client_socket accept() {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_socket = ::accept(server_socket, nullptr, nullptr);

    if (client_socket < 0) {
      throw std::runtime_error("Accept failed");
    }

    return Client_socket(client_socket);
  }
};

#endif