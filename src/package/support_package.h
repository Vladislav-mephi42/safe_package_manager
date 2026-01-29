#ifndef SUPPORT_PACKAGE
#define SUPPORT_PACKAGE

#include "package/package.h"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

class Main_package;
/**
 * @brief класс пакет со вспомогательным ПО
 *
 */
class Support_package : public Package {
private:
  /**
   * @brief имя файла
   *
   */
  std::string file_name;
  /**
   * @brief имя издателя
   *
   */
  std::string publisher_name;
  /**
   * @brief тякущая версия
   *
   */
  std::string current_version;
  /**
   * @brief последняя версия
   *
   */
  std::string last_version;

  /**
   * @brief флаг установки
   *
   */
  bool condition = false;
  /**
   * @brief флаг "установлен вручную"
   *
   */
  bool using_flag = false;

  /**
   * @brief пакеты, используемые данным
   *
   */
  std::vector<std::shared_ptr<Package>> req_packages;
  /**
   * @brief метод проверки корректности полей
   *
   */
  void correct() {
    if (publisher_name == "" || current_version == "" || last_version == "") {
      throw std::invalid_argument("Empty field");
    }
    if (file_name.length() < 5) {
      throw std::invalid_argument("invalid file_name ");
    }
    if (file_name.substr(file_name.length() - 4) != ".dep") {
      throw std::invalid_argument("invalid file_name");
    }
  }

public:
  /**
   * @brief Конструктор
   *
   * @param file_name
   * @param publisher_name
   * @param current_version
   * @param last_version
   * @param req_packages
   */
  Support_package(const std::string &file_name,
                  const std::string &publisher_name,
                  const std::string &current_version,
                  const std::string &last_version,
                  const std::vector<std::shared_ptr<Package>> &req_packages)
      : file_name(file_name), publisher_name(publisher_name),
        current_version(current_version), last_version(last_version),
        req_packages(std::move(req_packages)) {
    correct();
  }
  /**
   * @brief Конструктор
   *
   * @param file_name
   * @param publisher_name
   * @param current_version
   * @param last_version
   * @param req_packages
   */
  Support_package(std::string &&file_name, std::string &&publisher_name,
                  std::string &&current_version, std::string &&last_version,
                  std::vector<std::shared_ptr<Package>> &&req_packages)
      : file_name(std::move(file_name)),
        publisher_name(std::move(publisher_name)),
        current_version(std::move(current_version)),
        last_version(std::move(last_version)),
        req_packages(std::move(req_packages)) {
    correct();
  }
  /**
   * @brief Конструктор по умолчанию
   *
   */
  Support_package() = default;
  /**
   * @brief Деструктор
   *
   */
  ~Support_package() override = default;
  /**
   * @brief метод подготовки к вставке
   *
   */
  void add() override;
  /**
   * @brief метод подготовки к удалению
   *
   */
  void remove() override;
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
    return publisher_name;
  }
  /**
   * @brief геттер флага установки
   *
   * @return true
   * @return false
   */
  bool get_condition() const noexcept override { return condition; };
  std::string get_current_version() const noexcept override {
    return current_version;
  };
  /**
   * @brief геттер последней версии
   *
   * @return std::string
   */
  std::string get_last_version() const noexcept override {
    return last_version;
  };
  /**
   * @brief геттер имени файла
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
    publisher_name = new_publisher_name;
  };
  /**
   * @brief сеттер текущей версии
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
   * @brief сеттер последней версии
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
   * @brief геттер пакетов, используемых данным
   *
   * @return std::vector<std::shared_ptr<Package>>
   */
  std::vector<std::shared_ptr<Package>>
  get_connected_packages() const noexcept override {
    return req_packages;
  }
  /**
   * @brief добавление нового используемого пакета
   *
   * @param package
   * @return true
   * @return false
   */
  bool insert_connected(std::shared_ptr<Package> package) override {
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
    return true;
  }
  /**
   * @brief метод очистки используемого пакета
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
   * @brief оператор ==
   *
   * @param o
   * @return true
   * @return false
   */
  bool operator==(const Package &o) const override {

    if (dynamic_cast<const Support_package *>(&o) == nullptr) {
      return false;
    }
    auto this_t = std::make_tuple(current_version, last_version, publisher_name,
                                  file_name);
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
  bool get_using_flag() const noexcept override { return false; }
  /**
   * @brief сеттер флага "установлен вручную"
   *
   * @param new_using_flag
   */
  void set_using_flag(bool new_using_flag) override {
    (void)new_using_flag;
    return;
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
   * @brief метод клонирования экземпляра обьекта
   *
   * @return std::shared_ptr<Package>
   */
  std::shared_ptr<Package> clone() const override {
    return std::make_shared<Support_package>(*this);
  }
  /**
   * @brief метод разбиения пакетов с вспомагательным ПО на части
   *
   * @param parts_count
   * @return std::vector<std::shared_ptr<Package>>
   */
  std::vector<std::shared_ptr<Package>> devide(unsigned int parts_count);
  /**
   * @brief метод обьединения пакетов со вспомагательным ПО
   *
   * @param packages
   * @return std::shared_ptr<Package>
   */
  std::shared_ptr<Package>
  connect(std::vector<std::shared_ptr<Package>> packages);
};

#endif