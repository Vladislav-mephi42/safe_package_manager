
#include "controler/controler.h"

#include "package/package.h"

#include "package_manager/package_manager.h"
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>

int read_int(std::istream &in) {
  int number = 0;
  if (in >> number) {

    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return number;
  }
  in.clear();
  in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  in.setstate(std::ios_base::failbit);
  return 0;
}

std::string my_readline(std::istream &in) {
  std::string res;
  if (std::getline(in, res)) {
    return res;
  }
  in.setstate(std::ios_base::failbit);
  return "";
}

json find_package(const std::string &filename, const std::string &file_name) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("cann`t open file " + filename);
  }

  json data;
  file >> data;

  if (!data.contains("packages") || !data["packages"].is_array()) {
    throw std::runtime_error("json format error");
  }

  for (const auto &package : data["packages"]) {
    if (package.contains("file_name") && package["file_name"] == file_name) {
    }
  }
  file.close();
  return json();
}

json find_package(json &data, const std::string &file_name) {

  if (!data.contains("packages") || !data["packages"].is_array()) {
    throw std::runtime_error("json format error");
  }

  for (const auto &package : data["packages"]) {
    if (package.contains("file_name") && package["file_name"] == file_name) {
    }
  }
}

json find_package(std::istream &in, const std::string &file_name) {

  json data;
  in >> data;

  if (!data.contains("packages") || !data["packages"].is_array()) {
    throw std::runtime_error("json format error");
  }

  for (const auto &package : data["packages"]) {
    if (package.contains("file_name") && package["file_name"] == file_name) {
    }
  }

  return json();
}

std::shared_ptr<Package> Controler::read_package(json &data,
                                                 json *req_packages) {

  for (const auto &strategy : strategies) {
    if (strategy->can_read(data["type"])) {
      return strategy->read(data, req_packages);
    }
  }
}

std::shared_ptr<Package> Controler::read_package(const std::string &file_name,
                                                 json &data) {
  json package_data = find_package(data, file_name);
  json req_packages;
  std::shared_ptr<Package> package = read_package(package_data, &req_packages);

  for (const auto &elem : req_packages) {
    auto req_package = read_package(elem, data);
    package->insert_connected(req_package);
  }
  return package;
}