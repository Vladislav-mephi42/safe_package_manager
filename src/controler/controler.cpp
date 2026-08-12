
#include "controler/controler.h"

#include "package/package.h"

#include "package_manager/package_manager.h"
#include <algorithm>
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
/**
 * @brief find package in json file, return json package
 *
 * @param input_file_name
 * @param package_file_name
 * @return json
 */

std::shared_ptr<Package> Controler::read_package(const std::string &file_name,
                                                 json &data) const {
  auto a = read_strategies[0];
  for (const auto &elem : read_strategies) {
    if (elem->can_read(file_name)) {
      a = elem;
      return elem->read_package(file_name, data);
    }
  }
  std::cout << a->can_read(file_name) << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << file_name << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
  throw std::runtime_error("bad package name format can't read");
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
  if (!data.contains("packages")) {
    throw std::runtime_error("deserealization error");
  }
  for (const auto &elem : data["packages"]) {
    if (elem["using_flag"] == true) {
      auto package = read_package(elem["file_name"], data);
      pm->add(package);
    }
  }
}
bool contains_package_controler(const json &array,
                                const std::string &file_name) {

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

void Controler::write_package_manager_to_file(
    const std::string &output_file_name) {

  std::ofstream output(output_file_name);
  json data;
  data["packages"] = json::array();
  if (!output.is_open()) {
    throw std::runtime_error("Can't open file");
  }
  for (const auto &elem : pm->map) {
    auto package = elem.second;
    json new_data;
    new_data = json::array();
    P_IOF::write_package_to_json(package, new_data);
    for (const auto &elem_2 : new_data) {
      if (!contains_package_controler(data["packages"], elem_2["file_name"])) {
        data["packages"].push_back(elem_2);
      }
    }
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

        if (contains_package_controler(data["packages"], file_name)) {

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
          P_IOF::write_package_to_file(package, storage_file_name);
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

void Controler::add_package_external(const std::shared_ptr<Package> &package) {
  if (pm == nullptr) {
    throw std::runtime_error(
        "there is no package manager being monitored by the controller");
  }

  try {
    pm->add(package);
  } catch (const std::exception &e) {
    std::string what(e.what());
    if (what == "cycle found") {
      pm->cycle_destroy(package);
    }
    throw std::runtime_error("cycle found");
  }
  P_IOF::write_package_to_file(package, storage_file_name);
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
    write_package_manager_to_file(storage_file_name);
  } else {
    throw std::runtime_error("cann`t open storage");
  }
}

void Controler::remove_unuse() {
  if (pm == nullptr) {
    throw std::runtime_error(
        "there is no package manager being monitored by the controller");
  }
  std::ifstream ifile(storage_file_name);
  if (ifile.is_open()) {
    json data;
    ifile >> data;
    pm->remove_unuse();
    ifile.close();
    write_package_manager_to_file(storage_file_name);
  } else {
    throw std::runtime_error("cann`t open storage");
  }
}

void Controler::global_update() {
  if (pm == nullptr) {
    throw std::runtime_error(
        "there is no package manager being monitored by the controller");
  }
  std::ifstream ifile(storage_file_name);
  if (ifile.is_open()) {
    json data;
    ifile >> data;
    pm->global_update();
    ifile.close();
    write_package_manager_to_file(storage_file_name);
  } else {
    throw std::runtime_error("cann`t open storage");
  }
}

void Controler::correct_json(const std::string &file_name) {

  std::ifstream file(file_name);
  if (!file.is_open()) {
    throw std::runtime_error("bad json file");
  }
  json data;
  try {
    file >> data;
  } catch (...) {
    throw std::runtime_error("parsing error");
  }
  if (!data.contains("packages")) {
    throw std::runtime_error("bad json file");
  }
  if (!data["packages"].is_array()) {
    throw std::runtime_error("bad json file");
  }
}
