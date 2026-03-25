#ifndef CONTROLER
#define CONTROLER
#include "controler/strategy.h"
#include "package/package.h"
#include "package_manager/package_manager.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>
using json = nlohmann::json;

class Controler {
private:
  std::vector<std::shared_ptr<Read_strategy>> strategies;

  std::vector<std::string> json_repozitories_names;
  std::string storage_file_name;
  Package_manager *pm = nullptr;
  void correct_json(const std::string &file_name) {

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

public:
  Controler() : storage_file_name("default.json") {
    Empty_with_main_read empty;
    Support_read support;
    Main_read main;
    strategies.push_back(std::make_shared<Empty_with_main_read>(empty));
    strategies.push_back(std::make_shared<Support_read>(support));
    strategies.push_back(std::make_shared<Main_read>(main));
  }

  std::shared_ptr<Package> read_package(const std::string &file_name,
                                        json &data);
  Controler(std::vector<std::string> json_repositories_names,
            std::string storage_file_name, Package_manager *pm)
      : json_repozitories_names(json_repositories_names),
        storage_file_name(storage_file_name), pm(pm) {
    correct_json(storage_file_name);
    for (const auto &elem : json_repositories_names) {
      correct_json(elem);
    }
    Empty_with_main_read empty;
    Support_read support;
    Main_read main;
    strategies.push_back(std::make_shared<Empty_with_main_read>(empty));
    strategies.push_back(std::make_shared<Support_read>(support));
    strategies.push_back(std::make_shared<Main_read>(main));
    write_package_manager_to_file(storage_file_name, *pm);
  }

  const std::vector<std::string> &get_json_repozitories_names() const noexcept {
    return json_repozitories_names;
  }
  const std::string &get_storage_file_name() const noexcept {
    return storage_file_name;
  }
  const std::vector<std::shared_ptr<Read_strategy>> &
  get_strategies() const noexcept {
    return strategies;
  }

  void set_json_repozitories_names(
      const std::vector<std::string> &new_repozitories) {
    json_repozitories_names = new_repozitories;
  }
  void set_storage_file_name(const std::string &new_storage_file_name) {
    storage_file_name = new_storage_file_name;
  }
  void set_strategies(
      const std::vector<std::shared_ptr<Read_strategy>> new_strategies) {
    strategies = new_strategies;
  }

  std::shared_ptr<Package>
  read_package_from_file(const std::string &file_name,
                         const std::string &input_file_name);
  void read_package_manager_from_file(const std::string &input_file_name,
                                      Package_manager &pm);
  void write_package_to_file(const std::shared_ptr<Package> &package,
                             const std::string &output_file_name);
  void write_package_manager_to_file(const std::string &output_file_name,
                                     Package_manager &pm);

  std::shared_ptr<Package> read_package(json &data, json *req_packages);

  static std::ostream &write_package(const std::shared_ptr<Package> &package,
                                     std::ostream &out);

  json find_package(std::istream &in, const std::string &file_name);
  json find_package(json &data, const std::string &file_name);
  json find_package(const std::string &filename, const std::string &file_name);

  void add_package(const std::string &file_name);
  void remove_package(const std::string &file_name);
  bool find_package(const std::string &file_name) {
    return pm->find(file_name);
  }
};
#endif