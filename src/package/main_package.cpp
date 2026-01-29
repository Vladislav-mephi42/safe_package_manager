#include "package/main_package.h"
#include <iostream>

void Main_package::add() {
  if (!condition) {
    condition = true;
  }
}

void Main_package::remove() { condition = false; }

std::ostream &Main_package::write(std::ostream &out) {
  out << "main" << "\n";
  if (using_flag) {
    out << 1 << "\n";
  } else {
    out << 0 << "\n";
  }

  out << file_name << "\n";
  out << publisher_name << "\n";
  out << current_version << "\n";
  out << last_version << "\n";
  out << req_packages.size() << "\n";
  for (std::shared_ptr<Package> package : req_packages) {
    package->write(out);
  }
  return out;
}

int read_int(std::istream &in);
std::string my_readline(std::istream &in);
std::istream &
read_req_packages(std::istream &in,
                  std::vector<std::shared_ptr<Package>> &req_packages,
                  std::vector<std::shared_ptr<Read_strategy>> &strategies,
                  int req_packages_count);

std::istream &
Main_package::read(std::istream &in,
                   std::vector<std::shared_ptr<Read_strategy>> &strategies) {
  if (!in) {
    return in;
  }
  int using_flag = read_int(in);

  if (!in) {
    return in;
  }
  std::string file_name = my_readline(in);
  if (!in) {
    return in;
  }

  std::string publisher_name = my_readline(in);
  if (!in) {
    return in;
  }

  std::string current_version = my_readline(in);
  if (!in) {
    return in;
  }
  std::string last_version = my_readline(in);
  if (!in) {
    return in;
  }
  int req_packages_count = read_int(in);
  if (!in) {
    return in;
  }

  std::vector<std::shared_ptr<Package>> tmp_req_packages(req_packages_count);

  read_req_packages(in, tmp_req_packages, strategies, req_packages_count);
  set_file_name(file_name);
  set_publisher_name(publisher_name);
  set_current_version(current_version);
  set_last_version(last_version);
  set_using_flag(static_cast<bool>(using_flag));
  req_packages = tmp_req_packages;

  return in;
}