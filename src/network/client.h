#ifndef CLIENT_H
#define CLIENT_H

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

const int PORT = 49152;
const size_t BUFFER_SIZE = 4096;
#ifndef htonll
#define htonll(x) ((uint64_t)htonl((x) & 0xFFFFFFFF) << 32 | htonl((x) >> 32))
#endif
#ifndef ntohll
#define ntohll(x) ((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32 | ntohl((x) >> 32))
#endif

class Client_socket {
private:
  int client_socket;

public:
  Client_socket(size_t port, const char *server_ip) {
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
      throw std::runtime_error("socket creation error");
    }
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(server_ip);
    if (connect(client_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
      close(client_socket);
      throw std::runtime_error("bad connecting");
    }
  }
  explicit Client_socket(int socket) : client_socket(socket) {}
  Client_socket(const Client_socket &) = delete;
  Client_socket &operator=(const Client_socket &) = delete;
  ~Client_socket() {
    if (client_socket >= 0) {
      close(client_socket);
    }
  }
  void manuly_close() {
    if (client_socket >= 0) {
      close(client_socket);
      client_socket = -1;
    }
  }
  Client_socket(Client_socket &&other) noexcept
      : client_socket(other.client_socket) {
    other.client_socket = -1;
  }
  Client_socket &operator=(Client_socket &&other) noexcept {
    if (this != &other) {
      if (client_socket >= 0)
        close(client_socket);
      client_socket = other.client_socket;
      other.client_socket = -1;
    }
    return *this;
  }
  int get_client_socket() const noexcept { return client_socket; }

  void send(const void *buffer, size_t length) const {
    size_t total_sent = 0;
    while (total_sent < length) {
      ssize_t sent = ::send(client_socket, (const char *)buffer + total_sent,
                            length - total_sent, 0);
      if (sent < 0) {
        if (errno == EINTR)
          continue;
        throw std::runtime_error("send failed: " +
                                 std::string(strerror(errno)));
      }
      if (sent == 0) {
        throw std::runtime_error("connection closed by peer");
      }
      total_sent += sent;
    }
  }
  void send_json(const nlohmann::json &data) const {
    std::string json_str = data.dump();
    uint32_t length = static_cast<uint32_t>(json_str.size());
    uint32_t length_net = htonl(length);
    send(&length_net, sizeof(length_net));
    send(json_str.c_str(), length);
  }

  json recv_json() {

    uint32_t length_net;
    if (!recv(&length_net, sizeof(length_net))) {
      throw std::runtime_error("network error");
    }
    uint32_t length = ntohl(length_net);

    if (length > 10 * 1024 * 1024) {
      throw std::runtime_error(
          "error: the maximum size of json has been reached ");
    }

    std::string json_str(length, '\0');
    size_t received = 0;
    while (received < length) {
      ssize_t bytes =
          ::recv(client_socket, &json_str[received], length - received, 0);
      if (bytes <= 0) {
        throw std::runtime_error("network error");
      }
      received += static_cast<size_t>(bytes);
    }

    return json::parse(json_str);
  }
  bool recv(void *buffer, size_t length) {
    size_t total_received = 0;
    while (total_received < length) {
      ssize_t received = ::recv(client_socket, (char *)buffer + total_received,
                                length - total_received, 0);
      if (received <= 0)
        return false;
      total_received += received;
    }
    return true;
  }

  bool send_update_request();
  bool send_admin_add_request(const std::string &password);
  bool send_admin_remove_request(const std::string &password);
  json send_admin_find_request(const std::string &password);
};

#endif