#ifndef PACKAGE_MANAGER_H
#define PACKAGE_MANAGER_H

#include "map/map.h"
#include "package/package.h"
#include <memory>
#include <string>

class Controler;
class Package_manager {
private:
  /**
   * @brief таблица для хранения пакетов
   *
   */
  Map<std::string, std::shared_ptr<Package>> map;
  /**
   * @brief проверка  на наличие циклов
   *
   * @param package
   */
  void cycle_check(const std::shared_ptr<Package> package);
  /**
   * @brief метод предназнченный для сопоставления указателей на одинаковый по
   * содержанию пакеты
   *
   * @param package
   */
  void connect_equal_pointers(std::shared_ptr<Package> package);

public:
  /**
   * @brief Конструктор
   *
   */
  explicit Package_manager(std::vector<std::shared_ptr<Package>>);
  /**
   * @brief Конструктор
   *
   */
  Package_manager() = default;
  /**
   * @brief Деструктор
   *
   */
  ~Package_manager() = default;
  /**
   * @brief метод  добавления нового пакета в хранилище
   *
   * @param package
   * @param main_flag
   */
  void add(std::shared_ptr<Package> package, bool main_flag = true);
  /**
   * @brief метод удаления пакета из хранилища
   *
   * @param package
   */
  void remove(std::shared_ptr<Package> package);
  /**
   * @brief метод удаления пакета из хранилища
   *
   * @param package_name
   */
  void remove(const std::string &package_name);
  /**
   * @brief удаление не используемых пакетов в параллейном режиме
   *
   */
  void remove_unuse();
  /**
   * @brief удаление не используемых пакетов в однопоточном режиме
   *
   */
  void remove_unuse_one_thread();
  /**
   * @brief метод очистки пакетного менеджера от всех пакетов
   *
   */
  void clean();
  /**
   * @brief метод который обновляет все пакеты
   *
   */
  void global_update();
  /**
   * @brief  метод очистки пакетного менеджера от всех пакетов
   *
   */
  void clear() { map.clear(); }
  /**
   * @brief метод который уничтожает цикл, необходим чтобы shared_ptr могли
   * корректно удалиться
   *
   * @param package
   * @return true
   * @return false
   */
  bool cycle_destroy(const std::shared_ptr<Package> package);
  /**
   * @brief метод получения количества пакетов в хранилище
   *
   * @return size_t
   */
  size_t size() { return map.size(); }

  friend Controler;
  /**
   * @brief метод поиска пакета
   *
   * @param package_name
   * @return true
   * @return false
   */
  bool find(const std::string &package_name) {
    return map.contains(package_name);
  }
  /**
   * @brief метод поиска пакета
   *
   * @param package_name
   * @param out
   * @return true
   * @return false
   */
  bool find(const std::string &package_name, std::ostream &out) {

    auto it = map.find(package_name);
    if (it != map.end()) {
      ((*it).second)->write(out);
      return true;
    }
    return false;
  }
};

#endif