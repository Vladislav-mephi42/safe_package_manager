
#ifndef STRATEGY_H
#define STRATEGY_H

#include "package/empty_package.h"
#include "package/main_package.h"
#include "package/package.h"
#include "package/support_package.h"
#include <memory>
#include <string>
/**
 * @brief Abstract class that represents a deserialization strategy.
 *
 * The main purpose of this class is to determine whether a given class type
 * can be deserialized by this strategy and, if so, to construct the
 * corresponding object from JSON data.
 *
 */
class Deserialization_strategy {
public:
  /**
   * @brief Checks whether a software package with the specified type name can
   *        be deserialized by this strategy.
   *
   *
   * @param type_name
   * @return true
   * @return false
   */
  virtual bool can_read(const std::string &type_name) const = 0;
  /**
   * @brief Creates a shared pointer to the deserialized package.
   *
   * Reads the JSON data and constructs a concrete Package object. The names
   * of any required dependency packages are extracted and stored in the
   * provided output parameter.
   *
   * @param data
   * @param req_packages_names
   * @return std::shared_ptr<Package>
   */
  virtual std::shared_ptr<Package> read(json data,
                                        json *req_packages_names) const = 0;
};
/**
 * @brief Deserialization strategy for main software packages.
 * The main purpose of this class is to determine whether a given class type
 * can be deserialized by this strategy and, if so, to construct the
 * corresponding object from JSON data.
 *
 */
class Main_read : public Deserialization_strategy {
public:
  /**
   * @brief Checks whether a software package with the specified type name can
   *    be deserialized by this strategy.
   *
   * @param type_name
   * @return true
   * @return false
   */
  bool can_read(const std::string &type_name) const override {
    return type_name == "main";
  }
  /**
   *@brief Creates a shared pointer to the deserialized package.
   *
   * Reads the JSON data and constructs a concrete Package object. The names
   * of any required dependency packages are extracted and stored in the
   * provided output parameter.
   *
   * @param data
   * @param req_packages_names
   * @return std::shared_ptr<Package>
   */
  std::shared_ptr<Package> read(json data,
                                json *req_packages_names) const override {
    bool check = data.contains("file_name") && data.contains("package_name") &&
                 data.contains("publisher_name") &&
                 data.contains("using_flag") &&
                 data.contains("current_version") &&
                 data.contains("last_version") && data.contains("req_packages");
    if (!check) {
      throw std::runtime_error("deserealization error");
    }
    Main_package tmp;
    std::shared_ptr<Package> package = std::make_shared<Main_package>(tmp);
    package->set_file_name(data["file_name"]);
    package->set_package_name(data["package_name"]);
    package->set_publisher_name(data["publisher_name"]);
    package->set_using_flag(data["using_flag"]);
    package->set_current_version(data["current_version"]);
    package->set_last_version(data["last_version"]);
    *req_packages_names = data["req_packages"];
    return package;
  }
};
/**
 * @brief Deserialization strategy for support software packages.
 * The main purpose of this class is to determine whether a given class type
 * can be deserialized by this strategy and, if so, to construct the
 * corresponding object from JSON data.
 *
 */
class Support_read : public Deserialization_strategy {
public:
  /**
   * @brief Checks whether a software package with the specified type name can
   *    be deserialized by this strategy.
   *
   * @param type_name
   * @return true
   * @return false
   */
  bool can_read(const std::string &type_name) const override {
    return type_name == "support";
  }
  /**
   * @brief Creates a shared pointer to the deserialized package.
   *
   * Reads the JSON data and constructs a concrete Package object. The names
   * of any required dependency packages are extracted and stored in the
   * provided output parameter.
   *
   * @param data
   * @param req_packages_names
   * @return std::shared_ptr<Package>
   */
  std::shared_ptr<Package> read(json data,
                                json *req_packages_names) const override {
    bool check = data.contains("file_name") && data.contains("package_name") &&
                 data.contains("publisher_name") &&
                 data.contains("using_flag") &&
                 data.contains("current_version") &&
                 data.contains("last_version") && data.contains("req_packages");
    if (!check) {
      throw std::runtime_error("deserealization error");
    }
    Support_package tmp;
    std::shared_ptr<Package> package = std::make_shared<Support_package>(tmp);
    package->set_package_name(data["package_name"]);
    package->set_file_name(data["file_name"]);
    package->set_publisher_name(data["publisher_name"]);
    package->set_using_flag(data["using_flag"]);
    package->set_current_version(data["current_version"]);
    package->set_last_version(data["last_version"]);
    *req_packages_names = data["req_packages"];
    return package;
  }
};
/**
 * @brief Deserialization strategy for empty software packages.
 * The main purpose of this class is to determine whether a given class type
 * can be deserialized by this strategy and, if so, to construct the
 * corresponding object from JSON data.
 *
 */
class Empty_with_main_read : public Deserialization_strategy {
public:
  /**
   * @brief Checks whether a software package with the specified type name can
   *    be deserialized by this strategy.
   *
   * @param type_name
   * @return true
   * @return false
   */
  bool can_read(const std::string &type_name) const override {
    return type_name == "empty";
  }
  /**
   * @brief Creates a shared pointer to the deserialized package.
   *
   * Reads the JSON data and constructs a concrete Package object. The names
   * of any required dependency packages are extracted and stored in the
   * provided output parameter.
   *
   * @param data
   * @param req_packages_names
   * @return std::shared_ptr<Package>
   */
  std::shared_ptr<Package> read(json data,
                                json *req_packages_names) const override

  {
    bool check = data.contains("file_name") && data.contains("package_name") &&
                 data.contains("publisher_name") &&
                 data.contains("using_flag") &&
                 data.contains("current_version") &&
                 data.contains("last_version") && data.contains("req_packages");
    if (!check) {
      throw std::runtime_error("deserealization error");
    }
    if (data["linked_type"] != "main") {
      throw std::runtime_error("deserealization error");
    }
    Main_package tmp;
    std::shared_ptr<Package> package = std::make_shared<Main_package>(tmp);
    package->set_file_name(data["file_name"]);
    package->set_package_name(data["linked_package_name"]);
    package->set_publisher_name(data["publisher_name"]);
    package->set_using_flag(data["using_flag"]);
    package->set_current_version(data["current_version"]);
    package->set_last_version(data["last_version"]);
    *req_packages_names = data["req_packages"];
    Empty_package empty_package(data["package_name"], package);
    return std::make_shared<Empty_package>(empty_package);
  }
};
/**
 * @brief Represents a strategy that allows full deserialization of a software
 *        package.
 *
 * Provides an interface for reading a package from JSON data, dispatching
 * to registered Deserialization_strategy instances based on the package type.
 */
class Read_strategy {

public:
  /**
   * @brief Checks whether a given type name is valid for this strategy.
   *
   * @param type_name
   * @return true
   * @return false
   */
  virtual bool can_read(const std::string &type_name) const = 0;
  /**
   * @brief  Deserializes a package from JSON data.
   *
   * @param file_name
   * @param data
   * @return std::shared_ptr<Package>
   */
  virtual std::shared_ptr<Package> read_package(const std::string &file_name,
                                                json &data) const = 0;
  /**
   * @brief Adds a new deserealization strategy to the list registered
   * deserialization strategies.
   *
   * @param strategy
   */
  virtual void push_des_strategy(
      const std::shared_ptr<Deserialization_strategy> strategy) = 0;
  /**
   * @brief  Adds  new deserealization strategies to the list registered
   * deserialization strategies.
   *
   * @param strategies
   */
  virtual void push_des_strategies(
      const std::vector<std::shared_ptr<Deserialization_strategy>>
          strategies) = 0;
  /**
   * @brief Destroys the Read_strategy object.
   *
   */
  virtual ~Read_strategy() = default;
};
/**
 * @brief Represents a strategy that allows full deserialization of a software
 *        package.
 *
 * Provides an interface for reading a package from JSON data, dispatching
 * to registered Deserialization_strategy instances based on the package type.
 */
class Default_read : public Read_strategy {
private:
  std::vector<std::shared_ptr<Deserialization_strategy>> des_strategies;

  std::shared_ptr<Package> read_package(json &data, json *req_packages) const;
  std::shared_ptr<Package>
  read_package(const std::string &file_name, json &data,
               std::vector<std::string> &added_packages) const;

public:
  /**
   * @brief Constructs a new Default_read object.
   *
   */
  Default_read() = default;
  /**
   * @brief Copy constructor.
   *
   * @param other
   */
  Default_read(const Default_read &other) = default;
  /**
   * @brief Copy operator = .
   *
   * @param other
   * @return Default_read&
   */
  Default_read &operator=(const Default_read &other) = default;
  /**
   * @brief Move constructor.
   *
   * @param other
   */
  Default_read(Default_read &&other) = default;
  /**
   * @brief Move operator =.
   *
   * @param other
   * @return Default_read&
   */
  Default_read &operator=(Default_read &&other) = default;
  /**
   * @brief Destroys the Default_read object.
   *
   */
  ~Default_read() override = default;
  /**
   * @brief Checks whether a given type name is valid for this strategy.
   *
   * @param type_name
   * @return true
   * @return false
   */
  bool can_read(const std::string &type_name) const override;
  /**
   * @brief  Deserializes a package from JSON data.
   *
   * @param file_name
   * @param data
   * @return std::shared_ptr<Package>
   */
  std::shared_ptr<Package> read_package(const std::string &file_name,
                                        json &data) const override;
  /**
   * @brief Adds a new deserealization strategy to the list registered
   * deserialization strategies.
   *
   * @param strategy
   */
  void push_des_strategy(
      const std::shared_ptr<Deserialization_strategy> strategy) override {
    des_strategies.push_back(strategy);
  }
  /**
   * @brief Adds  new deserealization strategies to the list registered
   * deserialization strategies.
   *
   * @param strategies
   */
  void push_des_strategies(
      const std::vector<std::shared_ptr<Deserialization_strategy>> strategies)
      override {
    for (const auto &elem : strategies) {
      des_strategies.push_back(elem);
    }
  }
};
/**
 * @brief Represents a strategy that allows full deserialization of a software
 *        package.
 *
 * Provides an interface for reading a package from JSON data, dispatching
 * to registered Deserialization_strategy instances based on the package type.
 */
class Empty_read : public Read_strategy {
private:
  std::vector<std::shared_ptr<Deserialization_strategy>> des_strategies;

  std::shared_ptr<Package> read_package(json &data, json *req_packages) const;
  std::shared_ptr<Package>
  read_package(const std::string &file_name, json &data,
               std::vector<std::string> &added_packages) const;
  std::shared_ptr<Package>
  read_package_using_file_name(const std::string &file_name, json &data) const;

public:
  /**
   * @brief Constructs a new Empty_read object.
   *
   */
  Empty_read() = default;
  /**
   * @brief Copy constructor.
   *
   * @param other
   */
  Empty_read(const Empty_read &other) = default;
  /**
   * @brief Copy operator =.
   *
   * @param other
   * @return Empty_read&
   */
  Empty_read &operator=(const Empty_read &other) = default;
  /**
   * @brief Move constructor.
   *
   * @param other
   */
  Empty_read(Empty_read &&other) = default;
  /**
   * @brief Move operato =.
   *
   * @param other
   * @return Empty_read&
   */
  Empty_read &operator=(Empty_read &&other) = default;
  /**
   * @brief Destroys the Empty_read object.
   *
   */
  ~Empty_read() override = default;

  /**
   * @brief Checks whether a given type name is valid for this strategy.
   *
   * @param type_name
   * @return true
   * @return false
   */
  bool can_read(const std::string &type_name) const override;
  /**
   * @brief  Deserializes a package from JSON data.
   *
   * @param file_name
   * @param data
   * @return std::shared_ptr<Package>
   */
  std::shared_ptr<Package> read_package(const std::string &file_name,
                                        json &data) const override;
  /**
   * @brief Adds a new deserealization strategy to the list registered
   * deserialization strategies.
   *
   * @param strategy
   */
  void push_des_strategy(
      const std::shared_ptr<Deserialization_strategy> strategy) override {
    des_strategies.push_back(strategy);
  }
  /**
   * @brief Adds  new deserealization strategies to the list registered
   * deserialization strategies.
   *
   * @param strategies
   */
  void push_des_strategies(
      const std::vector<std::shared_ptr<Deserialization_strategy>> strategies)
      override {
    for (const auto &elem : strategies) {
      des_strategies.push_back(elem);
    }
  }
};

namespace P_IOF {
/**
 * @brief Finds package.
 *
 * @param in
 * @param file_name
 * @return json
 */
json find_package(std::istream &in, const std::string &file_name);
/**
 * @brief Finds package.
 *
 * @param data
 * @param file_name
 * @return json
 */
json find_package(json &data, const std::string &file_name);
/**
 * @brief Finds package.
 *
 * @param filename
 * @param file_name
 * @return json
 */
json find_package(const std::string &filename, const std::string &file_name);

/**
 * @brief Write package to the output stream.
 *
 * @param package
 * @param out
 * @return std::ostream&
 */
std::ostream &write_package(const std::shared_ptr<Package> &package,
                            std::ostream &out);

void write_package_to_json(const std::shared_ptr<Package> &package, json &data);
/**
 * @brief Serealizes the input package object into json string, than opens
 * file with given output file name and writes serialized package
 * into file.
 *
 * @param package
 * @param output_file_name
 */
void write_package_to_file(const std::shared_ptr<Package> &package,
                           const std::string &output_file_name);

}; // namespace P_IOF
#endif
