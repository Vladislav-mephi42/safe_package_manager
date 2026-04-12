
#include "controler/controler.h"
#include "view/view.h"

#include "network/client.h"
#include "network/network_controler.h"
#include "network/server.h"
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
  std::cout << "Enter availible-packages file_name\n If you dont want loading "
               "from backup, Enter"
            << std::endl;
  std::shared_ptr<Package> package;
  std::string storage_file_name = View::readline(std::cin);
  if (storage_file_name == "") {
    storage_file_name = "availible_packages.json";
  }

  std::string file_name;
  std::ifstream package_file;

  int option = 0;
  do {
    try {
      std::cout << std::endl;
      std::cout << "=========================MENU==========================="
                << std::endl;
      std::cout << "1. Add package------------------------------------------"
                << std::endl;
      std::cout << "2. Remove package --------------------------------------"
                << std::endl;
      std::cout << "3. Exit ------------------------------------------------"
                << std::endl;
      std::cout << "========================================================"
                << std::endl;
      option = View::read_int("option", std::cin, std::cout);

      std::string name;
      std::string password;
      std::shared_ptr<Package> package;
      json package_json;
      json package_array = json::array();
      json request;
      json response;
      std::string file_name;

      Client_socket client(49152, "127.0.0.1");

      switch (option) {
      case 1:
        std::cout << "Enter package name from " << storage_file_name
                  << std::endl;

        name = View::readline(std::cin);
        std::cout << "Enter file name" << std::endl;
        file_name = View::readline(std::cin);

        std::cout << "Enter admin password " << std::endl;
        password = View::readline(std::cin);
        std::cout << std::endl;
        package = controler.read_package_from_file(name, storage_file_name);

        controler.write_package_to_json(package, package_array);
        package_json["packages"] = package_array;
        request["request_type"] = "add";
        request["user_type"] = "admin";
        request["password"] = password;
        request["package"] = package_json;
        request["file_name"] = file_name;
        request["package_name"] = package->get_file_name();

        client.send_json(request);
        response = client.recv_json();
        std::cout << "Status : " << response["status"] << std::endl;
        std::cout << "Message : " << response["final"] << std::endl;
        client.manuly_close();
        break;
      case 2:
        std::cout << "Enter package name" << std::endl;
        name = View::readline(std::cin);
        std::cout << "Enter file name" << std::endl;
        file_name = View::readline(std::cin);
        std::cout << "Enter admin password " << std::endl;
        password = View::readline(std::cin);
        std::cout << std::endl;

        request["request_type"] = "remove";
        request["user_type"] = "admin";
        request["password"] = password;
        request["package_name"] = name;
        request["file_name"] = file_name;

        client.send_json(request);
        response = client.recv_json();

        std::cout << "Status : " << response["status"] << std::endl;
        std::cout << "Message : " << response["final"] << std::endl;
        client.manuly_close();
        break;
      case 3:
        break;

      default:
        std::cout << option << "is not an option" << std::endl;
        break;
      }
    }

    catch (const std::exception &e) {
      std::string what(e.what());

      if (what == "EOF") {
        std::cout << "EOF, exit........." << std::endl;
        return 1;
      }
      std::cerr << "Error: " << e.what() << std::endl;
      return 1;
    }
  } while (option != 9);

  return 0;
}
