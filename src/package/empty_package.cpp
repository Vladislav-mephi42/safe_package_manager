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
  std::string linked_package_name = linked["package_name"];

  linked["linked_package_name"] = linked_package_name;
  linked["type"] = "empty";
  linked["package_name"] = get_package_name();
  out << linked.dump(4);
  return out;
}

json Empty_package::write_to_json() const {

  std::stringstream sstr;

  linked_package->write(sstr);
  json linked = json::parse(sstr.str());

  std::string linked_type = linked["type"];
  linked["linked_type"] = linked_type;

  std::string linked_package_name = linked["package_name"];
  linked["linked_package_name"] = linked_package_name;

  linked["type"] = "empty";
  linked["package_name"] = get_package_name();

  return linked;
}
