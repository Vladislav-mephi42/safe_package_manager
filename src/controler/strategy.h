
#ifndef STRATEGY_H
#define STRATEGY_H

#include "package/empty_package.h"
#include "package/main_package.h"
#include "package/package.h"
#include "package/support_package.h"
#include <memory>
#include <string>

class Read_strategy {
public:
  virtual bool can_read(const std::string &type_name) const = 0;

  virtual std::shared_ptr<Package> read(json data,
                                        json *req_packages_names) const = 0;
};

class Main_read : public Read_strategy {
public:
  bool can_read(const std::string &type_name) const override {
    return type_name == "main";
  }

  std::shared_ptr<Package> read(json data,
                                json *req_packages_names) const override {
    bool check =
        data.contains("file_name") && data.contains("publisher_name") &&
        data.contains("using_flag") && data.contains("current_version") &&
        data.contains("last_version") && data.contains("req_packages");
    if (!check) {
      throw std::runtime_error("deserealization error");
    }
    Main_package tmp;
    std::shared_ptr<Package> package = std::make_shared<Main_package>(tmp);
    package->set_file_name(data["file_name"]);
    package->set_publisher_name(data["publisher_name"]);
    package->set_using_flag(data["using_flag"]);
    package->set_current_version(data["current_version"]);
    package->set_last_version(data["last_version"]);
    *req_packages_names = data["req_packages"];
    return package;
  }
};

class Support_read : public Read_strategy {
public:
  bool can_read(const std::string &type_name) const override {
    return type_name == "support";
  }

  std::shared_ptr<Package> read(json data,
                                json *req_packages_names) const override {
    bool check =
        data.contains("file_name") && data.contains("publisher_name") &&
        data.contains("using_flag") && data.contains("current_version") &&
        data.contains("last_version") && data.contains("req_packages");
    if (!check) {
      throw std::runtime_error("deserealization error");
    }
    Support_package tmp;
    std::shared_ptr<Package> package = std::make_shared<Support_package>(tmp);
    package->set_file_name(data["file_name"]);
    package->set_publisher_name(data["publisher_name"]);
    package->set_using_flag(data["using_flag"]);
    package->set_current_version(data["current_version"]);
    package->set_last_version(data["last_version"]);
    *req_packages_names = data["req_packages"];
    return package;
  }
};

class Empty_with_main_read : public Read_strategy {
public:
  bool can_read(const std::string &type_name) const override {
    return type_name == "empty";
  }

  std::shared_ptr<Package> read(json data,
                                json *req_packages_names) const override

  {
    bool check =
        data.contains("file_name") && data.contains("publisher_name") &&
        data.contains("using_flag") && data.contains("current_version") &&
        data.contains("last_version") && data.contains("req_packages");
    if (!check) {
      throw std::runtime_error("deserealization error");
    }
    Main_package tmp;
    std::shared_ptr<Package> package = std::make_shared<Main_package>(tmp);
    package->set_file_name(data["linked_file_name"]);
    package->set_publisher_name(data["publisher_name"]);
    package->set_using_flag(data["using_flag"]);
    package->set_current_version(data["current_version"]);
    package->set_last_version(data["last_version"]);
    *req_packages_names = data["req_packages"];
    Empty_package empty_package(data["file_name"], package);
    return std::make_shared<Empty_package>(empty_package);
  }
};

#endif
