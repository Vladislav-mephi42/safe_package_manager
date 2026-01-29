#include "package_manager/package_manager.h"
#include "map/map.h"
#include "package/package.h"
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>

enum Colour { white, grey, black };

void Package_manager::add(std::shared_ptr<Package> new_package,
                          bool main_flag) {
  const auto it = map.find(new_package->get_file_name());
  if (it != map.cend()) {
    if (!((*it).second)->get_using_flag() && main_flag) {
      ((*it).second)->set_using_flag(true);
    } else if (!main_flag) {
      return;
    } else {
      throw std::runtime_error("This package is already exist");
    }
  }
  connect_equal_pointers(new_package);
  cycle_check(new_package);

  new_package->add();

  new_package->set_using_flag(main_flag);

  map.emplace(new_package->get_file_name(), new_package);
  for (std::shared_ptr<Package> req : new_package->get_connected_packages()) {
    add(req, false);
  }
}

void Package_manager::connect_equal_pointers(std::shared_ptr<Package> package) {
  cycle_check(package);

  auto req_packages = package->get_connected_packages();
  for (std::shared_ptr<Package> req_package_ptr : req_packages) {
    auto map_it =
        std::ranges::find_if(map, [req_package_ptr](const auto &map_value) {
          return (*(map_value.second)) == (*req_package_ptr);
        });
    if (map_it != map.end()) {
      package->erase_connected(*req_package_ptr);
      package->insert_connected((*map_it).second);
    } else {
      connect_equal_pointers(req_package_ptr);
    }
  }
}
void DFS_visit(
    std::shared_ptr<Package> &u,
    std::unordered_map<std::string, Colour> &colour,
    std::unordered_map<std::string, std::shared_ptr<Package>> &pred,
    std::unordered_map<std::string, std::pair<unsigned, unsigned>> &time_stamp,
    unsigned &time) {
  const std::string &name = u->get_file_name();
  colour[name] = Colour::grey;
  time_stamp[name].first = ++time;

  for (auto &v : u->get_connected_packages()) {
    const std::string &v_name = v->get_file_name();

    if (colour[v_name] == Colour::white) {
      pred[v_name] = u;
      DFS_visit(v, colour, pred, time_stamp, time);
    } else if (colour[v_name] == Colour::grey) {
      throw std::runtime_error("cycle found");
    }
  }

  colour[name] = Colour::black;
  time_stamp[name].second = ++time;
}

void Package_manager::cycle_check(const std::shared_ptr<Package> package) {
  std::vector<std::shared_ptr<Package>> Adj_u =
      package->get_connected_packages();
  std::unordered_map<std::string, Colour> colour;
  std::unordered_map<std::string, std::shared_ptr<Package>> pred;
  std::unordered_map<std::string, std::pair<unsigned, unsigned>> time_stamp;
  for (std::shared_ptr<Package> el : Adj_u) {
    colour.emplace(el->get_file_name(), Colour::white);
    pred.emplace(el->get_file_name(), nullptr);
  }
  unsigned time = 0;
  for (std::shared_ptr<Package> el : Adj_u) {

    if (colour[el->get_file_name()] == Colour::white) {
      DFS_visit(el, colour, pred, time_stamp, time);
    }
  };
}

void Package_manager::remove(std::shared_ptr<Package> package) {
  remove(package->get_file_name());
}

bool Thread_visit(
    std::shared_ptr<Package> u,
    std::unordered_map<std::string, std::atomic<Colour>> &colour) {

  colour[u->get_file_name()].store(Colour::grey, std::memory_order_relaxed);

  for (const auto &value : u->get_connected_packages()) {
    Colour current_colour =
        colour[value->get_file_name()].load(std::memory_order_relaxed);
    if (current_colour == Colour::white) {
      Thread_visit(value, colour);
    }
  }

  colour[u->get_file_name()].store(Colour::black, std::memory_order_relaxed);
  return true;
}

void visit(std::unordered_map<std::string, std::atomic<Colour>> &colour,
           std::shared_ptr<Package> package,
           Map<std::string, std::shared_ptr<Package>> &map) {
  if (colour[package->get_file_name()].load(std::memory_order_relaxed) !=
      Colour::white) {
    return;
  }

  for (auto elem : package->get_connected_packages()) {
    if (colour[elem->get_file_name()].load(std::memory_order_relaxed) ==
            Colour::white &&
        !(elem->get_using_flag())) {
      visit(colour, elem, map);
    }
  }

  map.erase(package->get_file_name());
}

void Package_manager::remove(const std::string &package_name) {
  auto it = map.find(package_name);
  if (it == map.end()) {
    throw std::runtime_error("This package did not exist");
  }

  std::unordered_map<std::string, std::atomic<Colour>> colour;
  for (const auto &[name, pkg] : map) {
    colour.emplace(name, Colour::white);
  }

  std::vector<std::thread> threads;

  for (auto &value : map) {
    auto package = value.second;
    if (package->get_using_flag() && value.first != package_name) {
      colour[value.first].store(Colour::black, std::memory_order_relaxed);

      for (const auto &dep : package->get_connected_packages()) {
        if (colour[dep->get_file_name()].load(std::memory_order_relaxed) ==
            Colour::white) {
          threads.emplace_back(Thread_visit, dep, std::ref(colour));
        }
      }
    }
  }

  for (auto &t : threads) {
    t.join();
  }

  if (colour[package_name].load(std::memory_order_relaxed) == Colour::white) {
    visit(colour, it->second, map);
  } else {
    throw std::runtime_error("this package is using by other");
  }
}

void Package_manager::remove_unuse() {
  std::unordered_map<std::string, std::atomic<Colour>> colour;
  for (auto &value : map) {
    colour.emplace(value.first, Colour::white);
  }

  size_t optimal_threads = std::thread::hardware_concurrency();
  if (optimal_threads == 0)
    optimal_threads = 4;

  std::vector<std::thread> threads;
  std::vector<std::pair<std::string, std::shared_ptr<Package>>> tasks;

  for (auto &value : map) {
    auto package = value.second;

    if (package->get_using_flag()) {
      colour[value.first].store(Colour::black, std::memory_order_relaxed);
      tasks.push_back({value.first, package});
    }
  }

  for (size_t i = 0; i < tasks.size(); i += optimal_threads) {
    threads.clear();

    for (size_t j = i; j < std::min(i + optimal_threads, tasks.size()); ++j) {

      threads.emplace_back(Thread_visit, tasks[j].second, std::ref(colour));
    }

    for (auto &t : threads) {
      t.join();
    }
  }

  for (auto it = colour.begin(); it != colour.end(); ++it) {
    if (it->second.load(std::memory_order_relaxed) == Colour::white) {
      map.erase(it->first);
    }
  }
}

void Package_manager::remove_unuse_one_thread() {

  std::unordered_map<std::string, Colour> colour;
  std::unordered_map<std::string, std::shared_ptr<Package>> pred;
  std::unordered_map<std::string, std::pair<unsigned, unsigned>> time_stamp;
  for (std::pair<const std::string, std::shared_ptr<Package>> value : map) {
    auto el = value.second;
    colour.emplace(el->get_file_name(), Colour::white);
    pred.emplace(el->get_file_name(), nullptr);
  }
  unsigned time = 0;
  for (auto &value : map) {
    auto Adj_u = (value.second)->get_connected_packages();
    if ((value.second)->get_using_flag()) {
      colour[value.first] = Colour::black;
      for (std::shared_ptr<Package> el : Adj_u) {
        if (colour[el->get_file_name()] == Colour::white) {
          DFS_visit(el, colour, pred, time_stamp, time);
        }
      }
    }
  }
  std::string delete_key;
  for (auto &value : colour) {
    if (value.second == Colour::white) {
      map.erase(value.first);
    }
  }
}
bool DFS_visit_destroy(
    std::shared_ptr<Package> &u,
    std::unordered_map<std::string, Colour> &colour,
    std::unordered_map<std::string, std::shared_ptr<Package>> &pred,
    std::unordered_map<std::string, std::pair<unsigned, unsigned>> &time_stamp,
    unsigned &time) {
  colour[u->get_file_name()] = Colour::grey;
  time++;
  (time_stamp[u->get_file_name()]).first = time;
  for (std::shared_ptr<Package> v : u->get_connected_packages()) {
    if (colour[v->get_file_name()] == Colour::white) {
      pred[v->get_file_name()] = u;
      DFS_visit_destroy(v, colour, pred, time_stamp, time);
    }
    if (colour[v->get_file_name()] == Colour::grey) {
      u->erase_connected(*v);
      return false;
    }
    colour[u->get_file_name()] = Colour::black;
    time++;
    time_stamp[u->get_file_name()].second = time;
  }
  return true;
}

bool Package_manager::cycle_destroy(const std::shared_ptr<Package> package) {
  std::vector<std::shared_ptr<Package>> Adj_u =
      package->get_connected_packages();
  std::unordered_map<std::string, Colour> colour;
  std::unordered_map<std::string, std::shared_ptr<Package>> pred;
  std::unordered_map<std::string, std::pair<unsigned, unsigned>> time_stamp;
  for (std::shared_ptr<Package> el : Adj_u) {
    colour.emplace(el->get_file_name(), Colour::white);
    pred.emplace(el->get_file_name(), nullptr);
  }
  unsigned time = 0;
  for (std::shared_ptr<Package> el : Adj_u) {

    if (colour[el->get_file_name()] == Colour::white) {
      bool res = DFS_visit_destroy(el, colour, pred, time_stamp, time);
      while (res == false) {
        res = DFS_visit_destroy(el, colour, pred, time_stamp, time);
      }
    }
  }
  return true;
}

void Package_manager::global_update() {
  for (auto &elem : map) {
    (elem.second)->set_current_version((elem.second)->get_last_version());
  }
}