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
  j["package_name"] = package_name;
  j["publisher_name"] = publisher_name;
  j["current_version"] = current_version;
  j["last_version"] = last_version;
  j["req_packages"] = json::array();
  for (const auto &pkg : req_packages) {
    j["req_packages"].push_back(pkg->get_file_name());
  }
  out << j.dump(4);
  return out;
}

json Main_package::write_to_json() const {
  json j;
  j["type"] = "main";
  j["using_flag"] = using_flag;
  j["file_name"] = file_name;
  j["package_name"] = package_name;
  j["publisher_name"] = publisher_name;
  j["current_version"] = current_version;
  j["last_version"] = last_version;
  j["req_packages"] = json::array();
  for (const auto &pkg : req_packages) {
    j["req_packages"].push_back(pkg->get_file_name());
  }

  return j;
}
