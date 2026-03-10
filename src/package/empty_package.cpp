#include "package/empty_package.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

void Empty_package::add() {
  if (!condition) {
    condition = true;
  }
}

void Empty_package::remove() { condition = false; }

std::ostream &Empty_package::write(std::ostream &out) {
  json j;
  j["type"] = "empty";
  std::stringstream sstr;

  linked_package->write(sstr);
  json linked = json::parse(sstr.str());
  std::string type = linked["type"];
  linked.erase("type");
  linked["linked_type"] = type;
  out << j << linked;
  return out;
}

std::string my_readline(std::istream &in);
std::istream &
read_linked_package(std::istream &in, std::shared_ptr<Package> &linked_package,
                    std::vector<std::shared_ptr<Read_strategy>> &strategies);
json Empty_package::read(std::istream &in) {
  json j;
  in >> j;
  if (j["type"] != "empty") {
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

  return j;
}