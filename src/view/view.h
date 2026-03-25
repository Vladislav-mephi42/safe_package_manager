#ifndef VIEW
#define VIEW

#include "package/package.h"
#include "package_manager/package_manager.h"
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>

class View {
public:
  static void print_menu(std::ostream &out) noexcept {

    out << std::endl;
    out << "=========================MENU==========================="
        << std::endl;
    out << "1. Add package------------------------------------------"
        << std::endl;
    out << "2. Remove package --------------------------------------"
        << std::endl;
    out << "3. Find package-----------------------------------------"
        << std::endl;
    out << "4. Remove unused----------------------------------------"
        << std::endl;
    out << "5. Add package from file--------------------------------"
        << std::endl;
    out << "6. Print size-------------------------------------------"
        << std::endl;
    out << "7. Global update----------------------------------------"
        << std::endl;
    out << "9. EXIT-------------------------------------------------"
        << std::endl;
    out << "========================================================"
        << std::endl;
    out << std::endl;
  }

  static void print_package(const std::shared_ptr<Package> &package,
                            std::ostream &out) {
    json data = package->write_to_json();
    out << data.dump(4);
  }

  static int read_int(const std::string &prompt, std::istream &in,
                      std::ostream &out) {
    int number = 0;

    while (true) {
      out << "Input " << prompt << " --> ";

      if (in >> number) {
        in.clear();
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        break;
      }
      if (in.eof()) {
        throw std::runtime_error("EOF");
      }

      in.clear();
      in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      out << "Wrong input, please try again." << std::endl;
    }

    return number;
  }

  static std::string readline(std::istream &in) {
    std::string res;
    if (std::getline(in, res)) {
      return res;
    }
    in.setstate(std::ios_base::failbit);
    return "";
  }

  static void package_format(std::ostream &out) {
    out << std::endl;
    out << "=======INPUT PACKAGE FORMAT======" << std::endl;
    out << "  package name (.dep extension)  " << std::endl;
    out << "       package must exist!       " << std::endl;
    out << "=================================" << std::endl;
    out << std::endl;
  }
};

#endif