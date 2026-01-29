#ifndef PACKAGE_H
#define PACKAGE_H

#include <algorithm>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

class Read_strategy;
/**
 * @brief Абстрактный класс, представляющий пакет с ПО
 *
 */
class Package {

public:
  /**
   * @brief Деструктор
   *
   */
  virtual ~Package() = 0;
  /**
   * @brief метод подготовки к вставке
   *
   */
  virtual void add() = 0;
  /**
   * @brief метод подготовки к удвлению
   *
   */
  virtual void remove() = 0;
  /**
   * @brief геттер для  флага установки
   *
   * @return true
   * @return false
   */
  virtual bool get_condition() const noexcept = 0;
  /**
   * @brief геттер для текущей версии
   *
   * @return std::string
   */
  virtual std::string get_current_version() const noexcept = 0;
  /**
   * @brief геттер для последней версии
   *
   * @return std::string
   */
  virtual std::string get_last_version() const noexcept = 0;
  /**
   * @brief геттер для имени издателя
   *
   * @return std::string
   */
  virtual std::string get_publisher_name() const noexcept = 0;
  /**
   * @brief геттер для имени файла
   *
   * @return std::string
   */
  virtual std::string get_file_name() const noexcept = 0;
  /**
   * @brief сеттер для имени файла
   *
   * @param new_file_name
   */
  virtual void set_file_name(const std::string &new_file_name) = 0;
  /**
   * @brief сеттер для имени издателя
   *
   * @param new_publisher_name
   */
  virtual void set_publisher_name(const std::string &new_publisher_name) = 0;
  /**
   * @brief сеттер для текущей версии
   *
   * @param new_version
   */
  virtual void set_current_version(const std::string &new_version) = 0;
  /**
   * @brief сеттре для последней версии
   *
   * @param new_version
   */
  virtual void set_last_version(const std::string &new_version) = 0;
  /**
   * @brief геттер для пакетов от которых зависит данный
   *
   * @return std::vector<std::shared_ptr<Package>>
   */
  virtual std::vector<std::shared_ptr<Package>>
  get_connected_packages() const noexcept = 0;
  /**
   * @brief добавленик нового используемого пакета
   *
   * @param package
   * @return true
   * @return false
   */
  virtual bool insert_connected(std::shared_ptr<Package> package) = 0;
  /**
   * @brief удаление одного из используемых пакетов
   *
   * @param package
   * @return true
   * @return false
   */
  virtual bool erase_connected(const Package &package) = 0;
  /**
   * @brief оператор ==
   *
   * @return true
   * @return false
   */
  virtual bool operator==(const Package &) const = 0;
  /**
   * @brief геттер флага "установлен в ручную"
   *
   * @return true
   * @return false
   */
  virtual bool get_using_flag() const noexcept = 0;
  /**
   * @brief сеттер флага "установлен в ручную"
   *
   * @param new_using_flag
   */
  virtual void set_using_flag(bool new_using_flag) = 0;
  /**
   * @brief метод сериализации в выходящий поток
   *
   * @param out
   * @return std::ostream&
   */
  virtual std::ostream &write(std::ostream &out) = 0;
  /**
   * @brief метод десириализации из входного потока
   *
   * @param in
   * @param strategies
   * @return std::istream&
   */
  virtual std::istream &
  read(std::istream &in,
       std::vector<std::shared_ptr<Read_strategy>> &strategies) = 0;
  /**
   * @brief вспомогательный метод для клонирования обьекта
   *
   * @return std::shared_ptr<Package>
   */
  virtual std::shared_ptr<Package> clone() const = 0;
};

#endif