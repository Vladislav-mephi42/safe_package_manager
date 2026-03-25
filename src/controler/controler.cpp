
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

json Controler::find_package(const std::string &input_file_name,
                             const std::string &file_name) {
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
    if (package.contains("file_name") && package["file_name"] == file_name) {
      return package;
    }
  }
  file.close();
  throw std::runtime_error(" package is  not founded");
}

json Controler::find_package(json &data, const std::string &file_name) {

  if (!data.contains("packages") || !data["packages"].is_array()) {
    throw std::runtime_error("json format error");
  }

  for (const auto &package : data["packages"]) {
    if (package.contains("file_name") && package["file_name"] == file_name) {
      return package;
    }
  }
  throw std::runtime_error(" package is  not founded");
}

json Controler::find_package(std::istream &in, const std::string &file_name) {

  json data;
  in >> data;

  if (!data.contains("packages") || !data["packages"].is_array()) {
    throw std::runtime_error("json format error");
  }

  for (const auto &package : data["packages"]) {
    if (package.contains("file_name") && package["file_name"] == file_name) {
      return package;
    }
  }
  throw std::runtime_error(" package is  not founded");

  return json();
}

std::shared_ptr<Package> Controler::read_package(json &data,
                                                 json *req_packages) {

  for (const auto &strategy : strategies) {
    if (strategy->can_read(data["type"])) {
      return strategy->read(data, req_packages);
    }
  }
  throw std::runtime_error("unknown type of package");
}

std::shared_ptr<Package> Controler::read_package(const std::string &file_name,
                                                 json &data) {
  json package_data = find_package(data, file_name);
  json req_packages;
  std::shared_ptr<Package> package = read_package(package_data, &req_packages);

  for (const auto &elem : req_packages) {
    std::string fn = elem;
    auto req_package = read_package(fn, data);
    package->insert_connected(req_package);
  }
  return package;
}
std::shared_ptr<Package>
Controler::read_package_from_file(const std::string &file_name,
                                  const std::string &input_file_name) {
  std::ifstream file(input_file_name);

  if (!file.is_open()) {
    throw std::runtime_error("Can't open file");
  }
  json data;
  try {
    file >> data;
  } catch (const json::parse_error &e) {
    std::cerr << "Parsing file error" << e.what() << std::endl;
  }
  file.close();
  return read_package(file_name, data);
}

void Controler::read_package_manager_from_file(
    const std::string &input_file_name, Package_manager &pm) {
  std::ifstream file(input_file_name);

  if (!file.is_open()) {
    throw std::runtime_error("Can't open file");
  }
  json data;
  try {
    file >> data;
  } catch (const json::parse_error &e) {
    std::cerr << "Parsing file error" << e.what() << std::endl;
  }
  file.close();
  if (!data.contains("packages")) {
    throw std::runtime_error("deserealization error");
  }
  for (const auto &elem : data["packages"]) {
    if (elem["using_flag"] == true) {
      auto package = read_package(elem["file_name"], data);
      pm.add(package);
    }
  }
}
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
  std::cout << "ARRAY" << std::endl;
  std::cout << array.dump(4) << std::endl;
  for (const auto &elem : array) {

    if (elem["file_name"] == file_name) {

      return true;
    }
    std::cout << "Name of package on check  " << file_name << " Name of elem"
              << elem["file_name"] << std::endl;
  }
  return false;
}
void write_package_to_json(const std::shared_ptr<Package> &package,
                           json &data) {
  if (!contains_package(data["packages"], package->get_file_name())) {
    data["packages"].push_back(package->write_to_json());
  }
  for (const auto &elem : package->get_connected_packages()) {
    write_package_to_json(elem, data);
  }
}

void Controler::write_package_to_file(const std::shared_ptr<Package> &package,
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
  write_package_to_json(package, data);

  output << data.dump(4);
  output.close();
}

void Controler::write_package_manager_to_file(
    const std::string &output_file_name, Package_manager &pm) {

  std::ofstream output(output_file_name);
  json data;
  data["packages"] = json::array();
  if (!output.is_open()) {
    throw std::runtime_error("Can't open file");
  }
  for (const auto &elem : pm.map) {
    auto package = elem.second;

    write_package_to_json(package, data);
  }

  output << data.dump(4);
  output.close();
}

void Controler::add_package(const std::string &file_name) {
  if (pm == nullptr) {
    throw std::runtime_error(
        "there is no package manager being monitored by the controller");
  }
  for (const auto &elem : json_repozitories_names) {

    std::ifstream file(elem);
    if (file.is_open()) {
      json data;
      file >> data;
      if (data.contains("packages")) {

        if (contains_package(data["packages"], file_name)) {

          auto package = read_package(file_name, data);
          try {
            pm->add(package);
          } catch (const std::exception &e) {
            std::string what(e.what());
            if (what == "cycle found") {
              pm->cycle_destroy(package);
            }
            throw std::runtime_error("cycle found");
          }
          write_package_to_file(package, storage_file_name);
          return;
        }
      }
      file.close();
    }

    else {
      throw std::runtime_error("Can`t open file");
    }
  }

  throw std::runtime_error("No such package in repozitories");
}

void Controler::remove_package(const std::string &file_name) {
  if (pm == nullptr) {
    throw std::runtime_error(
        "there is no package manager being monitored by the controller");
  }
  std::ifstream ifile(storage_file_name);
  if (ifile.is_open()) {
    json data;
    ifile >> data;
    pm->remove(file_name);
    ifile.close();
    write_package_manager_to_file(storage_file_name, *pm);
  } else {
    throw std::runtime_error("cann`t open storage");
  }
}