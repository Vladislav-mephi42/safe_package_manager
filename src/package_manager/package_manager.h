#ifndef PACKAGE_MANAGER_H
#define PACKAGE_MANAGER_H

#include "map/map.h"
#include "package/package.h"
#include <memory>
#include <string>

class Controler;
class Package_manager {
private:
  Map<std::string, std::shared_ptr<Package>> map;
  static void cycle_check(const std::shared_ptr<Package> &package,
                          bool cycle_destroy_flag = true);
  void connect_equal_pointers(const std::shared_ptr<Package> &package);

public:
  explicit Package_manager(std::vector<std::shared_ptr<Package>> vec) {
    for (const auto &elem : vec) {
      add(elem);
    }
  }
  Package_manager() = default;
  ~Package_manager() = default;
  void add(std::shared_ptr<Package> package, bool main_flag = true,
           bool cycle_destroy_flag = true);
  void remove(const std::shared_ptr<Package> &package);
  void remove(const std::string &package_name);
  void remove_unuse();
  void remove_unuse_one_thread();
  void clean();
  void global_update();
  void clear() { map.clear(); }
  static bool cycle_destroy(const std::shared_ptr<Package> &package);
  size_t size() { return map.size(); }

  friend Controler;

  bool find(const std::string &package_name) {
    return map.contains(package_name);
  }

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