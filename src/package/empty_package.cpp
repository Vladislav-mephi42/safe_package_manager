#include "package/empty_package.h"

void Empty_package::add() {
  if (!condition) {
    condition = true;
  }
}

void Empty_package::remove() { condition = false; }

std::ostream &Empty_package::write(std::ostream &out) {
  out << "empty" << "\n";
  out << file_name << "\n";
  linked_package->write(out);
  return out;
}

std::string my_readline(std::istream &in);
std::istream &
read_linked_package(std::istream &in, std::shared_ptr<Package> &linked_package,
                    std::vector<std::shared_ptr<Read_strategy>> &strategies);
std::istream &
Empty_package::read(std::istream &in,
                    std::vector<std::shared_ptr<Read_strategy>> &strategies) {
  if (!in) {
    return in;
  }
  file_name = my_readline(in);
  if (!in) {
    return in;
  }
  read_linked_package(in, linked_package, strategies);
  if (!linked_package) {
    throw std::runtime_error("deserealization error(EMPTY PACKAGE)");
  }
  return in;
}