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
  std::vector<std::shared_ptr<Deserialization_strategy>> des_strategies;
  std::vector<std::shared_ptr<Read_strategy>> read_strategies;

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
    des_strategies.push_back(std::make_shared<Empty_with_main_read>(empty));
    des_strategies.push_back(std::make_shared<Support_read>(support));
    des_strategies.push_back(std::make_shared<Main_read>(main));
    Default_read default_read;
    default_read.push_des_strategies(des_strategies);
    read_strategies.push_back(std::make_shared<Default_read>(default_read));
    Empty_read empty_read;
    empty_read.push_des_strategies(des_strategies);
    read_strategies.push_back(std::make_shared<Empty_read>(empty_read));
  }
  Controler(std::vector<std::string> json_repositories_names,
            std::string storage_file_name, Package_manager *pm,
            bool load_from_storage = false)
      : json_repozitories_names(json_repositories_names),
        storage_file_name(storage_file_name), pm(pm) {

    for (const auto &elem : json_repositories_names) {
      correct_json(elem);
    }

    Empty_with_main_read empty;
    Support_read support;
    Main_read main;
    des_strategies.push_back(std::make_shared<Empty_with_main_read>(empty));
    des_strategies.push_back(std::make_shared<Support_read>(support));
    des_strategies.push_back(std::make_shared<Main_read>(main));
    Default_read read;
    read.push_des_strategies(des_strategies);
    read_strategies.push_back(std::make_shared<Default_read>(read));

    if (!load_from_storage) {
      write_package_manager_to_file(storage_file_name, *pm);
    } else {
      read_package_manager_from_file(storage_file_name, *pm);
    }
  }
  ~Controler() = default;
  Controler(const Controler &other)
      : des_strategies(other.des_strategies),
        read_strategies(other.read_strategies),
        json_repozitories_names(other.json_repozitories_names),
        storage_file_name(other.storage_file_name), pm(other.pm) {}

  Controler &operator=(const Controler &other) {
    if (this != &other) {
      // Копируем все члены
      des_strategies = other.des_strategies;
      read_strategies = other.read_strategies;
      json_repozitories_names = other.json_repozitories_names;
      storage_file_name = other.storage_file_name;
      pm = other.pm;
    }
    return *this;
  }

  Controler(const std::string &storage_file_name, Package_manager *pm)
      : storage_file_name(storage_file_name), pm(pm) {

    Empty_with_main_read empty;
    Support_read support;
    Main_read main;
    des_strategies.push_back(std::make_shared<Empty_with_main_read>(empty));
    des_strategies.push_back(std::make_shared<Support_read>(support));
    des_strategies.push_back(std::make_shared<Main_read>(main));
    Default_read read;
    read.push_des_strategies(des_strategies);
    read_strategies.push_back(std::make_shared<Default_read>(read));
    write_package_manager_to_file(storage_file_name, *pm);
  }

  const std::vector<std::string> &get_json_repozitories_names() const noexcept {
    return json_repozitories_names;
  }
  const std::string &get_storage_file_name() const noexcept {
    return storage_file_name;
  }
  const std::vector<std::shared_ptr<Deserialization_strategy>> &
  get_strategies() const noexcept {
    return des_strategies;
  }

  void set_json_repozitories_names(
      const std::vector<std::string> &new_repozitories) {
    json_repozitories_names = new_repozitories;
  }
  void set_storage_file_name(const std::string &new_storage_file_name) {
    storage_file_name = new_storage_file_name;
  }
  void
  set_strategies(const std::vector<std::shared_ptr<Deserialization_strategy>>
                     new_strategies) {
    des_strategies = new_strategies;
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

  std::shared_ptr<Package> read_package(const std::string &file_name,
                                        json &data) const;
  std::shared_ptr<Package>
  read_package(const std::string &file_name, json &data,
               std::vector<std::string> &added_packages) const;

  static std::ostream &write_package(const std::shared_ptr<Package> &package,
                                     std::ostream &out);

  json find_package(std::istream &in, const std::string &file_name);
  json find_package(json &data, const std::string &file_name);
  json find_package(const std::string &filename, const std::string &file_name);

  void add_package(const std::string &file_name);
  void add_package_external(const std::shared_ptr<Package> &package);
  void remove_package(const std::string &file_name);
  bool find_package(const std::string &file_name) {
    return pm->find(file_name);
  }
  void write_package_to_json(const std::shared_ptr<Package> &package,
                             json &data);
  void remove_unuse();
  void global_update();
};
#endif