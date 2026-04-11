#ifndef NETWORK_CONTROLER_H
#define NETWORK_CONTROLER_H
#include "client.h"

#include "controler/controler.h"
#include "handle_strategy.h"
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

class Server_network_controler {
private:
  std::vector<std::shared_ptr<Handle_strategy>> strategies;
  Controler *controler;

public:
  Server_network_controler(Controler *controler) : controler(controler) {
    if (controler == nullptr) {
      throw std::runtime_error("bad pounter");
    }
    Handle_admin_strategy admin;
    Handle_user_strategy user;
    strategies.push_back(std::make_shared<Handle_admin_strategy>(admin));
    strategies.push_back(std::make_shared<Handle_user_strategy>(user));
  }
  Server_network_controler(const Server_network_controler &other) = default;
  ~Server_network_controler() = default;

  bool can_handle(json &request) {
    if (!(request.contains("user_type") && request.contains("request_type"))) {
      throw std::runtime_error("bad json format");
    }
    for (const auto &elem : strategies) {
      if (elem->can_handle(request["user_type"])) {
        return true;
      }
    }
    return false;
  }

  json handle(json &request, Client_socket &socket) {
    for (const auto &elem : strategies) {
      if (elem->can_handle(request["user_type"])) {
        return elem->handle(request, socket, *controler);
      }
    }
    throw std::runtime_error("can't handle");
  }
};

class Client_network_controler {
private:
  std::vector<std::string> file_names;
  size_t port;
  std::string server_ip;

public:
  Client_network_controler(size_t port, const std::string &server_ip,
                           const std::vector<std::string> &file_names)
      : file_names(file_names), port(port), server_ip(server_ip) {}
  Client_network_controler(const Client_network_controler &other) = default;
  ~Client_network_controler() = default;
  void update() {
    Client_socket client(port, server_ip.c_str());
    json send_message;
    send_message["user_type"] = "user";
    send_message["file_names"] = json::array();
    for (const auto &elem : file_names) {
      send_message["file_names"].push_back(elem);
    }

    send_message["request_type"] = "update";

    client.send_json(send_message);
    json response;
    for (int i = 0; i < file_names.size(); i++) {
      response = client.recv_json();
      if (response["status"] == "fail" || !response.contains("file_name")) {
        return;
        throw std::runtime_error("server_error");
      } else {
        std::ofstream file(response["file_name"]);
        if (!file.is_open()) {
          return;
          throw std::runtime_error("file open error");
        }
        response.erase("file_name");
        file << response.dump(4);
        file.close();
      }
    }
  }
};

#endif