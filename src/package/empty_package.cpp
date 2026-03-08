#include "package/empty_package.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

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
json Empty_package::read(std::istream &in) {
  json j;
  return j;
}