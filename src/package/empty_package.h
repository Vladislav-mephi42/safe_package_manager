#ifndef EMPTY_PACKAGE
#define EMPTY_PACKAGE

#include "package/package.h"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

class Empty_package : public Package {
private:
  bool condition = false;

  std::string file_name;

  std::shared_ptr<Package> linked_package;
  void correct() {

    if (file_name.length() < 5) {
      throw std::invalid_argument("invalid file_name ");
    }
    if (file_name.substr(file_name.length() - 4) != ".dep") {
      throw std::invalid_argument("invalid file_name");
    }
  }

public:
  void add() override;

  void remove() override;
  Empty_package(const std::string &file_name,
                const std::shared_ptr<Package> &linked_package)
      : file_name(file_name), linked_package(linked_package) {
    correct();
  }
  Empty_package(const Empty_package &other) noexcept
      : file_name(other.file_name), linked_package(other.linked_package) {}
  Empty_package &operator=(const Empty_package &other) noexcept {
    if (this != &other) {
      file_name = other.file_name;
      linked_package = other.linked_package;
    }
    return *this;
  }
  Empty_package() : file_name("default.dep") {}

  ~Empty_package() override = default;

  std::string get_file_name() const noexcept override { return file_name; }

  std::string get_publisher_name() const noexcept override {
    if (!linked_package) {
      std::string empty;
      return empty;
    }
    return linked_package->get_publisher_name();
  }

  bool get_condition() const noexcept override { return condition; };

  std::string get_current_version() const noexcept override {
    if (!linked_package) {
      std::string empty;
      return empty;
    }
    return linked_package->get_current_version();
  };

  std::string get_last_version() const noexcept override {
    if (!linked_package) {
      std::string empty;
      return empty;
    }
    return linked_package->get_last_version();
  };

  void set_file_name(const std::string &new_file_name) override {
    if (new_file_name.length() < 5) {
      throw std::invalid_argument("invalid file_name");
    }
    if (new_file_name.substr(new_file_name.length() - 4) != ".dep") {
      throw std::invalid_argument("invalid file_name");
    }

    file_name = new_file_name;
  };

  void set_publisher_name(const std::string &new_publisher_name) override {
    if (!linked_package) {
      throw std::runtime_error("No linked package");
    }
    if (new_publisher_name == "") {
      throw std::invalid_argument("invalid publisher_name");
    }

    linked_package->set_publisher_name(new_publisher_name);
  };

  void set_last_version(const std::string &new_version) override {
    if (!linked_package) {
      throw std::runtime_error("No linked package");
    }
    linked_package->set_last_version(new_version);
  }

  void set_current_version(const std::string &new_version) override {
    if (!linked_package) {
      throw std::runtime_error("No linked package");
    }
    linked_package->set_current_version(new_version);
  }

  std::vector<std::shared_ptr<Package>>
  get_connected_packages() const noexcept override {
    if (!linked_package) {
      std::vector<std::shared_ptr<Package>> empty;
      return empty;
    }
    return linked_package->get_connected_packages();
  }

  bool insert_connected(const std::shared_ptr<Package> &package) override {
    if (!linked_package) {
      throw std::runtime_error("No linked package");
    }
    return linked_package->insert_connected(package);
  }

  bool erase_connected(const Package &package) override {
    if (!linked_package) {
      throw std::runtime_error("No linked package");
    }
    return linked_package->erase_connected(package);
  }

  bool operator==(const Package &o) const override {
    if (dynamic_cast<const Empty_package *>(&o) == nullptr) {
      return false;
    }
    auto this_t = std::make_tuple(get_current_version(), get_last_version(),
                                  get_publisher_name(), get_file_name());
    auto other_t =
        std::make_tuple(o.get_current_version(), o.get_last_version(),
                        o.get_publisher_name(), o.get_file_name());
    return this_t == other_t;
  }

  bool get_using_flag() const noexcept override {
    if (!linked_package) {
      return false;
    }
    return linked_package->get_using_flag();
  }

  void set_using_flag(bool new_using_flag) override {
    if (!linked_package) {
      throw std::runtime_error("No linked package");
    }
    linked_package->set_using_flag(new_using_flag);
  }

  std::ostream &write(std::ostream &out) override;
  std::istream &
  read(std::istream &in,
       std::vector<std::shared_ptr<Read_strategy>> &strategies) override;

  std::shared_ptr<Package> clone() const override {
    return std::make_shared<Empty_package>(*this);
  }
};

#endif