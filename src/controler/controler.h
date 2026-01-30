#include "controler/strategy.h"
#include "package/package.h"
#include "package_manager/package_manager.h"
#include <vector>

class Controler {
private:
  std::vector<std::shared_ptr<Read_strategy>> strategies;
  std::vector<std::shared_ptr<Package>> read_packages(std::istream &in);

public:
  Controler() {
    Empty_read empty;
    Support_read support;
    Main_read main;
    strategies.push_back(std::make_shared<Empty_read>(empty));
    strategies.push_back(std::make_shared<Support_read>(support));
    strategies.push_back(std::make_shared<Main_read>(main));
  }
  std::shared_ptr<Package> read_package(std::istream &in);

  static std::ostream &write_package(const std::shared_ptr<Package> &package,
                                     std::ostream &out);
  Package_manager read_manager(std::istream &in);
  static void write_mananger(const Package_manager &manager, std::ostream &out);
};