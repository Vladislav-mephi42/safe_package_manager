#include "controler/controler.h"
#include "package/main_package.h"
#include "package/package.h"
#include "package/support_package.h"
#include "package_manager/package_manager.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

int calc_count(int root_count, int depth,
               const std::vector<int> &branching_per_level) {
  if (root_count <= 0 || depth < 0) {
    return 0;
  }

  if (depth == 0) {
    return root_count;
  }

  if (static_cast<int>(branching_per_level.size()) < depth) {
    throw std::invalid_argument(
        "branching_per_level must have at least 'depth' elements");
  }

  long long total = 0;
  long long current_level_count = root_count;

  for (int d = 0; d <= depth; ++d) {
    if (current_level_count < 0 || total > 5000) {
      throw std::overflow_error(
          "Total package count exceeds safe limit (e.g., >5000 or overflow)");
    }
    total += current_level_count;

    if (d < depth) {
      int branching = branching_per_level[d];
      if (branching < 0) {
        throw std::invalid_argument("Branching factor must be non-negative");
      }

      if (current_level_count > 5000LL / (branching == 0 ? 1 : branching)) {
        throw std::overflow_error("Level node count would exceed safe limit");
      }
      current_level_count *= branching;
    }
  }

  if (total > 5000) {
    throw std::overflow_error("Total package count exceeds int32/safe limit");
  }

  return static_cast<int>(total);
}

std::vector<std::string> generate_names(int count) {
  std::vector<std::string> names;
  names.reserve(count);
  for (int i = 0; i < count; ++i) {
    names.emplace_back("pkg_" + std::to_string(i) + ".dep");
  }
  return names;
}

int compute_total_packages(int root_count, int depth,
                           const std::vector<int> &branching_per_level) {
  return calc_count(root_count, depth, branching_per_level);
}

void build_manager(Package_manager &pm, int root_count, int depth,
                   const std::vector<int> &branching_per_level) {

  int total_packages = calc_count(root_count, depth, branching_per_level);
  auto package_names = generate_names(total_packages);
  int name_index = 0;

  std::vector<std::shared_ptr<Package>> current_level;
  for (int i = 0; i < root_count; ++i) {
    auto root = std::make_shared<Main_package>(
        package_names[name_index++], "vlad", "2.1", "2.1",
        std::vector<std::shared_ptr<Package>>{});
    current_level.push_back(root);
  }

  auto roots = current_level;

  for (int d = 0; d < depth; ++d) {
    int branching = branching_per_level[d];
    std::vector<std::shared_ptr<Package>> next_level;
    for (const auto &parent : current_level) {
      for (int b = 0; b < branching; ++b) {
        auto child = std::make_shared<Support_package>(
            package_names[name_index++], "vlad", "2.1", "2.1",
            std::vector<std::shared_ptr<Package>>{});
        parent->insert_connected(child);
        next_level.push_back(child);
      }
    }
    current_level = std::move(next_level);
  }

  for (auto &root : roots) {
    pm.add(root);
  }
}

int main() {
  Package_manager m_1;
  Package_manager m_2;
  auto start = std::chrono::high_resolution_clock::now();

  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start)
          .count();
  auto duration_1 = duration;
  auto duration_2 = duration;

  std::vector<std::shared_ptr<Package>> unuse_packages(25);
  std::vector<std::string> package_names = {
      "A.dep", "B.dep", "C.dep", "D.dep", "E.dep", "F.dep", "G.dep",
      "H.dep", "I.dep", "J.dep", "K.dep", "L.dep", "M.dep", "N.dep",
      "O.dep", "P.dep", "Q.dep", "R.dep", "S.dep", "T.dep", "U.dep",
      "V.dep", "W.dep", "X.dep", "Y.dep", "Z.dep"};
  std::vector<std::shared_ptr<Package>> empty_vec;
  std::ofstream res_2("res_parallel.txt");

  if (!res_2.is_open()) {
    std::cerr << "Fail backup\n";
  }
  std::ofstream res_1("res_one_thread.txt");

  if (!res_1.is_open()) {
    std::cerr << "Fail backup\n";
  }
  for (int i = 0; i < 25; i++) {

    Support_package tmp(package_names[i], "vlad", "2.1", "2.1", empty_vec);
    unuse_packages[i] = std::make_shared<Support_package>(tmp);
  }
  for (int k = 100; k < 500; k += 100) {

    m_1.clear();
    m_2.clear();
    build_manager(m_1, 12, 1, {k});
    build_manager(m_2, 12, 1, {k});
    for (auto elem : unuse_packages) {
      m_1.add(elem);
      m_2.add(elem);
    }
    int count = calc_count(12, 1, {k});
    res_1 << count << " ";
    res_2 << count << " ";
    std::cout << count << " ";
    start = std::chrono::high_resolution_clock::now();
    m_1.remove_unuse_one_thread();
    end = std::chrono::high_resolution_clock::now();
    duration_1 =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start)
            .count();
    res_1 << duration_1 << "\n";
    std::cout << duration_1 << " one " << "\n";

    start = std::chrono::high_resolution_clock::now();
    m_2.remove_unuse();
    end = std::chrono::high_resolution_clock::now();
    duration_2 =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start)
            .count();
    res_2 << duration_2 << "\n";
    std::cout << duration_2 << "\n";
    std::cout << " K ==== " << duration_2 / duration_1 << "\n";
  }
  res_1.close();
  res_2.close();
}