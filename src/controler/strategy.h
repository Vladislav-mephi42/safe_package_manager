
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
                                json *req_packages_names) const override {}
};

class Support_read : public Read_strategy {
public:
  bool can_read(const std::string &type_name) const override {
    return type_name == "support";
  }

  std::shared_ptr<Package> read(json data,
                                json *req_packages_names) const override {}
};

class Empty_read : public Read_strategy {
public:
  bool can_read(const std::string &type_name) const override {
    return type_name == "empty";
  }

  std::shared_ptr<Package> read(json data,
                                json *req_packages_names) const override

  {}
};

#endif
