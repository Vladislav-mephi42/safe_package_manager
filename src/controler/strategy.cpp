#include "strategy.h"
#include "package/empty_package.h"
#include "package/main_package.h"
#include "package/package.h"
#include "package/support_package.h"
#include "package_manager/package_manager.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>

bool Default_read::can_read(const std::string &name) const {
  if (name.length() < 4) {
    return false;
  }
  if (name.substr(name.length() - 4, name.length()) != ".dep") {
    return false;
  }
  return true;
}

std::shared_ptr<Package> Default_read::read_package(json &data,
                                                    json *req_packages) const {

  for (const auto &strategy : des_strategies) {
    if (strategy->can_read(data["type"])) {
      return strategy->read(data, req_packages);
    }
  }
  throw std::runtime_error("unknown type of package");
}

std::shared_ptr<Package>
Default_read::read_package(const std::string &package_file_name, json &data,
                           std::vector<std::string> &added_packages) const {
  auto it = std::find(added_packages.begin(), added_packages.end(),
                      package_file_name);
  if (it != added_packages.end()) {
    throw std::runtime_error("cycle found in json base");
  }
  json package_data = P_IOF::find_package(data, package_file_name);
  json req_packages;

  std::shared_ptr<Package> package = read_package(package_data, &req_packages);

  added_packages.push_back(package->get_file_name());
  for (const auto &elem : req_packages) {
    std::string fn = elem;
    auto req_package = read_package(fn, data, added_packages);
    package->insert_connected(req_package);
  }
  return package;
}

std::shared_ptr<Package>
Default_read::read_package(const std::string &file_name, json &data) const {
  json package_data = P_IOF::find_package(data, file_name);
  json req_packages;
  std::shared_ptr<Package> package = read_package(package_data, &req_packages);
  std::vector<std::string> added_packages;
  added_packages.push_back(file_name);
  for (const auto &elem : req_packages) {
    std::string fn = elem;
    auto req_package = read_package(fn, data, added_packages);
    package->insert_connected(req_package);
  }
  return package;
}

bool Empty_read::can_read(const std::string &name) const {
  if (name.length() < 5) {
    return false;
  }
  if (name.substr(name.length() - 5, name.length()) != "-last") {
    return false;
  }
  return true;
}

std::shared_ptr<Package> Empty_read::read_package(json &data,
                                                  json *req_packages) const {

  for (const auto &strategy : des_strategies) {
    if (strategy->can_read(data["type"])) {
      return strategy->read(data, req_packages);
    }
  }
  throw std::runtime_error("unknown type of package");
}

std::shared_ptr<Package>
Empty_read::read_package(const std::string &package_file_name, json &data,
                         std::vector<std::string> &added_packages) const {
  auto it = std::find(added_packages.begin(), added_packages.end(),
                      package_file_name);
  if (it != added_packages.end()) {
    throw std::runtime_error("cycle found in json base");
  }
  json package_data = P_IOF::find_package(data, package_file_name);
  json req_packages;

  std::shared_ptr<Package> package = read_package(package_data, &req_packages);

  added_packages.push_back(package->get_file_name());
  for (const auto &elem : req_packages) {
    std::string fn = elem;
    auto req_package = read_package(fn, data, added_packages);
    package->insert_connected(req_package);
  }
  return package;
}

std::shared_ptr<Package>
Empty_read::read_package_using_file_name(const std::string &file_name,
                                         json &data) const {
  json package_data = P_IOF::find_package(data, file_name);
  json req_packages;
  std::shared_ptr<Package> package = read_package(package_data, &req_packages);
  std::vector<std::string> added_packages;
  added_packages.push_back(file_name);
  for (const auto &elem : req_packages) {
    std::string fn = elem;
    auto req_package = read_package(fn, data, added_packages);
    package->insert_connected(req_package);
  }
  return package;
}
std::shared_ptr<Package>
Empty_read::read_package(const std::string &package_name,
                         json &input_data) const {
  std::string prefix = package_name.substr(0, package_name.length() - 5);

  if (!input_data.contains("packages") || !input_data["packages"].is_array()) {
    throw std::runtime_error("json format error");
  }
  json data = input_data["packages"];
  json package;
  json last_package;
  bool flag = false;
  for (const auto &elem : data) {
    if (elem["package_name"] == package_name && elem["type"] == "empty") {
      flag = true;
      package = elem;
      break;
    }
  }
  if (!flag) {
    throw std::runtime_error("can't find package");
  }
  flag = false;
  last_package = package;
  for (const auto &elem : data) {
    if (elem["package_name"] == prefix &&
        elem["current_version"] > last_package["current_version"] &&
        elem["type"] != "empty") {
      flag = true;
      last_package = elem;
      break;
    }
  }
  if (!flag) {
    return read_package_using_file_name(package["file_name"], input_data);
  } else {
    std::shared_ptr<Package> linked_package =
        read_package_using_file_name(last_package["file_name"], input_data);
    return std::make_shared<Empty_package>(
        Empty_package(package_name, linked_package));
  }
}

/////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

bool contains_package(const json &array, const std::string &file_name) {

  if (!array.is_array()) {
    throw std::runtime_error("bad format");
  }
  for (const auto &elem : array) {

    if (elem["file_name"] == file_name) {

      return true;
    }
  }
  return false;
}
bool contains_package_2(const json &array, const std::string &file_name) {

  if (!array.is_array()) {
    throw std::runtime_error("bad format");
  }
  for (const auto &elem : array) {
    if (elem["file_name"] == file_name) {

      return true;
    }
  }
  return false;
}

json P_IOF::find_package(const std::string &input_file_name,
                         const std::string &package_file_name) {
  std::ifstream file(input_file_name);
  if (!file.is_open()) {
    throw std::runtime_error("cann`t open file " + input_file_name);
  }

  json data;
  file >> data;

  if (!data.contains("packages") || !data["packages"].is_array()) {
    throw std::runtime_error("json format error");
  }

  for (const auto &package : data["packages"]) {
    if (package.contains("file_name") &&
        package["file_name"] == package_file_name) {
      return package;
    }
  }
  file.close();
  throw std::runtime_error(" package is  not founded");
}

json P_IOF::find_package(json &data, const std::string &package_file_name) {

  if (!data.contains("packages") || !data["packages"].is_array()) {
    throw std::runtime_error("json format error");
  }

  for (const auto &package : data["packages"]) {
    if (package.contains("file_name") &&
        package["file_name"] == package_file_name) {
      return package;
    }
  }
  throw std::runtime_error(" package is  not founded");
}

json P_IOF::find_package(std::istream &in,
                         const std::string &package_file_name) {

  json data;
  in >> data;

  if (!data.contains("packages") || !data["packages"].is_array()) {
    throw std::runtime_error("json format error");
  }

  for (const auto &package : data["packages"]) {
    if (package.contains("file_name") &&
        package["file_name"] == package_file_name) {
      return package;
    }
  }
  throw std::runtime_error(" package is  not founded");

  return json();
}

void P_IOF::write_package_to_json(const std::shared_ptr<Package> &package,
                                  json &new_data) {
  Package_manager::cycle_check(package);
  if (!contains_package(new_data, package->get_file_name())) {
    new_data.push_back(package->write_to_json());
  } else {
    throw std::runtime_error("unreal package");
  }
  std::vector<std::string> added_packages = {package->get_file_name()};
  for (const auto &elem : package->get_connected_packages()) {
    write_package_to_json(elem, new_data);
  }
}

void P_IOF::write_package_to_file(const std::shared_ptr<Package> &package,
                                  const std::string &output_file_name) {
  std::ifstream input(output_file_name);

  if (!input.is_open()) {
    throw std::runtime_error("Can't open file");
  }
  json data;
  try {
    input >> data;
  } catch (const json::parse_error &e) {
    std::cerr << "Parsing file error" << e.what() << std::endl;
  }
  input.close();

  std::ofstream output(output_file_name);

  if (!output.is_open()) {
    throw std::runtime_error("Can't open file");
  }
  json new_data;
  new_data = json::array();

  write_package_to_json(package, new_data);
  for (const auto &elem : new_data) {
    if (!contains_package(data["packages"], elem["file_name"])) {
      data["packages"].push_back(elem);
    }
  }

  output << data.dump(4);
  output.close();
}
