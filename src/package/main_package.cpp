#include "package/main_package.h"
#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

void Main_package::add() {
  if (!condition) {
    condition = true;
  }
}

void Main_package::remove() { condition = false; }

std::ostream &Main_package::write(std::ostream &out) {
  json j;
  j["type"] = "main";
  j["using_flag"] = using_flag;
  j["file_name"] = file_name;
  j["publisher_name"] = publisher_name;
  j["current_version"] = current_version;
  j["last_version"] = last_version;
  j["req_packages"] = json::array();
  for (const auto &pkg : req_packages) {
    j["req_packages"].push_back(pkg->get_file_name());
  }
  out << j;
  return out;
}

json read_int(std::istream &in);
std::string my_readline(std::istream &in);
std::istream &
read_req_packages(std::istream &in,
                  std::vector<std::shared_ptr<Package>> &req_packages,
                  std::vector<std::shared_ptr<Read_strategy>> &strategies,
                  int req_packages_count);

json Main_package::read(std::istream &in) {
  json j;
  in >> j;

  if (j["type"] != "main") {
    throw std::runtime_error("deserealization error");
  }
  set_using_flag(j.at("using_flag").get<bool>());
  set_file_name(j.at("file_name").get<std::string>());
  set_publisher_name(j.at("publisher_name").get<std::string>());
  set_current_version(j.at("current_version").get<std::string>());
  set_last_version(j.at("last_version").get<std::string>());

  if (j.contains("req_packages") && j["req_packages"].is_array()) {
    return j["req_packages"];
  }
  throw std::runtime_error("deserealization error");
}
