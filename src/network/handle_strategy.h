#ifndef SERVER_STRATEGY_H
#define SERVER_STRATEGY_H

#include "controler/controler.h"
#include "sockets.h"
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
using json = nlohmann::json;
/**
 * @brief Interface for different strategies of handling user requests.
 *
 */
class User_request_strategy {
public:
  /**
   * @brief Checks whether a request with the given type can be handled
   * by this strategy.
   *
   * @return true if this strategy can handle the request type,
   *         false otherwise.
   */
  virtual bool can_handle(const std::string &request_type) const = 0;

  /**
   * @brief Handles the request and returns a report on the completed work.
   *
   * @return JSON object containing the result or report of the handling.
   */
  virtual json handle(const json &request,
                      const Client_socket &socket) const = 0;
};

/**
 * @brief Interface for strategies that handle administrator requests.
 */
class Admin_request_strategy {
public:
  /**
   * @brief Checks whether this strategy can handle the given request type.
   *
   * @return true if this strategy can handle the request, false otherwise.
   */
  virtual bool can_handle(const std::string &request_type) const = 0;

  /**
   * @brief Handles an administrator request and returns a report.
   *
   * @return JSON object containing the result.
   */
  virtual json handle(json &request, Client_socket &socket,
                      Controler &controler) const = 0;
};

/**
 * @brief Strategy for handling user update requests.
 */
class User_update_request : public User_request_strategy {
public:
  bool can_handle(const std::string &request_type) const override;
  json handle(const json &request, const Client_socket &socket) const override;
};

/**
 * @brief Strategy for handling administrator "add package" requests.
 */
class Admin_add_strategy : public Admin_request_strategy {
public:
  bool can_handle(const std::string &request_type) const override;
  json handle(json &request, Client_socket &socket,
              Controler &controler) const override;
};

/**
 * @brief Strategy for handling administrator "remove package" requests.
 */
class Admin_remove_strategy : public Admin_request_strategy {
public:
  bool can_handle(const std::string &request_type) const override;
  json handle(json &request, Client_socket &socket,
              Controler &controler) const override;
};

/**
 * @brief Interface for high-level strategies that dispatch requests
 * based on the user type (e.g., "user" or "admin").
 */
class Handle_strategy {
public:
  /**
   * @brief Checks whether this strategy can handle the given user type.
   *
   * @return true if this strategy can handle the user type, false otherwise.
   */
  virtual bool can_handle(const std::string &user_type) const = 0;

  /**
   * @brief Handles a request from a user of the corresponding type.
   *
   * @return JSON object containing the response.
   */
  virtual json handle(json &request, Client_socket &socket,
                      Controler &controler) const = 0;
};

/**
 * @brief High-level strategy for handling requests from regular users.
 *
 * Delegates to User_request_strategy implementations based on the
 * "request_type" field in the request JSON.
 */
class Handle_user_strategy : public Handle_strategy {
private:
  std::vector<std::shared_ptr<User_request_strategy>> strategies;

public:
  /** Constructs the strategy and registers available user request handlers. */
  Handle_user_strategy() {
    User_update_request update;
    strategies.push_back(std::make_shared<User_update_request>(update));
  }

  /**
   * @brief Returns true if the user type is "user".
   */
  bool can_handle(const std::string &user_type) const override {
    if (user_type == "user") {
      return true;
    }
    return false;
  }

  /**
   * @brief Handles a user request by delegating to the appropriate strategy.
   *
   * @throws std::runtime_error if the request lacks a "request_type" field
   *         or no strategy matches it.
   */
  json handle(json &request, Client_socket &socket,
              Controler &controler) const override {
    if (!request.contains("request_type")) {
      throw std::runtime_error("bad request format");
    }
    for (const auto &elem : strategies) {
      if (elem->can_handle(request["request_type"])) {
        json response = elem->handle(request, socket);
        socket.send_json(request);
        return response;
      }
    }
    throw std::runtime_error("bad request format");
  }
};

/**
 * @brief High-level strategy for handling requests from administrators.
 *
 * Delegates to Admin_request_strategy implementations based on the
 * "request_type" field in the request JSON.
 */
class Handle_admin_strategy : public Handle_strategy {
private:
  std::vector<std::shared_ptr<Admin_request_strategy>> strategies;

public:
  /** Constructs the strategy and registers available admin request handlers. */
  Handle_admin_strategy() {
    Admin_add_strategy add;
    Admin_remove_strategy remove;
    strategies.push_back(std::make_shared<Admin_add_strategy>(add));
    strategies.push_back(std::make_shared<Admin_remove_strategy>(remove));
  }

  Handle_admin_strategy(const Handle_admin_strategy &other) = default;
  ~Handle_admin_strategy() = default;

  /**
   * @brief Returns true if the user type is "admin".
   */
  bool can_handle(const std::string &user_type) const override {
    if (user_type == "admin") {
      return true;
    }
    return false;
  }

  /**
   * @brief Handles an admin request by delegating to the appropriate strategy.
   *
   * @throws std::runtime_error if the request lacks a "request_type" field
   *         or no strategy matches it.
   */
  json handle(json &request, Client_socket &socket,
              Controler &controler) const override {
    if (!request.contains("request_type")) {
      throw std::runtime_error("bad request format");
    }
    for (const auto &elem : strategies) {
      if (elem->can_handle(request["request_type"])) {
        json response = elem->handle(request, socket, controler);
        socket.send_json(response);
        return response;
      }
    }
    throw std::runtime_error("bad request format");
  }
};
#endif