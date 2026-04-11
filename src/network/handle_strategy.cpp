#include "handle_strategy.h"
#include "client.h"
#include "controler/controler.h"
#include "server.h"
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

#ifndef htonll
#define htonll(x) ((uint64_t)htonl((x) & 0xFFFFFFFF) << 32 | htonl((x) >> 32))
#endif
#ifndef ntohll
#define ntohll(x) ((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32 | ntohl((x) >> 32))
#endif
static std::hash<std::string> hasher;
static size_t hash = hasher("secret_password");

//////////////////////////////////////////////////////////
/////REQUEST//////////////////////////////////////////////
/// 1) user_type
/// 2) request_type
/// 3) param_1
/// 4) .......
/// 5) param_n
/////////////////////////////////////////////////////////
/////ERROR_RESPONSE//////////////////////////////////////
/// 1) status ("fail")
/// 2) error_message
/////////////////////////////////////////////////////////
/////OK_RESPONSE/////////////////////////////////////////
/// 1) status ("ok")
/// 2) param_1
/////////////////////////////////////////////////////////
/////UPDATE_RESPONSE/////////////////////////////////////
/// 1) status ("ok")
/// 2) number of files
/// #file_name in json response, must be deleted
/////OTHER_RESPONSE
/// 1) status
/// 2) message
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

bool password_check(const std::string &password) {
  if (hasher(password) == hash) {
    return true;
  }
  return false;
}

bool User_update_request::can_handle(const std::string &request_type) const {
  if (request_type == "update") {
    return true;
  }
  return false;
}
// Если вдруг происходит ошибка то метод звершает свою работу, в конце уже в
// вызывающем методе все равно отравится завершающее сообщение с подробным
// отзывом
json User_update_request::handle(const json &request,
                                 const Client_socket &socket) const {

  if (!request.contains("file_names")) {
    json error;
    error["status"] = "fail";
    error["final"] = "bad json format: file_names is missing";

    return error;
  }
  if (!request["file_names"].is_array()) {
    json error;
    error["status"] = "fail";
    error["final"] = "bad json format: file_names is missing";

    return error;
  }
  std::vector<std::ifstream> files;
  for (const auto &elem : request["file_names"]) {
    auto file_name = elem.get<std::string>();
    std::ifstream file(file_name);
    if (!file.is_open()) {
      for (auto &f : files) {
        f.close();
      }
      json error;
      error["status"] = "fail";
      error["final"] = "bad json format: file_name is invalid";
      return error;
    }
    files.push_back(std::move(file));
  }
  int c = 0;
  for (auto &elem : files) {
    json data;
    elem >> data;
    data["file_name"] = request["file_names"][c];
    socket.send_json(data);
    c++;
  }
  json ok_response;
  ok_response["status"] = "ok";

  ok_response["final"] = "operation end succesfuly";

  return ok_response;
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
bool Admin_add_strategy::can_handle(const std::string &request_type) const {

  if (request_type == "add") {
    return true;
  }
  return false;
}
json Admin_add_strategy::handle(json &request, Client_socket &socket,
                                Controler &controler) const {
  if (!request.contains("password")) {
    json error;
    error["status"] = "fail";
    error["final"] = "bad json format: password is missing";
    return error;
  }
  if (!password_check(request["password"])) {
    json error;
    error["status"] = "fail";
    error["final"] = "bad password";
    return error;
  }
  if (!(request.contains("package") && request.contains("package_name"))) {
    json error;
    error["status"] = "fail";
    error["final"] = "bad json format";
    return error;
  }
  try {
    auto package = controler.read_package(
        request["package_name"].get<std::string>(), request["package"]);
    controler.add_package_external(package);
  } catch (const std::exception &e) {
    json error;
    error["status"] = "fail";
    error["final"] = e.what();
    return error;
  }
  json data;
  data["status"] = "ok";
  data["final"] = "adding is successfully";
  return data;
}
///////////////////////////////////////////////////////////
bool Admin_remove_strategy::can_handle(const std::string &request_type) const {
  if (request_type == "remove") {
    return true;
  }
  return false;
}
json Admin_remove_strategy::handle(json &request, Client_socket &socket,
                                   Controler &controler) const {
  if (!request.contains("password")) {
    json error;
    error["status"] = "fail";
    error["final"] = "bad json format: password is missing";
    return error;
  }
  if (!password_check(request["password"])) {
    json error;
    error["status"] = "fail";
    error["final"] = "bad password";
    return error;
  }
  if (!request.contains("password")) {
    json error;
    error["status"] = "fail";
    error["final"] = "bad json format: password is missing";
    return error;
  }
  if (!password_check(request["password"])) {
    json error;
    error["status"] = "fail";
    error["final"] = "bad password";
    return error;
  }
  if (!request.contains("package_name")) {
    json error;
    error["status"] = "fail";
    error["final"] = "bad json format";
    return error;
  }
  try {
    controler.remove_package(request["package_name"].get<std::string>());

  } catch (const std::exception &e) {
    json error;
    error["status"] = "fail";
    error["final"] = e.what();
    return error;
  }

  json data;
  data["status"] = "ok";
  data["final"] = "removing is successfully";
  return data;
}
