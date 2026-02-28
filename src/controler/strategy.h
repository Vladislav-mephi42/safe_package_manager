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

  virtual std::shared_ptr<Package>
  read(std::istream &in,
       std::vector<std::shared_ptr<Read_strategy>> &strategies) const = 0;
};

class Main_read : public Read_strategy {
public:
  bool can_read(const std::string &type_name) const override {
    return type_name == "main";
  }

  std::shared_ptr<Package>
  read(std::istream &in,
       std::vector<std::shared_ptr<Read_strategy>> &strategies) const override {
    Main_package package;
    std::shared_ptr<Package> package_ptr =
        std::make_shared<Main_package>(package);
    if (package_ptr->read(in, strategies)) {
      return package_ptr;
    }
    throw std::runtime_error("invalid input");
  }
};

class Support_read : public Read_strategy {
public:
  bool can_read(const std::string &type_name) const override {
    return type_name == "support";
  }

  std::shared_ptr<Package>
  read(std::istream &in,
       std::vector<std::shared_ptr<Read_strategy>> &strategies) const override {
    Support_package package;
    std::shared_ptr<Package> package_ptr =
        std::make_shared<Support_package>(package);
    if (package_ptr->read(in, strategies)) {
      return package_ptr;
    }
    throw std::runtime_error("invalid input");
  }
};

class Empty_read : public Read_strategy {
public:
  bool can_read(const std::string &type_name) const override {
    return type_name == "empty";
  }

  std::shared_ptr<Package>
  read(std::istream &in,
       std::vector<std::shared_ptr<Read_strategy>> &strategies) const override

  {
    Empty_package package;
    std::shared_ptr<Package> package_ptr =
        std::make_shared<Empty_package>(package);

    if (package_ptr->read(in, strategies)) {
      return package_ptr;
    }
    throw std::runtime_error("invalid input");
  }
};

#endif