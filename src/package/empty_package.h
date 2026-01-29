#ifndef EMPTY_PACKAGE
#define EMPTY_PACKAGE

#include "package/package.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * @brief класс пустого пакета
 *
 */
class Empty_package : public Package {
private:
  /**
   * @brief флаг установки
   *
   */
  bool condition = false;
  /**
   * @brief имя файла
   *
   */
  std::string file_name;
  /**
   * @brief связанный пакет
   *
   */
  std::shared_ptr<Package> linked_package;

public:
  /**
   * @brief подготовка к добавлению
   *
   */
  void add() override;
  /**
   * @brief подготовка к удалению
   *
   */
  void remove() override;
  /**
   * @brief Конструктор
   *
   * @param file_name
   * @param linked_package
   */
  Empty_package(const std::string &file_name,
                std::shared_ptr<Package> linked_package)
      : file_name(file_name), linked_package(linked_package) {}
  /**
   * @brief Конструктор
   *
   */
  Empty_package() = default;
  /**
   * @brief Деструктор
   *
   */
  ~Empty_package() override = default;
  /**
   * @brief геттер имени файла
   *
   * @return std::string
   */
  std::string get_file_name() const noexcept override { return file_name; }
  /**
   * @brief геттер имени издателя
   *
   * @return std::string
   */
  std::string get_publisher_name() const noexcept override {
    return linked_package->get_publisher_name();
  }
  /**
   * @brief геттер флага установки
   *
   * @return true
   * @return false
   */
  bool get_condition() const noexcept override { return condition; };
  /**
   * @brief геттер текущей версии
   *
   * @return std::string
   */
  std::string get_current_version() const noexcept override {
    return linked_package->get_current_version();
  };
  /**
   * @brief геттер текущей версии
   *
   * @return std::string
   */
  std::string get_last_version() const noexcept override {
    return linked_package->get_last_version();
  };
  /**
   * @brief сеттер имени файла
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
   * @brief сеттер имени издателя
   *
   * @param new_publisher_name
   */
  void set_publisher_name(const std::string &new_publisher_name) override {
    if (new_publisher_name == "") {
      throw std::invalid_argument("invalid publisher_name");
    }

    linked_package->set_file_name(new_publisher_name);
  };
  /**
   * @brief сеттер последней версии
   *
   * @param new_version
   */
  void set_last_version(const std::string &new_version) override {
    linked_package->set_last_version(new_version);
  }
  /**
   * @brief сеттер текущей версии
   *
   * @param new_version
   */
  void set_current_version(const std::string &new_version) override {
    linked_package->set_current_version(new_version);
  }
  /**
   * @brief геттер пакетов, используемых данным
   *
   * @return std::vector<std::shared_ptr<Package>>
   */
  std::vector<std::shared_ptr<Package>>
  get_connected_packages() const noexcept override {
    return linked_package->get_connected_packages();
  }
  /**
   * @brief метод добавления нового используемого пакета
   *
   * @param package
   * @return true
   * @return false
   */
  bool insert_connected(std::shared_ptr<Package> package) {
    return linked_package->insert_connected(package);
  }
  /**
   * @brief метод удаления используемого пакета
   *
   * @param package
   * @return true
   * @return false
   */
  bool erase_connected(const Package &package) {
    return linked_package->erase_connected(package);
  }
  /**
   * @brief оператор ==
   *
   * @param o
   * @return true
   * @return false
   */
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
  /**
   * @brief геттер флага "установлен вручную"
   *
   * @return true
   * @return false
   */
  bool get_using_flag() const noexcept override {
    return linked_package->get_using_flag();
  }
  /**
   * @brief сеттер флага "установлен вручную"
   *
   * @param new_using_flag
   */
  void set_using_flag(bool new_using_flag) override {
    linked_package->set_using_flag(new_using_flag);
  }
  /**
   * @brief сериализатор
   *
   * @param out
   * @return std::ostream&
   */
  std::ostream &write(std::ostream &out) override;
  /**
   * @brief десериализатор
   *
   * @param in
   * @param strategies
   * @return std::istream&
   */
  std::istream &
  read(std::istream &in,
       std::vector<std::shared_ptr<Read_strategy>> &strategies) override;
  /**
   * @brief метод кланирования экземпляра класса
   *
   * @return std::shared_ptr<Package>
   */
  std::shared_ptr<Package> clone() const override {
    return std::make_shared<Empty_package>(*this);
  }
};

#endif