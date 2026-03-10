
#include "controler/strategy.h"
#include "package/package.h"
#include "package_manager/package_manager.h"
#include <nlohmann/json.hpp>
#include <vector>

// Controler get a package name from view. Than it start analizing embedded
// repositories and check availability of package. If this package is availible,
// controler parse it and do three actions -> 1) add package in json data_base
// 2) add package to Package_manager

//+ method add() of package will execute buils.sh skript, which will install
// this ytilty an add it to path

// View will work with user and get file_name, flags

class Controler {
private:
  std::vector<std::shared_ptr<Read_strategy>> strategies;
  std::vector<std::shared_ptr<Package>> read_packages(std::istream &in);

public:
  Controler() {
    Empty_read empty;
    Support_read support;
    Main_read main;
    json package_base;
    strategies.push_back(std::make_shared<Empty_read>(empty));
    strategies.push_back(std::make_shared<Support_read>(support));
    strategies.push_back(std::make_shared<Main_read>(main));
  }

  std::shared_ptr<Package> read_package(const std::string &file_name,
                                        json &data);
  std::shared_ptr<Package> read_package(json &data, json *req_packages);

  static std::ostream &write_package(const std::shared_ptr<Package> &package,
                                     std::ostream &out);

  json find_package(std::istream &in, const std::string &file_name);
  json find_package(json &data, const std::string &file_name);
  json find_package(const std::string &filename, const std::string &file_name);

#ifdef SKIP
  Package_manager read_manager(std::istream &in);
  static void write_mananger(const Package_manager &manager, std::ostream &out);

  json find_package(std::string path_to_repository, std::string file_name);
  std::shared_ptr<Package> create_package(json package);
  void save_to_database(json package, std::string file_to_data_base);
#endif
};
