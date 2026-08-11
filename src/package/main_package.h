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
/**
 * @brief Represents a main package of software.
 * This class is used to store metadata about a software appliсation,
 * including its version history, publisher details, operational state
 * and a list of required dependency packages
 *
 */
class Main_package : public Package {
private:
  /**The display name of the sofrware package. */
  std::string package_name;
  /**The name of the executable or archive file. */
  std::string file_name;
  /**The name of the publicher or developer. */
  std::string publisher_name;
  /**The currently instaled version of software. */
  std::string current_version;
  /**The latest available version of software. */
  std::string last_version;
  /**Indicates the installed state of the package. */
  bool condition = false;
  /**Indicates whether this instance is a dependency of any other package.*/
  bool using_flag = false;
  /**A vector of pointers to the dependency packages. */
  std::vector<std::shared_ptr<Package>> req_packages;
  /**
   * @brief Checks whether all fields of the object are valid.
   *
   * @return
   */
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
  /**
   * @brief Constructs a new Main_package object with specified metadata and
   dependences.
   * Initializes all fields and than validates the object invariants.
   *
   * @param package_name
   * @param file_name
   * @param publisher_name
   * @param current_version
   * @param last_version
   * @param req_packages
   */
  Main_package(const std::string &package_name, const std::string &file_name,
               const std::string &publisher_name,
               const std::string &current_version,
               const std::string &last_version,
               const std::vector<std::shared_ptr<Package>> &req_packages)
      : package_name(package_name), file_name(file_name),
        publisher_name(publisher_name), current_version(current_version),
        last_version(last_version), req_packages(req_packages) {
    correct();
    std::sort(this->req_packages.begin(), this->req_packages.end(),
              [](const auto &a, const auto &b) {
                return a->get_file_name() < b->get_file_name();
              });
  }
  /**
   * @brief Construct a new Main_package object with specified metadata and
   dependences.
   *
   * Initializes all fields and than validates the object invariants.
   *
   * @param file_name
   * @param publisher_name
   * @param current_version
   * @param last_version
   * @param req_packages
   */
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
  /**
   * @brief Constructs a new Main_package object (Copy constructor).
   *
   * @param other
   */
  Main_package(const Main_package &other) = default;
  /**
   * @brief Copy operator=.
   *
   * @param other
   * @return Main_package&
   */
  Main_package &operator=(const Main_package &other) = default;
  /**
   * @brief Constructs a new Main_package object (Move constructor).
   *
   * @param other
   */
  Main_package(Main_package &&other) = default;
  /**
   * @brief Move operator=.
   *
   * @param other
   * @return Main_package&
   */
  Main_package &operator=(Main_package &&other) = default;
  /**
   * @brief Constructs a Main_package object with default placeholder values.
   *
   * All fields are initialized to syntactically valid but semantically empty
   * defaults. This constructor is intended for creating an object that will be
   * populated later.
   */
  Main_package()
      : file_name("default.dep"), publisher_name("default"),
        current_version("default"), last_version("default") {}
  /**
   * @brief Destroys the Main_package object
   *
   */
  ~Main_package() override = default;
  /**
   * @brief Marks this instance as installed.
   *
   */
  void add() override;
  /**
   * @brief Marks this instance as uninstalled.
   *
   */
  void remove() override;
  /**
   * @brief Gets the file name object.
   *
   * @return std::string
   */
  std::string get_file_name() const noexcept override { return file_name; }
  /**
   * @brief Gets the publisher name object.
   *
   * @return std::string
   */
  std::string get_publisher_name() const noexcept override {
    return publisher_name;
  }
  /**
   * @brief Gets the condition object.
   *
   * @return true
   * @return false
   */
  bool get_condition() const noexcept override { return condition; };
  /**
   * @brief Gets the current version object.
   *
   * @return std::string
   */
  std::string get_current_version() const noexcept override {
    return current_version;
  };
  /**
   * @brief Gets the last version object.
   *
   * @return std::string
   */
  std::string get_last_version() const noexcept override {
    return last_version;
  };
  /**
   * @brief Gets the package name object.
   *
   * @return std::string
   */
  std::string get_package_name() const noexcept override {
    return package_name;
  }
  /**
   * @brief Sets the package name object.
   *
   * @param new_prog_name
   */
  void set_package_name(const std::string &new_prog_name) override {
    if (new_prog_name == "") {
      throw std::runtime_error("bad new prog name");
    }
    package_name = new_prog_name;
  }
  /**
   * @brief Sets the file name object.
   *
   * @param new_file_name
   */
  void set_file_name(const std::string &new_file_name) override {
    if (new_file_name.length() < 5) {
      throw std::invalid_argument("invalid file_name");
    }
    if (new_file_name.substr(new_file_name.length() - 4) != ".dep") {
      throw std::invalid_argument("invalid file_name");
    }
    file_name = new_file_name;
  };
  /**
   * @brief Sets the publisher name object.
   *
   * @param new_publisher_name
   */
  void set_publisher_name(const std::string &new_publisher_name) override {
    if (new_publisher_name == "") {
      throw std::invalid_argument("invalid publisher_name");
    }
    publisher_name = new_publisher_name;
  };
  /**
   * @brief Sets the current version object.
   *
   * @param new_version
   */
  void set_current_version(const std::string &new_version) override {
    if (new_version == "") {
      throw std::invalid_argument("invalid version");
    }
    current_version = new_version;
  }
  /**
   * @brief Sets the last version object.
   *
   * @param new_version
   */
  void set_last_version(const std::string &new_version) override {
    if (new_version == "") {
      throw std::invalid_argument("invalid version");
    }
    last_version = new_version;
  }

  /**
   * @brief Gets the connected packages object
   *
   * @return const std::vector<std::shared_ptr<Package>>&
   */
  const std::vector<std::shared_ptr<Package>> &
  get_connected_packages() const override {
    return req_packages;
  }
  /**
   * @brief Adds a new dependency package to the list of required packages
   *
   * Checks whether the package is already present in the list by comparing
   * both pointers and values. If the package is not found, it is inserted
   * and the list is sorted alphabetically by file name.
   * @param package
   * @return true
   * @return false
   */
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
  /**
   * @brief Removes an existing dependency from the list of required packages.
   *
   * @param package
   * @return true
   * @return false
   */
  bool erase_connected(const Package &package) override {
    auto it = std::find_if(req_packages.begin(), req_packages.end(),
                           [&package](auto x) { return package == (*x); });
    if (it == req_packages.end()) {
      return false;
    }
    req_packages.erase(it);
    return true;
  }
  /**
   * @brief Overloaded operator== .
   *
   * @param o
   * @return true
   * @return false
   */
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
  /**
   * @brief Gets the using flag object.
   *
   * @return true
   * @return false
   */
  bool get_using_flag() const noexcept override { return using_flag; }
  /**
   * @brief Sets the using flag object.
   *
   * @param new_using_flag
   */
  void set_using_flag(bool new_using_flag) override {
    using_flag = new_using_flag;
  }
  /**
   * @brief Writes the package metadata to an output stream.
   *
   * @param out
   * @return std::ostream&
   */
  std::ostream &write(std::ostream &out) override;
  /**
   * @brief Creates a deep copy of this package.
   *
   * @return std::shared_ptr<Package>
   */
  std::shared_ptr<Package> clone() const override {
    std::vector<std::shared_ptr<Package>> copy_req_packages;
    for (auto &elem : req_packages) {
      copy_req_packages.push_back(elem->clone());
    }
    Main_package res(package_name, file_name, publisher_name, current_version,
                     last_version, copy_req_packages);
    return std::make_shared<Main_package>(res);
  }
  /**
   * @brief Serializes the package metadata to a JSON object.
   *
   * @return json
   */
  json write_to_json() const override;
};

#endif