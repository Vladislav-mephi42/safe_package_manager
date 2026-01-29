#include "controler/controler.h"

#include "package/package.h"

#include "package_manager/package_manager.h"
#include <iostream>
#include <limits>
#include <memory>
#include <vector>

int read_int(std::istream &in) {
  int number = 0;
  if (in >> number) {

    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return number;
  } else {
    in.clear();
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    in.setstate(std::ios_base::failbit);
    return 0;
  }
}

std::string my_readline(std::istream &in) {
  std::string res;
  if (std::getline(in, res)) {
    return res;
  } else {
    in.setstate(std::ios_base::failbit);
    return "";
  }
}

std::istream &
read_req_packages(std::istream &in,
                  std::vector<std::shared_ptr<Package>> &req_packages,
                  std::vector<std::shared_ptr<Read_strategy>> &strategies,
                  int req_packages_count) {
  for (int i = 0; i < req_packages_count; i++) {
    if (!in) {
      throw std::runtime_error("invalid input(Req_1)");
    }
    std::string type_name = my_readline(in);
    if (!in) {
      throw std::runtime_error("invalid input(Req_2)");
    }
    for (auto &strategy : strategies) {
      if (strategy->can_read(type_name)) {
        req_packages[i] = strategy->read(in, strategies);
      }
    }
  }
  return in;
}

std::istream &
read_linked_package(std::istream &in, std::shared_ptr<Package> &linked_package,
                    std::vector<std::shared_ptr<Read_strategy>> &strategies) {

  if (!in) {
    throw std::runtime_error("invalid input(req_1)");
  }
  std::string type_name = my_readline(in);
  if (!in) {
    throw std::runtime_error("invalid input(req_2)");
  }
  for (auto &strategy : strategies) {
    if (strategy->can_read(type_name)) {
      linked_package = strategy->read(in, strategies);
      if (linked_package == nullptr) {
        throw std::runtime_error("deserealization error");
      }
      return in;
    }
  }

  return in;
}

std::shared_ptr<Package> Controler::read_package(std::istream &in) {
  if (!in) {
    in.setstate(std::ios::failbit);
    return nullptr;
  }
  std::string type_name = my_readline(in);
  if (!in) {
    in.setstate(std::ios::failbit);
    return nullptr;
  }
  for (auto &strategy : strategies) {
    if (strategy->can_read(type_name)) {
      return strategy->read(in, strategies);
    }
  }
  in.setstate(std::ios::failbit);
  return nullptr;
}

std::ostream &Controler::write_package(std::shared_ptr<Package> package,
                                       std::ostream &out) {
  return package->write(out);
}

Package_manager Controler::read_manager(std::istream &in) {

  Package_manager manager;
  if (!in) {
    throw std::runtime_error("bad input");
  }
  std::shared_ptr<Package> package = read_package(in);

  while (!in.eof() && in) {
    try {
      if (!manager.find(package->get_file_name())) {
        manager.add(package);
      }
    } catch (const std::exception &e) {
      std::string what(e.what());
      if (what == "cycle found") {
        Package_manager pm;
        pm.cycle_destroy(package);
      }

      throw;
    }
    package = read_package(in);
  }
  return manager;
}

void Controler::write_mananger(Package_manager manager, std::ostream &out) {

  for (auto &value : manager.map) {
    if (value.second->get_using_flag()) {
      (value.second)->write(out);
    }
  }
}
