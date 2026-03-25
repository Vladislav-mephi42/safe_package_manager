
#include "controler/controler.h"
#include "view/view.h"

#include "package/package.h"

#include "package_manager/package_manager.h"
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>

int main() {

  Package_manager package_manager;
  std::string package_name;
  Controler controler;
  std::cout
      << "Enter backup file_name\n If you dont want loading from backup, Enter"
      << std::endl;
  std::shared_ptr<Package> package;
  std::string storage_file_name = View::readline(std::cin);
  std::cout
      << "Enter json data bases names, the first step is writting number of it"
      << std::endl;
  std::cout
      << "If you write 0, default data bases will be used (1.json, 2.json)"
      << std::endl;
  int len = View::read_int("number a number of json data bases names", std::cin,
                           std::cout);
  std::vector<std::string> json_repositories_names;
  if (len > 0) {
    for (int i = 0; i < len; i++) {
      std::string base_name = View::readline(std::cin);
      json_repositories_names.push_back(base_name);
    }
  } else {
    std::cout << "start with default repozitories (1.json, 2.json)"
              << std::endl;
    json_repositories_names = {"1.json", "2.json"};
  }

  try {

    if (storage_file_name != "") {

      controler = Controler(json_repositories_names, storage_file_name,
                            &package_manager, true);

    } else {
      controler =
          Controler(json_repositories_names, "backup.json", &package_manager);
      std::cout << "Starting without backup" << std::endl;
      std::cout << "This package manager will be stored in bacup.json"
                << std::endl;
    }
  } catch (const std::exception &e) {
    std::string what(e.what());
    std::cerr << "Error: " << e.what() << std::endl;
    std::cout << "Fail starting, please try again" << std::endl;
    return 1;
  }

  std::string file_name;
  std::ifstream package_file;
  int option = 0;
  do {
    try {
      View::print_menu(std::cout);
      option = View::read_int("option", std::cin, std::cout);
      switch (option) {
      case 1:
        View::package_format(std::cout);
        package_name = View::readline(std::cin);
        controler.add_package(package_name);
        break;

      case 2:
        std::cout << "package name -->" << std::flush;

        std::cin >> package_name;
        package_manager.remove(package_name);

        break;

      case 3:
        std::cout << "print package name -->" << std::flush;
        package_name = View::readline(std::cin);

        std::cout << "========================================================"
                  << std::endl;
        std::cout << "====================PACKAGE INFO========================"
                  << std::endl;
        if (package_manager.find(package_name, std::cout)) {
          std::cout << "======================================================"
                    << std::endl;
          break;
        }
        std::cout << "This pakage did not exist" << std::endl;
        std::cout << "========================================================"
                  << std::endl;
        break;

      case 4:
        controler.remove_unuse();
        break;

      case 6:
        std::cout << "size = " << package_manager.size() << std::endl;
        break;

      case 7:
        controler.global_update();
        break;

      case 9:
        break;

      default:
        std::cout << option << "is not an option" << std::endl;
        break;
      }
    }

    catch (const std::exception &e) {
      std::string what(e.what());
      if (what == "cycle found") {
        package_manager.cycle_destroy(package);
        std::cout << "Cycle is destroed" << std::endl;
      }
      if (what == "EOF") {
        std::cout << "EOF, exit........." << std::endl;
        return 1;
      }
      std::cerr << "Error: " << e.what() << std::endl;
    }
  } while (option != 9);

  return 0;
}
