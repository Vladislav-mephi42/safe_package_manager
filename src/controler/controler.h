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
  void correct_json(const std::string &file_name);

public:
  /**
   * @brief Constructs a new Controler object with default attributes.
   *
   */
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
  /**
   * @brief Constructs a new Controler object with specified attributes.
   *
   * @param json_repositories_names
   * @param storage_file_name
   * @param pm
   * @param load_from_storage
   */
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
      write_package_manager_to_file(storage_file_name);
    } else {
      read_package_manager_from_file(storage_file_name);
    }
  }
  /**
   * @brief Destroy the Controler object
   *
   */

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
    write_package_manager_to_file(storage_file_name);
  }
  /**
   * @brief Gets the json repozitories names.
   *
   * @return const std::vector<std::string>&
   */
  const std::vector<std::string> &get_json_repozitories_names() const noexcept {
    return json_repozitories_names;
  }
  /**
   * @brief Gets the storage file name.
   *
   * @return const std::string&
   */
  const std::string &get_storage_file_name() const noexcept {
    return storage_file_name;
  }
  /**
   * @brief Gets the strategies.
   *
   * @return const std::vector<std::shared_ptr<Deserialization_strategy>>&
   */
  const std::vector<std::shared_ptr<Deserialization_strategy>> &
  get_strategies() const noexcept {
    return des_strategies;
  }
  /**
   * @brief Sets the json repozitories names.
   *
   * @param new_repozitories
   */
  void set_json_repozitories_names(
      const std::vector<std::string> &new_repozitories) {
    json_repozitories_names = new_repozitories;
  }
  /**
   * @brief Sets the storage file name object.
   *
   * @param new_storage_file_name
   */
  void set_storage_file_name(const std::string &new_storage_file_name) {
    storage_file_name = new_storage_file_name;
  }
  /**
   * @brief Sets the strategies.
   *
   * @param new_strategies
   */
  void
  set_strategies(const std::vector<std::shared_ptr<Deserialization_strategy>>
                     new_strategies) {
    des_strategies = new_strategies;
  }

  /**
   * @brief  Open the input json file and deserialize package manager object
   * from it.
   *
   * @param input_file_name
   */
  void read_package_manager_from_file(const std::string &input_file_name);

  /**
   * @brief Serealizes the input package manager object into json string, than
   * opens file with given output file name and writes serialized package
   * manager into the file.
   *
   * @param output_file_name
   */
  void write_package_manager_to_file(const std::string &output_file_name);
  /**
   * @brief Find package with given file name in json data and deserializes it
   * using special strategies methods.
   *
   * @param file_name
   * @param data
   * @return std::shared_ptr<Package>
   */
  std::shared_ptr<Package> read_package(const std::string &file_name,
                                        json &data) const;

  /**
   * @brief Find the serialized package in controler repozitpries, than read it
   * and add to the package manager
   *
   * @param file_name
   */
  void add_package(const std::string &file_name);
  /**
   * @brief Adds package from external source.
   *
   * @param package
   */
  void add_package_external(const std::shared_ptr<Package> &package);
  /**
   * @brief Removes the package from package manager. After that stores it to
   * the storadge.
   *
   * @param file_name
   */
  void remove_package(const std::string &file_name);
  /**
   * @brief Finds the package in package manager.
   *
   * @param file_name
   * @return true
   * @return false
   */
  bool find_package(const std::string &file_name) {
    return pm->find(file_name);
  }
  std::shared_ptr<Package>
  read_package_from_file(const std::string &file_name,
                         const std::string &input_file_name);
  void remove_unuse();
  void global_update();
};

#endif