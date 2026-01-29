

#include "controler/controler.h"

#include "package/package.h"

#include "package_manager/package_manager.h"
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>

void print_menu() {

  std::cout << std::endl;
  std::cout << "=========================MENU==========================="
            << std::endl;
  std::cout << "1. Add package------------------------------------------"
            << std::endl;
  std::cout << "2. Remove package --------------------------------------"
            << std::endl;
  std::cout << "3. Find package-----------------------------------------"
            << std::endl;
  std::cout << "4. Remove unused----------------------------------------"
            << std::endl;
  std::cout << "5. Add package from file--------------------------------"
            << std::endl;
  std::cout << "6. Print size-------------------------------------------"
            << std::endl;
  std::cout << "7. Global update----------------------------------------"
            << std::endl;

  std::cout << "9. EXIT-------------------------------------------------"
            << std::endl;
  std::cout << "========================================================"
            << std::endl;
  std::cout << std::endl;
}

void package_format() {
  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << "INPUT PACKAGE FORMAT\n" << std::endl;

  std::cout << "package type(for example support)" << std::endl;
  std::cout << "using flag\n" << std::endl;
  std::cout << "File_name(end with .dep\\n" << std::endl;
  std::cout << "Publisher name\\n" << std::endl;
  std::cout << "Current version\\n" << std::endl;
  std::cout << "Last version\\n" << std::endl;
  std::cout << "Count of requirement packages\\n" << std::endl;
  std::cout << "Req_packages in INPUT PACKAGE FORMAT\\n" << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
}

int read_int(const std::string &prompt) {
  int number = 0;

  while (true) {
    std::cout << "Input " << prompt << " --> ";

    if (std::cin >> number) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      break;
    }
    if (std::cin.eof()) {
      throw std::runtime_error("EOF");
    }

    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Wrong input, please try again." << std::endl;
  }

  return number;
}
std::string my_readline(std::istream &in);
int main() {
  Controler controler;
  Package_manager package_manager;
  std::string package_name;
  std::cout
      << "Enter backup file_name\n If you dont want loading from backup, Enter"
      << std::endl;
  std::shared_ptr<Package> package;
  std::string file_name = my_readline(std::cin);
  try {
    std::ifstream input_file(file_name);

    if (input_file.is_open()) {
      package_manager = controler.read_manager(input_file);
      input_file.close();
    } else {
      std::cout << "Starting without backup" << std::endl;
    }
  } catch (const std::exception &e) {
    std::string what(e.what());

    std::cerr << "Error: " << e.what() << std::endl;

    return 1;
  }

  try {
    std::string file_name;
    std::ifstream package_file;
    int option = 0;
    do {

      print_menu();
      option = read_int("option");
      switch (option) {
      case 1:
        package_format();
        package = controler.read_package(std::cin);
        if (package != nullptr) {
          package_manager.add(package);
        } else {
          throw std::runtime_error("bad input");
        }
        break;

      case 2:
        std::cout << "package name -->" << std::flush;

        std::cin >> package_name;
        package_manager.remove(package_name);

        break;

      case 3:
        std::cout << "package name -->" << std::flush;

        std::cin >> package_name;
        if (package_manager.find(package_name, std::cout)) {
          std::cout << "This pakage exist" << std::endl;
          break;
        }
        std::cout << "This pakage did not exist" << std::endl;
        break;
      case 4:
        package_manager.remove_unuse();
        break;
      case 5:
        std::cout << "Enter file_name -->" << std::flush;
        file_name = my_readline(std::cin);
        package_file = std::ifstream(file_name);

        if (!package_file.is_open()) {
          std::cerr << "File didnot exist\n";
          return 1;
        }

        package = controler.read_package(package_file);
        if (package != nullptr) {
          package_manager.add(package);
        } else {
          throw std::runtime_error("bad input");
        }
        package_file.close();
        break;
      case 6:
        std::cout << "size = " << package_manager.size() << std::endl;
        break;
      case 7:
        package_manager.global_update();
        break;

      case 9:
        break;

      default:
        std::cout << option << "is not an option" << std::endl;
        break;
      }

    } while (option != 9);
  } catch (const std::exception &e) {
    std::string what(e.what());
    if (what == "cycle found") {
      package_manager.cycle_destroy(package);
    }
    std::cerr << "Error: " << e.what() << std::endl;

    return 1;
  }
  std::cout
      << "Enter backup file_name\n If you dont want storing  backup, Enter"
      << std::endl;
  std::string b_file_name = my_readline(std::cin);

  std::ofstream output_file(b_file_name);

  if (!output_file.is_open()) {
    std::cerr << "Fail backup\n";
    return 1;
  }

  controler.write_mananger(package_manager, output_file);
  output_file.close();
  return 0;
}
