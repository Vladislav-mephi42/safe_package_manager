#ifndef STRATEGY_H
#define STRATEGY_H

#include "package/empty_package.h"
#include "package/main_package.h"
#include "package/package.h"
#include "package/support_package.h"
#include <memory>
#include <string>
/**
 * @brief Абстрактный класс предоставляющий набор методов для десериализации и
 * десериализации класса Package
 *
 */
class Read_strategy {
public:
  /**
   * @brief метод который возращает true если десериализация возможна
   *
   * @param type_name
   * @return true
   * @return false
   */
  virtual bool can_read(const std::string &type_name) const = 0;
  /**
   * @brief метод десериализации
   *
   * @param in
   * @param strategies
   * @return std::shared_ptr<Package>
   */
  virtual std::shared_ptr<Package>
  read(std::istream &in,
       std::vector<std::shared_ptr<Read_strategy>> &strategies) const = 0;
};

/**
 * @brief Наследник класса Read_strategy, предназначенный для Main_package
 *
 */
class Main_read : public Read_strategy {
public:
  /**
   * @brief метод который возращает true если десериализация возможна
   *
   * @param type_name
   * @return true
   * @return false
   */
  bool can_read(const std::string &type_name) const override {
    return type_name == "main";
  }
  /**
   * @brief метод десериализации
   *
   * @param in
   * @param strategies
   * @return std::shared_ptr<Package>
   */
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
/**
 * @brief Наследник класса Read_strategy, предназначенный для Support_package
 *
 */
class Support_read : public Read_strategy {
public:
  /**
   * @brief метод который возращает true если десериализация возможна
   *
   * @param type_name
   * @return true
   * @return false
   */
  bool can_read(const std::string &type_name) const override {
    return type_name == "support";
  }
  /**
   * @brief метод десериализации
   *
   * @param in
   * @param strategies
   * @return std::shared_ptr<Package>
   */
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
/**
 * @brief Наследник класса Read_strategy, предназначенный для Empty_package
 *
 */
class Empty_read : public Read_strategy {
public:
  /**
   * @brief метод который возращает true если десериализация возможна
   *
   * @param type_name
   * @return true
   * @return false
   */
  bool can_read(const std::string &type_name) const override {
    return type_name == "empty";
  }
  /**
   * @brief метод десериализации
   *
   * @param in
   * @param strategies
   * @return std::shared_ptr<Package>
   */
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