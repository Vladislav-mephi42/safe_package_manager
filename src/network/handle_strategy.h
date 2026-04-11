#ifndef SERVER_STRATEGY_H
#define SERVER_STRATEGY_H
#include "client.h"

#include "controler/controler.h"
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
#include <vector>

#ifndef htonll
#define htonll(x) ((uint64_t)htonl((x) & 0xFFFFFFFF) << 32 | htonl((x) >> 32))
#endif
#ifndef ntohll
#define ntohll(x) ((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32 | ntohl((x) >> 32))
#endif
class Client_socket;

class User_request_strategy {
public:
  virtual bool can_handle(const std::string &request_type) const = 0;
  virtual json handle(const json &request,
                      const Client_socket &socket) const = 0;
};

class Admin_request_strategy {
public:
  virtual bool can_handle(const std::string &request_type) const = 0;
  virtual json handle(json &request, Client_socket &socket,
                      Controler &controler) const = 0;
};

class User_update_request : public User_request_strategy {
public:
  bool can_handle(const std::string &request_type) const override;
  json handle(const json &request, const Client_socket &socket) const override;
};

class Admin_add_strategy : public Admin_request_strategy {
public:
  bool can_handle(const std::string &request_type) const override;
  json handle(json &request, Client_socket &socket,

              Controler &controler) const override;
};

class Admin_remove_strategy : public Admin_request_strategy {
public:
  bool can_handle(const std::string &request_type) const override;
  json handle(json &request, Client_socket &socket,
              Controler &controler) const override;
};

class Handle_strategy {
public:
  virtual bool can_handle(const std::string &user_type) const = 0;
  virtual json handle(json &request, Client_socket &socket,
                      Controler &controler) const = 0;
};

class Handle_user_strategy : public Handle_strategy {
private:
  std::vector<std::shared_ptr<User_request_strategy>> strategies;

public:
  Handle_user_strategy() {
    User_update_request update;
    strategies.push_back(std::make_shared<User_update_request>(update));
  }
  Handle_user_strategy(const Handle_user_strategy &other) = default;
  ~Handle_user_strategy() = default;
  bool can_handle(const std::string &user_type) const override {
    if (user_type == "user") {
      return true;
    }
    return false;
  }
  json handle(json &request, Client_socket &socket,
              Controler &controler) const override {
    if (!request.contains("request_type")) {
      throw std::runtime_error("bad request format");
    }
    for (const auto &elem : strategies) {

      if (elem->can_handle(request["request_type"])) {
        json response = elem->handle(request, socket);
        return response;
      }
    }
    throw std::runtime_error("bad request format");
  }
};

class Handle_admin_strategy : public Handle_strategy {
private:
  std::vector<std::shared_ptr<Admin_request_strategy>> strategies;

public:
  Handle_admin_strategy() {
    Admin_add_strategy add;
    Admin_remove_strategy remove;
    strategies.push_back(std::make_shared<Admin_add_strategy>(add));
    strategies.push_back(std::make_shared<Admin_remove_strategy>(remove));
  }
  Handle_admin_strategy(const Handle_admin_strategy &other) = default;
  ~Handle_admin_strategy() = default;
  bool can_handle(const std::string &user_type) const override {
    if (user_type == "admin") {
      return true;
    }
    return false;
  }
  json handle(json &request, Client_socket &socket,
              Controler &controler) const override {
    if (!request.contains("request_type")) {
      throw std::runtime_error("bad request format");
    }
    for (const auto &elem : strategies) {

      if (elem->can_handle(request["request_type"])) {
        json response = elem->handle(request, socket, controler);
        return response;
      }
    }
    throw std::runtime_error("bad request format");
  }
};

#endif