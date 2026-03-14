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
  json j;
  j["type"] = "empty";
  std::stringstream sstr;

  linked_package->write(sstr);
  json linked = json::parse(sstr.str());
  std::string type = linked["type"];
  linked.erase("type");
  linked["linked_type"] = type;
  std::string file_name = linked["file_name"];
  linked.erase("file_name");
  linked["linked_file_name"] = file_name;
  out << j << linked;
  return out;
}

std::string my_readline(std::istream &in);
std::istream &
read_linked_package(std::istream &in, std::shared_ptr<Package> &linked_package,
                    std::vector<std::shared_ptr<Read_strategy>> &strategies);
