#ifndef SOCKETS_H
#define SOCKETS_H

#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <nlohmann/json.hpp>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using json = nlohmann::json;

/**
 * @brief Interface class for interacting with a socket.
 *
 */
class Socket {
protected:
  /**Socket descriptor */
  int fd;

public:
  /**
   * @brief Constructs a new Socket object.
   * Creates a socket using the specified domain, type, and protocol,
   * and stores the resulting file descriptor.
   * @param domain
   * @param type
   * @param protocol
   */
  Socket(int domain, int type, int protocol) {
    fd = socket(domain, type, protocol);
  }
  /**
   * @brief Constructs a new Socket object.
   *
   * @param fd_in
   */
  Socket(int fd_in) { fd = fd_in; }
  /**
   * @brief Destroys the Socket object.
   *
   */
  ~Socket() {
    if (fd != -1) {
      close(fd);
    }
  }

  Socket(const Socket &other) = delete;
  Socket &operator=(const Socket &other) = delete;
  /**
   * @brief Move constructor.
   *
   * @param other
   */
  Socket(Socket &&other) {
    fd = other.fd;
    other.fd = -1;
  }
  /**
   * @brief Move operator =.
   *
   * @param other
   * @return Socket&
   */
  Socket &operator=(Socket &&other) {
    std::swap(fd, other.fd);
    return *this;
  }
  /**
   * @brief Sends exactly len bytes from the given buffer.
   *
   * Sends all len bytes, retrying until the entire buffer is transmitted.
   * Throws a std::system_error if a send operation fails.
   * @param buf
   * @param len
   * @param flags
   */
  void send_n(const char *buf, int len, int flags = 0) const {
    int current = 0;
    int total = 0;

    while (current != len) {
      total = send(fd, buf + current, len - current, flags);
      if (total < 0) {
        throw std::system_error(errno, std::generic_category(), "bad send");
      }
      current += total;
    }
  }
  /**
   *@brief Receives exactly len bytes and stores them in the given buffer.
   *
   * Receives data until the entire buffer is filled, retrying as needed.
   * Throws a std::system_error if a receive operation fails, or a
   * std::runtime_error if the connection is closed before len bytes
   * are received.
   *
   * @param buf
   * @param len
   * @param flags
   */
  void recv_n(char *buf, int len, int flags = 0) const {
    int current = 0;
    int total = 0;

    while (current != len) {
      total = recv(fd, (void *)(buf + current), len - current, flags);
      if (total < 0) {
        throw std::system_error(errno, std::generic_category(), "bad send");
      }
      if (total == 0) {
        throw std::runtime_error("recv: connection closed by peer");
      }
      current += total;
    }
  }
  /**
   * @brief Sends len bytes from the given buffer, prefixed with a header
   * containing the length of the data in network byte order.
   *
   *
   * @param buf
   * @param len
   * @param flags
   */
  void auto_send(const char *buf, int len, int flags = 0) const {
    uint32_t inet_len = htonl((uint32_t)len);
    int res = send(fd, &inet_len, sizeof(inet_len), 0);
    if (res <= 0) {
      throw std::system_error(errno, std::generic_category(), "bad send");
    }
    send_n(buf, len, flags);
  }

  /**
   * @brief Receives data prefixed with a 4-byte length header and stores
   * the payload in the given buffer.
   *
   * @param buf
   * @param len
   * @param flags
   * @return int
   */

  int auto_recv(char *buf, int len, int flags = 0) const {
    uint32_t inet_len;
    int res = recv(fd, &inet_len, sizeof(inet_len), 0);
    if (res <= 0) {
      throw std::system_error(errno, std::generic_category(), "bad recv");
    }
    inet_len = ntohl(inet_len);
    if (inet_len > len) {
      throw std::runtime_error("inet len is bigger than can be");
    }
    recv_n(buf, (int)inet_len, flags);
    return (int)inet_len;
  }
  /**
   * @brief Sends the given JSON object.
   *
   * @param data
   */
  void send_json(const json &data) const {
    auto ser = data.dump();
    auto_send(ser.c_str(), ser.size());
  }
  /**
   * @brief Receives a JSON object.
   *
   * @return json
   */

  json recv_json() const {
    uint32_t inet_len;
    int res = recv(fd, &inet_len, sizeof(inet_len), 0);
    if (res <= 0) {
      throw std::system_error(errno, std::generic_category(), "bad recv");
    }
    int len = ntohl(inet_len);
    std::string buf;
    buf.resize(len);
    recv_n(buf.data(), len);
    return json::parse(buf);
  }
  /**
   * @brief Closes socket descriptor.
   *
   */
  void manualy_close() {
    if (fd != -1) {
      close(fd);
    }
    fd = -1;
  }
};
/**
 * @brief Interface class for interracting with socket as client.
 *
 */
class Client_socket : public Socket {
public:
  /**
   * @brief Constructs a new Client_socket object with specified domain, type,
   * protocol.
   * Creates a socket via the base Socket constructor and sets the
   * SO_REUSEADDR option to allow the socket to bind to an address
   * that is still in use.
   * @param domain
   * @param type
   * @param protocol
   */
  Client_socket(int domain, int type, int protocol)
      : Socket(domain, type, protocol) {
    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
      throw std::system_error(errno, std::generic_category(),
                              "setsockopt failed");
    }
  }
  /**
   * @brief Constructs a new Client_socket object
   *
   * @param fd
   */
  Client_socket(int fd) : Socket(fd) {}
  /**
   * @brief Connects socket to server.
   *
   * @param server_ip
   * @param port
   */
  void connect(const char *server_ip, uint16_t port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(server_ip);
    if (::connect(fd, reinterpret_cast<const sockaddr *>(&addr),
                  sizeof(addr)) == -1) {
      throw std::system_error(errno, std::generic_category(), "connect failed");
    }
  }
};
/**
 * @brief Interface class for interracting with socket as a server.
 *
 */
class Server_socket : public Socket {
public:
  /**
   * @brief Constructs a new Server_socket object with specified domain, type,
   * protocol.
   *
   * @param domain
   * @param type
   * @param protocol
   */
  Server_socket(int domain, int type, int protocol)
      : Socket(domain, type, protocol) {
    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
      throw std::system_error(errno, std::generic_category(),
                              "setsockopt failed");
    }
  }
  /**
   * @brief Binds the server socket to the specified port on all
   * available network interfaces.
   *
   * @param port
   */
  void bind(uint16_t port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (::bind(fd, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr)) ==
        -1) {
      throw std::system_error(errno, std::generic_category(), "bind failed");
    }
  }
  /**
   * @brief Starts listening for incoming connections on the bound port.
   *
   * @param backlog
   */
  void listen(int backlog) {
    if (backlog > SOMAXCONN) {
      throw std::logic_error("max backlog has been exceeded");
    }
    if (::listen(fd, backlog) == -1) {
      throw std::system_error(errno, std::generic_category(), "listen failed");
    }
  }
  /**
   * @brief Accepts an incoming client connection.
   *
   * @return Client_socket
   */
  Client_socket accept() {
    int client = ::accept(fd, NULL, NULL);
    if (client == -1) {
      throw std::system_error(errno, std::generic_category(), "bad_accept");
    }
    return Client_socket(client);
  }
};
#endif