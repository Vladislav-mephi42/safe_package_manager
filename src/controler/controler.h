#include "controler/strategy.h"
#include "package/package.h"
#include "package_manager/package_manager.h"
#include <vector>
/**
 * @brief Класс предназначенный для сериализации и десериализации пакетов и их
 * хранилища
 *
 */
class Controler {
private:
  /**
   * @brief Массив состоящий из класса стратегия, который нужен для того чтобы
   * подбирать нужную для десериализации функцию
   *
   */
  std::vector<std::shared_ptr<Read_strategy>> strategies;
  /**
   * @brief метод стения пакетов
   *
   * @param in
   * @return std::vector<std::shared_ptr<Package>>
   */
  std::vector<std::shared_ptr<Package>> read_packages(std::istream &in);

public:
  /**
   * @brief Конструктор
   *
   */
  Controler() {
    Empty_read empty;
    Support_read support;
    Main_read main;
    strategies.push_back(std::make_shared<Empty_read>(empty));
    strategies.push_back(std::make_shared<Support_read>(support));
    strategies.push_back(std::make_shared<Main_read>(main));
  }
  /**
   * @brief чтение одного пакета
   *
   * @param in
   * @return std::shared_ptr<Package>
   */
  std::shared_ptr<Package> read_package(std::istream &in);
  /**
   * @brief метод записи пакета
   *
   * @param package
   * @param out
   * @return std::ostream&
   */
  std::ostream &write_package(std::shared_ptr<Package> package,
                              std::ostream &out);
  /**
   * @brief метод чтения пакетного менеджера
   *
   * @param in
   * @return Package_manager
   */
  Package_manager read_manager(std::istream &in);
  /**
   * @brief метод записи пакетного менеджера
   *
   * @param manager
   * @param out
   */
  void write_mananger(Package_manager manager, std::ostream &out);
};