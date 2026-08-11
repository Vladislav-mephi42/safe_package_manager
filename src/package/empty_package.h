#ifndef EMPTY_PACKAGE
#define EMPTY_PACKAGE

#include "package/main_package.h"
#include "package/package.h"
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <vector>

// package_name will be ......-latest
// it's tells controler that you want find and install -latest version of
// package-name without latest package, if it will find it
using json = nlohmann::json;
/**
 * @brief Represents the latest version package of software.
 * This class called empty package because this package is a wrapper on other
 * package
 *
 */
class Empty_package : public Package {
private:
  /**Indicates the installed state of the package. */

  bool condition = false;
  /** The display package name */
  std::string package_name = "default-last";
  /**The package with the latest available version */
  std::shared_ptr<Package> linked_package;

public:
  /** Marks that the software package is installed */
  void add() override;
  /** Marks that the software package is uninstalled */
  void remove() override;
  /**
   * @brief Constructs a new Empty_package.
   *
   * @param package_name
   * @param linked_package
   */
  Empty_package(const std::string &package_name,
                const std::shared_ptr<Package> &linked_package)
      : package_name(package_name), linked_package(linked_package) {}
  /**
   * @brief Constructs a new Empty_package object with null linked package and
   * default name.
   *
   */
  Empty_package() : package_name("default-last") {}
  /**
   * @brief Constructs a new Empty_package object (Copy constructor).
   *
   */
  Empty_package(const Empty_package &) = default;
  /**
   * @brief Copy operator = .
   *
   * @return Empty_package&
   */
  Empty_package &operator=(const Empty_package &) = default;
  /**
   * @brief Constructs a new Empty_package object (Mpve constructor).
   *
   */
  Empty_package(Empty_package &&) = default;
  /**
   * @brief Move operator = .
   *
   * @return Empty_package&
   */
  Empty_package &operator=(Empty_package &&) = default;
  /**
   * @brief Destroy the Empty_package object
   *
   */
  ~Empty_package() override = default;
  /**
   * @brief Gets the file name of the package.
   *
   *
   * The file name is the name of the executable or archive file. This method
   * delegates the call to the linked package if one is set. If no package
   * is linked, an empty string is returned.
   * @return std::string
   */
  std::string get_file_name() const noexcept override {
    if (!linked_package) {
      std::string empty;
      return empty;
    }
    return linked_package->get_file_name();
  }
  /**
   * @brief Get the publisher name.
   *
   * The publisher name is an attribute of the linked package. This method
   * delegates the call to the linked package if one is set. If no package is
   * linked, an empty string is returned.
   *
   * @return std::string
   */
  std::string get_publisher_name() const noexcept override {
    if (!linked_package) {
      std::string empty;
      return empty;
    }
    return linked_package->get_publisher_name();
  }
  /**
   * @brief Gets the condition.
   *
   * @return true
   * @return false
   */
  bool get_condition() const noexcept override { return condition; };
  /**
   * @brief Gets the current version.
   * The current version is an attribute of the linked package. This method
   * delegates the call to the linked package if one is set. If no package is
   * linked, an empty string is returned.
   *
   * @return std::string
   */
  std::string get_current_version() const noexcept override {
    if (!linked_package) {
      std::string empty;
      return empty;
    }
    return linked_package->get_current_version();
  };
  /**
   * @brief Gets the last version.
   * The last version is an attribute of the linked package. This method
   * delegates the call to the linked package if one is set. If no package is
   * linked, an empty string is returned.
   *
   * @return std::string
   */
  std::string get_last_version() const noexcept override {
    if (!linked_package) {
      std::string empty;
      return empty;
    }
    return linked_package->get_last_version();
  };
  /**
   * @brief Gets the pakage name.
   * The package name is an attribute of the linked package. This method
   * delegates the call to the linked package if one is set. If no package is
   * linked, an empty string is returned.
   *
   * @return std::string
   */
  std::string get_package_name() const noexcept override {
    return package_name;
  }
  /**
   * @brief Sets the package name.
   *
   * @param new_package_name
   */
  void set_package_name(const std::string &new_package_name) override {
    if (new_package_name == "") {
      throw std::runtime_error("bad new prog name");
    }
    package_name = new_package_name;
  }
  /**
   * @brief Validates and sets the file name.
   * The file name is an attribute of the linked package. The method delegates
   * the call to the linked package if one is set. The linked package must be
   * set before calling this method.
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

    if (!linked_package) {
      throw std::invalid_argument("invalid linked package");
    }
    linked_package->set_file_name(new_file_name);
  };
  /**
   * @brief Validates and sets the publisher name.
   * The publisher name is an attribute of the linked package. The method
   * delegates the call to the linked package if one is set. The linked package
   * must be set before calling this method.
   *
   * @param new_publisher_name
   */
  void set_publisher_name(const std::string &new_publisher_name) override {
    if (!linked_package) {
      throw std::runtime_error("No linked package");
    }
    if (new_publisher_name == "") {
      throw std::invalid_argument("invalid publisher_name");
    }

    linked_package->set_publisher_name(new_publisher_name);
  };
  /**
   * @brief Validates and sets the last version.
   * The last version is an attribute of the linked package. The method
   * delegates the call to the linked package if one is set. The linked package
   * must be set before calling this method.
   *
   *
   * @param new_version
   */
  void set_last_version(const std::string &new_version) override {
    if (!linked_package) {
      throw std::runtime_error("No linked package");
    }
    linked_package->set_last_version(new_version);
  }
  /**
   * @brief Validates and sets the current version.
   * The current version is an attribute of the linked package. The method
   * delegates the call to the linked package if one is set. The linked package
   * must be set before calling this method.
   *
   *
   * @param new_version
   */
  void set_current_version(const std::string &new_version) override {
    if (!linked_package) {
      throw std::runtime_error("No linked package");
    }
    linked_package->set_current_version(new_version);
  }
  /**
   * @brief Gets the connected packages.
   * The connected packages is an attribute of the linked package. This method
   * delegates the call to the linked package if one is set. If no package is
   * linked, an empty vector is returned.
   *
   *
   * @return const std::vector<std::shared_ptr<Package>>&
   */
  const std::vector<std::shared_ptr<Package>> &
  get_connected_packages() const override {
    if (!linked_package) {
      throw std::runtime_error("No linked package");
    }
    return linked_package->get_connected_packages();
  }
  /**
   * @brief Adds new dependency to the required packages.
   * The reauired packages is an attribute of the linked package. This method
   * delegates the call to the linked package if one is set.
   *
   * @param package
   * @return true
   * @return false
   */
  bool insert_connected(const std::shared_ptr<Package> &package) override {
    if (!linked_package) {
      throw std::runtime_error("No linked package");
    }
    return linked_package->insert_connected(package);
  }
  /**
   * @brief Remove dependency from the required packages.
   * The reauired packages is an attribute of the linked package. This method
   * delegates the call to the linked package if one is set.
   *
   * @param package
   * @return true
   * @return false
   */
  bool erase_connected(const Package &package) override {
    if (!linked_package) {
      throw std::runtime_error("No linked package");
    }
    return linked_package->erase_connected(package);
  }
  /**
   * @brief operator == .
   *
   * @param o
   * @return true
   * @return false
   */
  bool operator==(const Package &o) const override {
    if (dynamic_cast<const Empty_package *>(&o) == nullptr) {
      return false;
    }
    auto this_t = std::make_tuple(get_package_name(), get_current_version(),
                                  get_last_version(), get_publisher_name(),
                                  get_file_name());
    auto other_t = std::make_tuple(
        o.get_package_name(), o.get_current_version(), o.get_last_version(),
        o.get_publisher_name(), o.get_file_name());

    for (int i = 0; i < this->get_connected_packages().size(); i++) {
      if (*(this->get_connected_packages()[i].get()) !=
          *(((o.get_connected_packages())[i]).get())) {
        return false;
      }
    }

    return this_t == other_t;
  }
  /**
   * @brief Gets the using flag.
   * The using flag is an attribute of the linked package. This method
   * delegates the call to the linked package if one is set. If no package is
   * linked, an empty string is returned.
   *
   * @return true
   * @return false
   */
  bool get_using_flag() const noexcept override {
    if (!linked_package) {
      return false;
    }
    return linked_package->get_using_flag();
  }
  /**
   * @brief Sets the using flag.
   * The using flag is an attribute of the linked package. The method delegates
   * the call to the linked package if one is set. The linked package must be
   * set before calling this method.
   *
   *
   * @param new_using_flag
   */
  void set_using_flag(bool new_using_flag) override {
    if (!linked_package) {
      throw std::runtime_error("No linked package");
    }
    linked_package->set_using_flag(new_using_flag);
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
    auto new_linked_package = linked_package->clone();
    Empty_package res(package_name, new_linked_package);
    return std::make_shared<Empty_package>(res);
  }
  /**
   * @brief Serializes the package metadata to a JSON object.
   *
   * @return json
   */
  json write_to_json() const override;
};

#endif