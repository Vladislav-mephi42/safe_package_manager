#ifndef PACKAGE_MANAGER_H
#define PACKAGE_MANAGER_H

#include "map/map.h"
#include "package/package.h"
#include <memory>
#include <string>

class Controler;
/**
 * @brief Represents a storage of software packages that manages dependencies.
 *
 * Stores software packages in a hash table and maintains the main class
 * invariant — the absence of cyclic dependencies. Can remove a specified
 * package if no other package depends on it.
 *
 */
class Package_manager {
private:
  /** Hash map that stores packages, keyed by package name. */
  Map<std::string, std::shared_ptr<Package>> map;

  /**
   *@brief Replaces shared pointers in the required packages list with
   *        equivalent pointers from the hash table.
   *
   * @param package
   * @param cycle_destroy_flag
   */
  void connect_equal_pointers(const std::shared_ptr<Package> &package,
                              bool cycle_destroy_flag = true);
  /**
   * @brief Adds a new package to the manager.
   *
   * If main_flag is true, this is the initial call in the insertion chain.
   * If false, the method is being called recursively to add a dependency.
   * The cycle_destroy_flag controls whether detected cyclic dependencies
   * are broken before an exception is thrown.
   * @param package
   * @param main_flag
   * @param cycle_destroy_flag
   */
  void add(const std::shared_ptr<Package> &package, bool main_flag = true,
           bool cycle_destroy_flag = true);

public:
  /**
   * @brief Constructs a Package_manager with the specified set of packages.
   *
   * @param vec
   */
  explicit Package_manager(std::vector<std::shared_ptr<Package>> vec) {
    for (const auto &elem : vec) {
      add(elem);
    }
  }
  /**
   * @brief Constructs a new Package_manager object.
   *
   */
  Package_manager() = default;
  /**
   * @brief Destroys the Package_manager object.
   *
   */
  ~Package_manager() = default;
  /**
   * @brief Adds a new package with deep copying of its dependencies.
   *
   * Unlike add(), this method creates deep copies of all dependency packages
   * before inserting them. If main_flag is true, this is the initial call.
   * The cycle_destroy_flag controls whether cycles are broken before an
   * exception is thrown.
   * @param package
   * @param main_flag
   * @param cycle_destroy_flag
   */
  void add_with_deep_copying(const std::shared_ptr<Package> &package,
                             bool main_flag = true,
                             bool cycle_destroy_flag = true);
  /**
   * @brief Removes the specified package from the manager.
   *
   * Also marks the package as not installed. The package cannot be removed
   * if any other package depends on it.
   * @param package
   */
  void remove(const std::shared_ptr<Package> &package);
  /**
   * @brief Removes the specified package from the manager.
   * Also marks the package as not installed. The package cannot be removed
   * if any other package depends on it.
   *
   * @param package_name
   */
  void remove(const std::string &package_name);
  /**
   * @brief Removes packages that are no longer in use. (Parallel mode)
   *
   * Removes all packages that were only present as dependencies of other
   * packages and are no longer required. Uses parallel execution.
   */
  void remove_unuse();

  /**
   * @brief Removes packages that are no longer in use. (Single-threaded mode)
   *
   * Removes all packages that were only present as dependencies of other
   * packages and are no longer required. Runs in a single thread.
   */
  void remove_unuse_one_thread();

  void clean();
  /**
   * @brief Calls clear for the hash table.
   *
   */
  void clear() { map.clear(); }
  /**
   * @brief Breaks cyclic shared_ptr references originating from the given
   * package.
   *
   * @param package
   * @return true
   * @return false
   */
  static bool cycle_destroy(const std::shared_ptr<Package> &package);
  /**
   * @brief Returns the number of packages in the manager.
   *
   * @return size_t
   */
  size_t size() { return map.size(); }

  friend Controler;
  /**
   * @brief Checks whether the package manager contains a package with the
   *        specified name.
   *
   * @param package_name
   * @return true
   * @return false
   */
  bool find(const std::string &package_name) {
    return map.contains(package_name);
  }
  /**
   * @brief Checks whether the package manager contains a package with the
   *        specified name and writes its metadata to the output stream.
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
  void global_update() {
    for (auto &elem : map) {
      (elem.second)->set_current_version((elem.second)->get_last_version());
    }
  }
  /**
   * @brief Checks whether the package has cyclic dependencies.
   * Traverses the dependency graph starting from the given package and
   * throws an exception if a cycle is detected.
   * @param package
   * @param cycle_destroy_flag
   */
  static void cycle_check(const std::shared_ptr<Package> &package,
                          bool cycle_destroy_flag = true);
};

#endif