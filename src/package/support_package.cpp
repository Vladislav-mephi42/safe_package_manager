#include "package/support_package.h"
#include <iostream>
#include <regex>
#include <sstream>
#include <unordered_set>
#include <vector>

void Support_package::add() {
  if (!condition) {
    condition = true;
  }
}

void Support_package::remove() { condition = false; }

std::ostream &Support_package::write(std::ostream &out) {
  out << "support" << "\n";
  out << 0 << "\n";
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
Support_package::read(std::istream &in,
                      std::vector<std::shared_ptr<Read_strategy>> &strategies) {
  if (!in) {
    return in;
  }
  read_int(in); // read using flag, which is unsused in support package

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
  req_packages = tmp_req_packages;

  return in;
}

std::string devide_name(const std::string &filename, unsigned int part_number) {
  const std::string suffix = ".dep";
  if (filename.length() < suffix.length() ||
      filename.substr(filename.length() - suffix.length()) != suffix) {
    throw std::invalid_argument("Invalid input");
  }

  std::string base = filename.substr(0, filename.length() - suffix.length());

  std::ostringstream oss;
  oss << base << "_part_" << part_number << suffix;
  return oss.str();
}

std::vector<std::shared_ptr<Package>>
Support_package::devide(unsigned int parts_count) {
  std::vector<std::shared_ptr<Package>> parts(parts_count);
  for (unsigned int i = 0; i < parts_count; i++) {
    std::shared_ptr<Package> package = this->clone();
    package->set_file_name(devide_name(file_name, i + 1));
    parts[i] = package;
  }
  return parts;
}

bool can_connect(const std::vector<std::string> &filenames) {
  if (filenames.empty()) {
    return true;
  }

  const std::regex pattern(R"(^(.*)_part_([1-9]\d*)\.dep$)");
  std::smatch matches;

  std::string expectedPrefix;
  std::unordered_set<unsigned int> seenNumbers;

  for (const auto &name : filenames) {
    if (!std::regex_match(name, matches, pattern)) {
      return false;
    }

    std::string prefix = matches[1].str();
    unsigned int number = std::stoul(matches[2].str());

    if (expectedPrefix.empty()) {
      expectedPrefix = prefix;
    } else if (prefix != expectedPrefix) {
      return false;
    }

    if (!seenNumbers.insert(number).second) {
      return false;
    }
  }

  return true;
}

std::string get_connected_name(const std::vector<std::string> &filenames) {
  if (!can_connect(filenames)) {
    throw std::invalid_argument("connecting not available");
  }

  if (filenames.empty()) {
    throw std::invalid_argument("empty massive");
  }

  const std::regex pattern(R"(^(.*)_part_[1-9]\d*\.dep$)");
  std::smatch matches;

  if (std::regex_match(filenames[0], matches, pattern)) {

    return matches[1].str() + ".dep";
  }
  throw std::logic_error("something get wrong");
}

std::shared_ptr<Package>
Support_package::connect(std::vector<std::shared_ptr<Package>> packages) {
  std::vector<std::string> file_names;
  for (auto &el : packages) {
    file_names.push_back(el->get_file_name());
  }

  std::string new_name = get_connected_name(file_names);
  std::shared_ptr<Package> result = packages[0]->clone();
  result->set_file_name(new_name);
  for (auto &el : packages) {
    for (auto req : el->get_connected_packages()) {
      el->insert_connected(req);
    }
  }
  return result;
}