#ifndef MAIN_PACKAGE
#define MAIN_PACKAGE

#include "package/package.h"
#include <algorithm>
#include <memory>

#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <vector>
using json = nlohmann::json;

class Main_package : public Package {
private:
  std::string package_name;
  std::string file_name;
  std::string publisher_name;
  std::string current_version;
  std::string last_version;
  bool condition = false;
  bool using_flag = false;
  std::vector<std::shared_ptr<Package>> req_packages;
  void correct() {
    if (publisher_name == "" || current_version == "" || last_version == "") {
      throw std::invalid_argument("Empty field");
    }
    if (file_name.length() < 5) {
      throw std::invalid_argument("invalid file_name");
    }
    if (file_name.substr(file_name.length() - 4) != ".dep") {
      throw std::invalid_argument("invalid file_name");
    }
  }

public:
  Main_package(const std::string &package_name, const std::string &file_name,
               const std::string &publisher_name,
               const std::string &current_version,
               const std::string &last_version,
               const std::vector<std::shared_ptr<Package>> &req_packages)
      : file_name(file_name), publisher_name(publisher_name),
        current_version(current_version), last_version(last_version),
        req_packages(req_packages) {
    correct();
    std::sort(this->req_packages.begin(), this->req_packages.end(),
              [](const auto &a, const auto &b) {
                return a->get_file_name() < b->get_file_name();
              });
  }
  Main_package(const std::string &file_name, const std::string &publisher_name,
               const std::string &current_version,
               const std::string &last_version,
               const std::vector<std::shared_ptr<Package>> &req_packages)
      : file_name(file_name), publisher_name(publisher_name),
        current_version(current_version), last_version(last_version),
        req_packages(req_packages) {

    correct();
    package_name = file_name.substr(0, file_name.length() - 4);
    std::sort(this->req_packages.begin(), this->req_packages.end(),
              [](const auto &a, const auto &b) {
                return a->get_file_name() < b->get_file_name();
              });
  }

  Main_package(const Main_package &other)
      : file_name(other.file_name), publisher_name(other.publisher_name),
        current_version(other.current_version),
        last_version(other.last_version), req_packages(other.req_packages) {}
  Main_package &operator=(const Main_package &other) {
    if (this != &other) {
      file_name = other.file_name;
      publisher_name = other.publisher_name;
      current_version = other.current_version;
      last_version = other.last_version;
      req_packages = other.req_packages;
    }
    return *this;
  }
  Main_package()
      : file_name("default.dep"), publisher_name("default"),
        current_version("default"), last_version("default") {}
  ~Main_package() override = default;
  void add() override;
  void remove() override;
  std::string get_file_name() const noexcept override { return file_name; }
  std::string get_publisher_name() const noexcept override {
    return publisher_name;
  }
  bool get_condition() const noexcept override { return condition; };
  std::string get_current_version() const noexcept override {
    return current_version;
  };
  std::string get_last_version() const noexcept override {
    return last_version;
  };
  std::string get_package_name() const noexcept override {
    return package_name;
  }
  void set_package_name(const std::string &new_prog_name) {
    if (new_prog_name == "") {
      throw std::runtime_error("bad new prog name");
    }
    package_name = new_prog_name;
  }

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
    if (new_publisher_name == "") {
      throw std::invalid_argument("invalid publisher_name");
    }
    publisher_name = new_publisher_name;
  };

  void set_current_version(const std::string &new_version) override {
    if (new_version == "") {
      throw std::invalid_argument("invalid version");
    }
    current_version = new_version;
  }

  void set_last_version(const std::string &new_version) override {
    if (new_version == "") {
      throw std::invalid_argument("invalid version");
    }
    last_version = new_version;
  }
  const std::vector<std::shared_ptr<Package>> &
  get_connected_packages() const override {
    return req_packages;
  }

  bool insert_connected(const std::shared_ptr<Package> &package) override {
    if (std::find(req_packages.begin(), req_packages.end(), package) !=
        req_packages.end()) {
      return false;
    }
    if (std::find_if(req_packages.begin(), req_packages.end(),
                     [package](auto x) { return (*x) == (*package); }) !=
        req_packages.end()) {
      return false;
    }
    req_packages.push_back(package);
    std::sort(this->req_packages.begin(), this->req_packages.end(),
              [](const auto &a, const auto &b) {
                return a->get_file_name() < b->get_file_name();
              });
    return true;
  }

  bool erase_connected(const Package &package) override {
    auto it = std::find_if(req_packages.begin(), req_packages.end(),
                           [&package](auto x) { return package == (*x); });
    if (it == req_packages.end()) {
      return false;
    }
    req_packages.erase(it);
    return true;
  }

  bool operator==(const Package &o) const override {
    if (dynamic_cast<const Main_package *>(&o) == nullptr) {
      return false;
    }
    auto this_t = std::make_tuple(current_version, last_version, publisher_name,
                                  file_name);
    auto other_t =
        std::make_tuple(o.get_current_version(), o.get_last_version(),
                        o.get_publisher_name(), o.get_file_name());
    if (this_t != other_t ||
        req_packages.size() != (o.get_connected_packages()).size()) {
      return false;
    }
    for (int i = 0; i < req_packages.size(); i++) {
      if (*(req_packages[i].get()) !=
          *(((o.get_connected_packages())[i]).get())) {
        return false;
      }
    }
    return true;
  }

  bool get_using_flag() const noexcept override { return using_flag; }

  void set_using_flag(bool new_using_flag) override {
    using_flag = new_using_flag;
  }

  std::ostream &write(std::ostream &out) override;

  std::shared_ptr<Package> clone() const override {
    return std::make_shared<Main_package>(*this);
  }
  json write_to_json() const override;
};

#endif