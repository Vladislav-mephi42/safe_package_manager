#ifndef PACKAGE_H
#define PACKAGE_H

#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

class Read_strategy;

class Package {

public:
  virtual ~Package() = 0;
  virtual void add() = 0;
  virtual void remove() = 0;
  virtual bool get_condition() const noexcept = 0;
  virtual std::string get_current_version() const noexcept = 0;
  virtual std::string get_last_version() const noexcept = 0;
  virtual std::string get_publisher_name() const noexcept = 0;
  virtual std::string get_file_name() const noexcept = 0;
  virtual std::string get_package_name() const noexcept = 0;
  virtual void set_package_name(const std::string &new_package_name) = 0;
  virtual void set_file_name(const std::string &new_file_name) = 0;
  virtual void set_publisher_name(const std::string &new_publisher_name) = 0;
  virtual void set_current_version(const std::string &new_version) = 0;
  virtual void set_last_version(const std::string &new_version) = 0;
  virtual const std::vector<std::shared_ptr<Package>> &
  get_connected_packages() const = 0;
  virtual bool insert_connected(const std::shared_ptr<Package> &package) = 0;
  virtual bool erase_connected(const Package &package) = 0;
  virtual bool operator==(const Package &) const = 0;
  virtual bool get_using_flag() const noexcept = 0;
  virtual void set_using_flag(bool new_using_flag) = 0;
  virtual std::ostream &write(std::ostream &out) = 0;
  virtual json write_to_json() const = 0;
  virtual std::shared_ptr<Package> clone() const = 0;
};

#endif