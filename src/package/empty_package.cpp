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

  std::stringstream sstr;

  linked_package->write(sstr);
  json linked = json::parse(sstr.str());

  std::string linked_type = linked["type"];
  linked["linked_type"] = linked_type;

  std::string linked_file_name = linked["file_name"];
  linked["linked_file_name"] = linked_file_name;

  linked["type"] = "empty";
  linked["file_name"] = file_name;
  out << linked;
  return out;
}

std::string my_readline(std::istream &in);
std::istream &
read_linked_package(std::istream &in, std::shared_ptr<Package> &linked_package,
                    std::vector<std::shared_ptr<Read_strategy>> &strategies);
