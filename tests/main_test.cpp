#define CATCH_CONFIG_MAIN
#include "controler/controler.h"
#include "package/empty_package.h"
#include "package/main_package.h"
#include "package/package.h"
#include "package/support_package.h"
#include "package_manager/package_manager.h"
#include <catch2/catch_all.hpp>
#include <cmath>

#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

TEST_CASE("Main_package") {
  SECTION("Constructors") {
    Main_package package;
    REQUIRE_NOTHROW(package = Main_package());
    REQUIRE(package.get_file_name() == "default.dep");
    REQUIRE(package.get_publisher_name() == "default");
    REQUIRE(package.get_current_version() == "default");
    REQUIRE(package.get_last_version() == "default");
    std::string file_name = "python.dep";
    std::string publisher_name = "Rossym";
    std::string current_version = "3.12";
    std::string last_version = "3.12";
    std::vector<std::shared_ptr<Package>> empty_req;
    REQUIRE_NOTHROW(package =
                        Main_package(file_name, publisher_name, current_version,
                                     last_version, empty_req));
    REQUIRE(package.get_file_name() == "python.dep");
    REQUIRE(package.get_publisher_name() == "Rossym");
    REQUIRE(package.get_current_version() == "3.12");
    REQUIRE(package.get_last_version() == "3.12");
    Main_package other = package;
    REQUIRE_NOTHROW(package = Main_package(other));
    REQUIRE(package.get_file_name() == "python.dep");
    REQUIRE(package.get_publisher_name() == "Rossym");
    REQUIRE(package.get_current_version() == "3.12");
    REQUIRE(package.get_last_version() == "3.12");
  }
  SECTION("Operator =") {
    Main_package package;
    std::string file_name = "python.dep";
    std::string publisher_name = "Rossym";
    std::string current_version = "3.12";
    std::string last_version = "3.12";
    std::vector<std::shared_ptr<Package>> empty_req;
    Main_package other(file_name, publisher_name, current_version, last_version,
                       empty_req);
    REQUIRE_NOTHROW(package = other);
    REQUIRE(package.get_file_name() == "python.dep");
    REQUIRE(package.get_publisher_name() == "Rossym");
    REQUIRE(package.get_current_version() == "3.12");
    REQUIRE(package.get_last_version() == "3.12");
  }
  SECTION("Setters") {
    Main_package package;
    REQUIRE_THROWS(package.set_file_name("python"));
    REQUIRE_THROWS(package.set_publisher_name(""));
    REQUIRE_THROWS(package.set_current_version(""));
    REQUIRE_THROWS(package.set_last_version(""));

    REQUIRE_NOTHROW(package.set_file_name("python.dep"));
    REQUIRE_NOTHROW(package.set_publisher_name("Rossym"));
    REQUIRE_NOTHROW(package.set_current_version("3.12"));
    REQUIRE_NOTHROW(package.set_last_version("3.12"));
    REQUIRE(package.get_file_name() == "python.dep");
    REQUIRE(package.get_publisher_name() == "Rossym");
    REQUIRE(package.get_current_version() == "3.12");
    REQUIRE(package.get_last_version() == "3.12");
  }
  SECTION("Insert/erase connected") {
    Main_package package;
    std::string file_name = "python.dep";
    std::string publisher_name = "Rossym";
    std::string current_version = "3.12";
    std::string last_version = "3.12";
    std::vector<std::shared_ptr<Package>> empty_req;
    Main_package other(file_name, publisher_name, current_version, last_version,
                       empty_req);
    REQUIRE_NOTHROW(
        package.insert_connected(std::make_shared<Main_package>(other)));
    REQUIRE(package.get_connected_packages().size() == 1);
    REQUIRE(*((package.get_connected_packages())[0].get()) == other);
    REQUIRE_NOTHROW(package.erase_connected(other));
    REQUIRE(package.get_connected_packages().size() == 0);
  }
}

TEST_CASE("Support_package") {
  SECTION("Constructors") {
    Support_package package;
    REQUIRE_NOTHROW(package = Support_package());
    REQUIRE(package.get_file_name() == "default.dep");
    REQUIRE(package.get_publisher_name() == "default");
    REQUIRE(package.get_current_version() == "default");
    REQUIRE(package.get_last_version() == "default");
    std::string file_name = "python.dep";
    std::string publisher_name = "Rossym";
    std::string current_version = "3.12";
    std::string last_version = "3.12";
    std::vector<std::shared_ptr<Package>> empty_req;
    REQUIRE_NOTHROW(package = Support_package(file_name, publisher_name,
                                              current_version, last_version,
                                              empty_req));
    REQUIRE(package.get_file_name() == "python.dep");
    REQUIRE(package.get_publisher_name() == "Rossym");
    REQUIRE(package.get_current_version() == "3.12");
    REQUIRE(package.get_last_version() == "3.12");
    Support_package other = package;
    REQUIRE_NOTHROW(package = Support_package(other));
    REQUIRE(package.get_file_name() == "python.dep");
    REQUIRE(package.get_publisher_name() == "Rossym");
    REQUIRE(package.get_current_version() == "3.12");
    REQUIRE(package.get_last_version() == "3.12");
  }
  SECTION("Operator =") {
    Support_package package;
    std::string file_name = "python.dep";
    std::string publisher_name = "Rossym";
    std::string current_version = "3.12";
    std::string last_version = "3.12";
    std::vector<std::shared_ptr<Package>> empty_req;
    Support_package other(file_name, publisher_name, current_version,
                          last_version, empty_req);
    REQUIRE_NOTHROW(package = other);
    REQUIRE(package.get_file_name() == "python.dep");
    REQUIRE(package.get_publisher_name() == "Rossym");
    REQUIRE(package.get_current_version() == "3.12");
    REQUIRE(package.get_last_version() == "3.12");
  }
  SECTION("Setters") {
    Support_package package;
    REQUIRE_THROWS(package.set_file_name("python"));
    REQUIRE_THROWS(package.set_publisher_name(""));
    REQUIRE_THROWS(package.set_current_version(""));
    REQUIRE_THROWS(package.set_last_version(""));

    REQUIRE_NOTHROW(package.set_file_name("python.dep"));
    REQUIRE_NOTHROW(package.set_publisher_name("Rossym"));
    REQUIRE_NOTHROW(package.set_current_version("3.12"));
    REQUIRE_NOTHROW(package.set_last_version("3.12"));
    REQUIRE(package.get_file_name() == "python.dep");
    REQUIRE(package.get_publisher_name() == "Rossym");
    REQUIRE(package.get_current_version() == "3.12");
    REQUIRE(package.get_last_version() == "3.12");
  }
  SECTION("Insert/erase connected") {
    Support_package package;
    std::string file_name = "python.dep";
    std::string publisher_name = "Rossym";
    std::string current_version = "3.12";
    std::string last_version = "3.12";
    std::vector<std::shared_ptr<Package>> empty_req;
    Support_package other(file_name, publisher_name, current_version,
                          last_version, empty_req);
    REQUIRE_NOTHROW(
        package.insert_connected(std::make_shared<Support_package>(other)));
    REQUIRE(package.get_connected_packages().size() == 1);
    REQUIRE(*((package.get_connected_packages())[0].get()) == other);
    REQUIRE_NOTHROW(package.erase_connected(other));
    REQUIRE(package.get_connected_packages().size() == 0);
  }
}

TEST_CASE("Empty_package") {
  SECTION("Constructors") {
    Empty_package package;
    REQUIRE_NOTHROW(package = Empty_package());

    std::string file_name = "python.dep";
    std::string publisher_name = "Rossym";
    std::string current_version = "3.12";
    std::string last_version = "3.12";
    std::vector<std::shared_ptr<Package>> empty_req;
    Main_package linked_package(file_name, publisher_name, current_version,
                                last_version, empty_req);
    REQUIRE_NOTHROW(
        package = Empty_package(
            "python.dep", std::make_shared<Main_package>(linked_package)));
    REQUIRE(package.get_file_name() == "python.dep");
    REQUIRE(package.get_publisher_name() == "Rossym");
    REQUIRE(package.get_current_version() == "3.12");
    REQUIRE(package.get_last_version() == "3.12");
    Empty_package other = package;
    REQUIRE_NOTHROW(package = Empty_package(other));
    REQUIRE(package.get_file_name() == "python.dep");
    REQUIRE(package.get_publisher_name() == "Rossym");
    REQUIRE(package.get_current_version() == "3.12");
    REQUIRE(package.get_last_version() == "3.12");
  }
  SECTION("Operator =") {
    Empty_package package;
    std::string file_name = "python.dep";
    std::string publisher_name = "Rossym";
    std::string current_version = "3.12";
    std::string last_version = "3.12";
    std::vector<std::shared_ptr<Package>> empty_req;
    Main_package linked_package(file_name, publisher_name, current_version,
                                last_version, empty_req);
    Empty_package other(file_name,
                        std::make_shared<Main_package>(linked_package));
    REQUIRE_NOTHROW(package = other);
    REQUIRE(package.get_file_name() == "python.dep");
    REQUIRE(package.get_publisher_name() == "Rossym");
    REQUIRE(package.get_current_version() == "3.12");
    REQUIRE(package.get_last_version() == "3.12");
  }
  SECTION("Setters") {
    Empty_package package;
    REQUIRE_NOTHROW(package.set_file_name("python.dep"));
    REQUIRE_THROWS(package.set_publisher_name("Rossym"));
    REQUIRE_THROWS(package.set_current_version("3.12"));
    REQUIRE_THROWS(package.set_last_version("3.12"));
    Main_package main_package;
    package = Empty_package("default.dep",
                            std::make_shared<Main_package>(main_package));
    REQUIRE_NOTHROW(package.set_file_name("python.dep"));
    REQUIRE_NOTHROW(package.set_publisher_name("Rossym"));
    REQUIRE_NOTHROW(package.set_current_version("3.12"));
    REQUIRE_NOTHROW(package.set_last_version("3.12"));
    REQUIRE(package.get_file_name() == "python.dep");
    REQUIRE(package.get_publisher_name() == "Rossym");
    REQUIRE(package.get_current_version() == "3.12");
    REQUIRE(package.get_last_version() == "3.12");
  }
  SECTION("Insert/erase connected") {
    Empty_package package;
    std::string file_name = "python.dep";
    std::string publisher_name = "Rossym";
    std::string current_version = "3.12";
    std::string last_version = "3.12";
    std::vector<std::shared_ptr<Package>> empty_req;
    Main_package main_other(file_name, publisher_name, current_version,
                            last_version, empty_req);
    Empty_package other = Empty_package(
        "default.dep", std::make_shared<Main_package>(main_other));
    REQUIRE_THROWS(
        package.insert_connected(std::make_shared<Empty_package>(other)));
    REQUIRE_THROWS(package.get_connected_packages());
    REQUIRE_THROWS(package.erase_connected(other));

    Main_package main_package;
    package = Empty_package("default.dep",
                            std::make_shared<Main_package>(main_package));
    REQUIRE_NOTHROW(
        package.insert_connected(std::make_shared<Empty_package>(other)));
    REQUIRE(package.get_connected_packages().size() == 1);
    REQUIRE(*((package.get_connected_packages())[0].get()) == other);
    REQUIRE_NOTHROW(package.erase_connected(other));
    REQUIRE(package.get_connected_packages().size() == 0);
  }
}

std::vector<std::string> generate_names(int count) {
  std::vector<std::string> names;
  names.reserve(count);
  for (int i = 0; i < count; ++i) {
    names.emplace_back("pkg_" + std::to_string(i) + ".dep");
  }
  return names;
}

int compute_total_packages(int root_count, int depth, int branching) {
  int level_nodes = root_count;
  int total = 0;
  for (int d = 0; d <= depth; ++d) {
    total += level_nodes;
    level_nodes *= branching;
  }
  return total;
}

void build_manager_ms(Package_manager &pm, int root_count, int depth,
                      int branching) {
  int total_packages = compute_total_packages(
      root_count, depth, branching); // ms - root nodes - Main_package
                                     //  other nodes - Support_package
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

void build_manager_mm(Package_manager &pm, int root_count, int depth,
                      int branching) {
  int total_packages = compute_total_packages(
      root_count, depth, branching); // mm - root nodes - Main_package
                                     //  other nodes - Main_package
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
    std::vector<std::shared_ptr<Package>> next_level;
    for (const auto &parent : current_level) {
      for (int b = 0; b < branching; ++b) {
        auto child = std::make_shared<Main_package>(
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

void build_manager_me(Package_manager &pm, int root_count, int depth,
                      int branching) {
  int total_packages = compute_total_packages(
      root_count, depth, branching); // me - root nodes - Main_package
                                     //  other nodes - Empty_package
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
    std::vector<std::shared_ptr<Package>> next_level;
    for (const auto &parent : current_level) {
      for (int b = 0; b < branching; ++b) {
        auto child_main = std::make_shared<Main_package>(
            Main_package(package_names[name_index++], "vlad", "2.1", "2.1",
                         std::vector<std::shared_ptr<Package>>{}));
        auto child = std::make_shared<Empty_package>(
            child_main->get_file_name(), child_main);
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

std::vector<std::string> package_names = {
    "coreutils.dep",           "networking.dep",      "crypto.lib.dep",
    "ui.framework.dep",        "logging.sys.dep",     "parser.yaml.dep",
    "compression.zlib.dep",    "graphics.opengl.dep", "database.sqlite.dep",
    "security.tls.dep",        "filesystem.vfs.dep",  "config.json.dep",
    "math.linalg.dep",         "audio.alc.dep",       "render.vulkan.dep",
    "testing.gtest.dep",       "io.async.dep",        "shell.bash.dep",
    "ipc.message.dep",         "utils.hash.dep",      "net.http.dep",
    "script.lua.dep",          "storage.nvme.dep",    "debug.gdb.dep",
    "profiler.perf.dep",       "driver.usb.dep",      "kernel.syscall.dep",
    "memory.alloc.dep",        "scheduler.rt.dep",    "sensor.gpio.dep",
    "serial.uart.dep",         "crypto.openssl.dep",  "ui.qt.dep",
    "web.engine.dep",          "cli.argparse.dep",    "data.protobuf.dep",
    "image.jpeg.dep",          "video.h264.dep",      "lang.cpp.std.dep",
    "lang.python.runtime.dep", "vm.jit.dep",          "sandbox.seccomp.dep",
    "auth.oauth.dep",          "event.loop.dep",      "cache.lru.dep",
    "format.csv.dep",          "time.ntp.dep",        "fs.ext4.dep",
    "net.bluetooth.dep",       "hw.riscv.dep"};

TEST_CASE("Package manager (Basic methods)") {
  SECTION("Constructors") {
    Package_manager pm;
    REQUIRE(pm.size() == 0);
    REQUIRE_NOTHROW(pm.clear());
    REQUIRE(pm.size() == 0);
    std::vector<std::shared_ptr<Package>> empty;
    REQUIRE_NOTHROW(pm = Package_manager(empty));
    REQUIRE(pm.size() == 0);
  }
  SECTION("Add (Basic)") {
    std::vector<std::shared_ptr<Package>> empty;
    Main_package pkg_1(package_names[0], "batman", "123456", "12344", empty);
    Support_package pkg_2(package_names[1], "batman", "123456", "12344", empty);
    Main_package tmp_pkg(package_names[2], "batman", "123456", "12344", empty);
    Empty_package pkg_3(package_names[2],
                        std::make_shared<Main_package>(tmp_pkg));
    pkg_2.insert_connected(std::make_shared<Empty_package>(pkg_3));
    pkg_1.insert_connected(std::make_shared<Support_package>(pkg_2));
    Package_manager pm;
    REQUIRE_NOTHROW(pm.add(std::make_shared<Support_package>(pkg_2)));
    REQUIRE(pm.size() == 2);
    REQUIRE_NOTHROW(pm.add(std::make_shared<Main_package>(pkg_1)));
    REQUIRE(pm.size() == 3);
    pm = Package_manager();
    Package_manager pm_2;
    REQUIRE(pkg_2.get_connected_packages().size() == 1);
    REQUIRE_NOTHROW(pm_2.add(std::make_shared<Main_package>(pkg_1)));
    REQUIRE(pm_2.size() == 3);
  }
  SECTION("Add (two same packages with different other atributes)") {
    std::vector<std::shared_ptr<Package>> empty;
    Main_package pkg_1(package_names[0], "batman", "123456", "12344", empty);
    Support_package pkg_2(package_names[1], "batman", "123456", "12344", empty);
    Main_package tmp_pkg(package_names[2], "batman", "123456", "12344", empty);
    Empty_package pkg_3(package_names[2],
                        std::make_shared<Main_package>(tmp_pkg));
    pkg_2.insert_connected(std::make_shared<Empty_package>(pkg_3));
    pkg_1.insert_connected(std::make_shared<Support_package>(pkg_2));
    Package_manager pm;
    pm.add(std::make_shared<Main_package>(pkg_1));
    REQUIRE(pm.size() == 3);
    pkg_2.set_publisher_name("joker");
    REQUIRE_THROWS(pm.add(std::make_shared<Support_package>(pkg_2)));
  }

  SECTION("Add (cycle found)") {
    std::vector<std::shared_ptr<Package>> empty;
    Main_package pkg_1(package_names[0], "batman", "123456", "12344", empty);
    Support_package pkg_2(package_names[1], "batman", "123456", "12344", empty);
    Main_package tmp_pkg(package_names[2], "batman", "123456", "12344", empty);
    Empty_package pkg_3(package_names[2],
                        std::make_shared<Main_package>(tmp_pkg));
    pkg_3.insert_connected(std::make_shared<Main_package>(pkg_1));
    pkg_2.insert_connected(std::make_shared<Empty_package>(pkg_3));
    pkg_1.insert_connected(std::make_shared<Support_package>(pkg_2));
    Package_manager pm;
    REQUIRE_THROWS(pm.add(std::make_shared<Main_package>(pkg_1)));
  }

  SECTION("Add (the same package)") {
    std::vector<std::shared_ptr<Package>> empty;
    Main_package pkg_1(package_names[0], "batman", "123456", "12344", empty);
    Package_manager pm;
    REQUIRE_NOTHROW(pm.add(std::make_shared<Main_package>(pkg_1)));
    REQUIRE_THROWS(pm.add(std::make_shared<Main_package>(pkg_1)));
  }

  SECTION("Remove (basic)") {
    std::vector<std::shared_ptr<Package>> empty;
    Main_package pkg_1(package_names[0], "batman", "123456", "12344", empty);
    Package_manager pm;
    REQUIRE_NOTHROW(pm.add(std::make_shared<Main_package>(pkg_1)));
    REQUIRE(pm.size() == 1);
    REQUIRE_NOTHROW(pm.remove(pkg_1.get_file_name()));
    REQUIRE(pm.size() == 0);
  }

  SECTION("Remove (basic) overload") {
    std::vector<std::shared_ptr<Package>> empty;
    Main_package pkg_1(package_names[0], "batman", "123456", "12344", empty);
    Package_manager pm;
    REQUIRE_NOTHROW(pm.add(std::make_shared<Main_package>(pkg_1)));
    REQUIRE(pm.size() == 1);
    REQUIRE_NOTHROW(pm.remove(std::make_shared<Main_package>(pkg_1)));
    REQUIRE(pm.size() == 0);
  }

  SECTION("Remove (chain of packages)") {
    std::vector<std::shared_ptr<Package>> empty;
    Main_package pkg_1(package_names[0], "batman", "123456", "12344", empty);
    Support_package pkg_2(package_names[1], "batman", "123456", "12344", empty);
    Main_package tmp_pkg(package_names[2], "batman", "123456", "12344", empty);
    Empty_package pkg_3(package_names[2],
                        std::make_shared<Main_package>(tmp_pkg));
    pkg_2.insert_connected(std::make_shared<Empty_package>(pkg_3));
    pkg_1.insert_connected(std::make_shared<Support_package>(pkg_2));
    Package_manager pm;

    REQUIRE_NOTHROW(pm.add(std::make_shared<Main_package>(pkg_1)));
    REQUIRE_NOTHROW(pm.remove(pkg_1.get_file_name()));
    REQUIRE(pm.size() == 0);
  }

  SECTION("Remove (support package in use)") {
    std::vector<std::shared_ptr<Package>> empty;
    Main_package pkg_1(package_names[0], "batman", "123456", "12344", empty);
    Support_package pkg_2(package_names[1], "batman", "123456", "12344", empty);
    Main_package tmp_pkg(package_names[2], "batman", "123456", "12344", empty);
    Empty_package pkg_3(package_names[2],
                        std::make_shared<Main_package>(tmp_pkg));
    pkg_2.insert_connected(std::make_shared<Empty_package>(pkg_3));
    pkg_1.insert_connected(std::make_shared<Support_package>(pkg_2));
    Package_manager pm;

    REQUIRE_NOTHROW(pm.add(std::make_shared<Main_package>(pkg_1)));
    REQUIRE_THROWS(pm.remove(pkg_2.get_file_name()));
    REQUIRE(pm.size() == 3);
    REQUIRE_THROWS(pm.remove(pkg_3.get_file_name()));
    REQUIRE(pm.size() == 3);
  }

  SECTION("Big parallel adding/removing") {
    Package_manager pm;
    for (int root_count = 1; root_count < 4; root_count += 2) {

      REQUIRE_NOTHROW(build_manager_mm(pm, root_count, 3, 2));
      REQUIRE(pm.size() == compute_total_packages(root_count, 3, 2));
      std::vector<std::string> package_names = generate_names(pm.size());
      for (int i = 0; i < root_count; i++) {
        pm.remove(package_names[i]);
      }
      REQUIRE(pm.size() == 0);
      REQUIRE_NOTHROW(build_manager_ms(pm, root_count, 3, 2));
      REQUIRE(pm.size() == compute_total_packages(root_count, 3, 2));
      for (int i = 0; i < root_count; i++) {
        pm.remove(package_names[i]);
      }
      REQUIRE(pm.size() == 0);
      REQUIRE_NOTHROW(build_manager_me(pm, root_count, 3, 2));
      REQUIRE(pm.size() == compute_total_packages(root_count, 3, 2));
      for (int i = 0; i < root_count; i++) {
        pm.remove(package_names[i]);
      }
      REQUIRE(pm.size() == 0);
    }
  }
  SECTION("find") {
    std::vector<std::shared_ptr<Package>> empty;
    Main_package pkg_1(package_names[0], "batman", "123456", "12344", empty);
    Support_package pkg_2(package_names[1], "batman", "123456", "12344", empty);
    Main_package tmp_pkg(package_names[2], "batman", "123456", "12344", empty);
    Empty_package pkg_3(package_names[2],
                        std::make_shared<Main_package>(tmp_pkg));
    pkg_2.insert_connected(std::make_shared<Empty_package>(pkg_3));
    pkg_1.insert_connected(std::make_shared<Support_package>(pkg_2));
    Package_manager pm;
    REQUIRE_NOTHROW(pm.add(std::make_shared<Support_package>(pkg_2)));
    std::stringstream pkg_stream;
    std::string pkg_str;
    REQUIRE(pm.find(pkg_1.get_file_name()) == false);
    REQUIRE(pm.find(pkg_2.get_file_name()) == true);
    REQUIRE(pm.find(pkg_1.get_file_name(), pkg_stream) == false);
    REQUIRE(pm.find(pkg_2.get_file_name(), pkg_stream) == true);
    pkg_str = pkg_stream.str();
    REQUIRE(pkg_str.find(pkg_2.get_file_name()) != std::string::npos);
    REQUIRE(pm.find(pkg_3.get_file_name(), pkg_stream) == true);
    pkg_str = pkg_stream.str();
    REQUIRE(pkg_str.find(pkg_3.get_file_name()) != std::string::npos);
    REQUIRE_NOTHROW(pm.add(std::make_shared<Main_package>(pkg_1)));

    pm = Package_manager();
    Package_manager pm_2;
    REQUIRE(pkg_2.get_connected_packages().size() == 1);
    REQUIRE_NOTHROW(pm_2.add(std::make_shared<Main_package>(pkg_1)));
    REQUIRE(pm_2.size() == 3);
  }

  SECTION("cycle destroy") {
    std::vector<std::shared_ptr<Package>> empty;
    Main_package pkg_1(package_names[0], "batman", "123456", "12344", empty);
    Support_package pkg_2(package_names[1], "batman", "123456", "12344", empty);
    Main_package tmp_pkg(package_names[2], "batman", "123456", "12344", empty);
    Empty_package pkg_3(package_names[2],
                        std::make_shared<Main_package>(tmp_pkg));
    pkg_3.insert_connected(std::make_shared<Main_package>(pkg_1));
    pkg_2.insert_connected(std::make_shared<Empty_package>(pkg_3));
    pkg_1.insert_connected(std::make_shared<Support_package>(pkg_2));
    auto ptr_pkg_1 = std::make_shared<Main_package>(pkg_1);
    Package_manager pm;
    REQUIRE_THROWS(pm.add(ptr_pkg_1, false));
    REQUIRE_NOTHROW(pm.cycle_destroy(ptr_pkg_1));
  }

  SECTION("remove unuse") {
    Package_manager pm;

    REQUIRE_NOTHROW(build_manager_mm(pm, 3, 3, 2));
    REQUIRE(pm.size() == compute_total_packages(3, 3, 2));

    Support_package pkg;
    std::vector<std::shared_ptr<Package>> empty;
    for (int i = 0; i < 20; i++) {
      pkg = Support_package(package_names[i], "batman", "3.12", "3.12", empty);
      pm.add(std::make_shared<Support_package>(pkg));
    }
    REQUIRE(pm.size() == (compute_total_packages(3, 3, 2)) + 20);
    REQUIRE_NOTHROW(pm.remove_unuse());
    REQUIRE(pm.size() == compute_total_packages(3, 3, 2));
  }
}

TEST_CASE("Package  manager (advanced checks)") {
  SECTION("Removing package that is used by several other") {
    std::vector<std::shared_ptr<Package>> empty;
    auto pkg_1 = std::make_shared<Main_package>(
        Main_package(package_names[0], "batman", "123456", "12344", empty));
    auto pkg_2 = std::make_shared<Main_package>(
        Main_package(package_names[1], "batman", "123456", "12344", empty));
    auto tmp_pkg =
        Main_package(package_names[2], "batman", "123456", "12344", empty);
    auto pkg_3 = std::make_shared<Empty_package>(Empty_package(
        package_names[2], std::make_shared<Main_package>(tmp_pkg)));
    auto pkg_4 = std::make_shared<Empty_package>(Empty_package(
        package_names[3], std::make_shared<Main_package>(tmp_pkg)));
    pkg_1->insert_connected(pkg_3);
    pkg_1->insert_connected(pkg_4);
    pkg_2->insert_connected(pkg_3);
    Package_manager pm;
    REQUIRE_NOTHROW(pm.add(pkg_1));
    REQUIRE_NOTHROW(pm.add(pkg_2));
    REQUIRE(pm.size() == 4);
    REQUIRE_NOTHROW(pm.remove(pkg_1));
    REQUIRE(pm.size() == 2);
    REQUIRE_NOTHROW(pm.remove(pkg_2));
    REQUIRE(pm.size() == 0);
  }
  SECTION("Same packages with different configuration error(1)") {
    std::vector<std::shared_ptr<Package>> empty;
    Main_package pkg_1(package_names[0], "batman", "123456", "12344", empty);
    Support_package pkg_2(package_names[0], "batman", "123456", "12344", empty);
    Package_manager pm;
    REQUIRE_NOTHROW(pm.add(std::make_shared<Main_package>(pkg_1)));
    REQUIRE_THROWS(pm.add(std::make_shared<Support_package>(pkg_2)));
    REQUIRE(pm.size() == 1);
  }
  SECTION("Same packages with different configuration error(2)") {
    std::vector<std::shared_ptr<Package>> empty;
    Main_package pkg_1(package_names[0], "batman", "123456", "12344", empty);
    Main_package pkg_2(package_names[0], "joker", "123456", "12344", empty);
    Package_manager pm;
    REQUIRE_NOTHROW(pm.add(std::make_shared<Main_package>(pkg_1)));
    REQUIRE_THROWS(pm.add(std::make_shared<Main_package>(pkg_2)));
    REQUIRE(pm.size() == 1);
  }
}

TEST_CASE("Controler") {
#ifdef SKIP_THIS_BLOCK
  SECTION("Packages methods that are needed to controler") {
    std::vector<std::shared_ptr<Package>> empty;
    Main_package pkg(package_names[0], "batman", "123456", "12344", empty);
    Main_package other;
    std::stringstream stream;
    pkg.write(stream);
    other.read(stream);
    bool flag = (pkg == other); // fix catch error
    REQUIRE(flag);
  }
#endif

  SECTION("read package from json(Main package)") {
    json data;
    data["packages"] = json::array();
    std::vector<std::shared_ptr<Package>> empty;
    Main_package pkg(package_names[0], "batman", "123456", "12344", empty);
    Main_package req_pkg_1(package_names[1], "batman", "123456", "12344",
                           empty);
    Main_package req_pkg_2(package_names[2], "batman", "123456", "12344",
                           empty);
    pkg.insert_connected(std::make_shared<Main_package>(req_pkg_1));
    pkg.insert_connected(std::make_shared<Main_package>(req_pkg_2));
    Main_package other(package_names[3], "batman", "123456", "12344", empty);

    std::stringstream out_1;
    json tmp_1;
    pkg.write(out_1);
    tmp_1 = json::parse(out_1.str());
    data["packages"].push_back(tmp_1);
    std::stringstream out_2;

    for (const auto &elem : pkg.get_connected_packages()) {
      std::stringstream out_1;
      json tmp_1;
      elem->write(out_1);
      tmp_1 = json::parse(out_1.str());
      data["packages"].push_back(tmp_1);
    }
    json tmp_2;
    other.write(out_2);
    tmp_2 = json::parse(out_2.str());
    data["packages"].push_back(tmp_2);
    Controler controler;
    auto new_pkg = controler.read_package(package_names[0], data);
    auto old_pkg = std::make_shared<Main_package>(pkg);
    auto new_req = new_pkg->get_connected_packages();

    auto old_req = old_pkg->get_connected_packages();

    bool check = (*(new_pkg.get()) == pkg);
    REQUIRE(check);
  }

  SECTION("read package from json(Support package)") {
    json data;
    data["packages"] = json::array();
    std::vector<std::shared_ptr<Package>> empty;
    Support_package pkg(package_names[0], "batman", "123456", "12344", empty);
    Support_package other(package_names[1], "batman", "123456", "12344", empty);
    std::stringstream out_1;
    json tmp_1;
    pkg.write(out_1);
    tmp_1 = json::parse(out_1.str());
    data["packages"].push_back(tmp_1);
    std::stringstream out_2;
    json tmp_2;
    other.write(out_2);
    tmp_2 = json::parse(out_2.str());
    data["packages"].push_back(tmp_2);
    Controler controler;
    auto new_pkg = controler.read_package(package_names[0], data);
    auto old_pkg = std::make_shared<Support_package>(pkg);
    bool check = (*(new_pkg.get()) == pkg);
    REQUIRE(check);
  }
  SECTION("read package from json(Empty package)") {
    json data;
    data["packages"] = json::array();
    std::vector<std::shared_ptr<Package>> empty;
    Main_package main_pkg(package_names[0], "batman", "123456", "12344", empty);
    Main_package main_other(package_names[1], "batman", "123456", "12344",
                            empty);
    Empty_package pkg(package_names[0],
                      std::make_shared<Main_package>(main_pkg));
    Empty_package other(package_names[1],
                        std::make_shared<Main_package>(main_other));
    std::stringstream out_1;
    json tmp_1;
    pkg.write(out_1);
    tmp_1 = json::parse(out_1.str());
    data["packages"].push_back(tmp_1);
    std::stringstream out_2;
    json tmp_2;
    other.write(out_2);
    tmp_2 = json::parse(out_2.str());
    data["packages"].push_back(tmp_2);
    Controler controler;
    auto new_pkg = controler.read_package(package_names[0], data);
    auto old_pkg = std::make_shared<Empty_package>(pkg);
    bool check = (*(new_pkg.get()) == pkg);
    REQUIRE(check);
  }
  SECTION("Write/read package manager to file MM") {
    Package_manager pm_1;
    Package_manager pm_2;
    build_manager_mm(pm_1, 3, 3, 2);

    Controler controler;
    controler.write_package_manager_to_file("test_file_1.json", pm_1);
    controler.read_package_manager_from_file("test_file_1.json", pm_2);
    REQUIRE(pm_1.size() == pm_2.size());
    int total_packages = compute_total_packages(3, 3, 2);
    auto package_names = generate_names(total_packages);
    for (const auto &elem : package_names) {
      REQUIRE(pm_1.find(elem));
      REQUIRE(pm_2.find(elem));
    }
  }
  SECTION("Write/read package manager to file MS") {
    Package_manager pm_1;
    Package_manager pm_2;
    build_manager_ms(pm_1, 3, 3, 2);

    Controler controler;
    controler.write_package_manager_to_file("test_file_1.json", pm_1);
    controler.read_package_manager_from_file("test_file_1.json", pm_2);
    REQUIRE(pm_1.size() == pm_2.size());
    int total_packages = compute_total_packages(3, 3, 2);
    auto package_names = generate_names(total_packages);
    for (const auto &elem : package_names) {
      REQUIRE(pm_1.find(elem));
      REQUIRE(pm_2.find(elem));
    }
  }
  SECTION("Write/read package manager to file MS") {
    Package_manager pm_1;
    Package_manager pm_2;
    build_manager_me(pm_1, 3, 3, 2);

    Controler controler;
    controler.write_package_manager_to_file("test_file_1.json", pm_1);
    controler.read_package_manager_from_file("test_file_1.json", pm_2);
    REQUIRE(pm_1.size() == pm_2.size());
    int total_packages = compute_total_packages(3, 3, 2);
    auto package_names = generate_names(total_packages);
    for (const auto &elem : package_names) {
      REQUIRE(pm_1.find(elem));
      REQUIRE(pm_2.find(elem));
    }
  }
  SECTION("Write/read package to file") {
    json data;
    data["packages"] = json::array();
    std::ofstream file("test_file_2.json");
    file << data;
    file.close();

    Main_package pkg_1(package_names[0], "batman", "123456", "12344", {});
    Support_package pkg_2(package_names[1], "batman", "123456", "12344", {});
    Main_package tmp(package_names[3], "batman", "123456", "12344", {});
    Empty_package pkg_3(package_names[3], std::make_shared<Main_package>(tmp));
    pkg_1.insert_connected(std::make_shared<Support_package>(pkg_2));
    pkg_1.insert_connected(std::make_shared<Empty_package>(pkg_3));
    Controler controler;
    controler.write_package_to_file(std::make_shared<Main_package>(pkg_1),
                                    "test_file_2.json");
    auto result =
        controler.read_package_from_file(package_names[0], "test_file_2.json");
    REQUIRE(*result.get() == pkg_1);
  }
  SECTION("write package to file(cycle error)") {
    json data;
    data["packages"] = json::array();
    std::ofstream file("test_file_2.json");
    file << data;
    file.close();

    Main_package pkg_1_tmp(package_names[0], "batman", "123456", "12344", {});
    Support_package pkg_2_tmp(package_names[1], "batman", "123456", "12344",
                              {});
    Main_package tmp(package_names[3], "batman", "123456", "12344", {});
    Empty_package pkg_3_tmp(package_names[3],
                            std::make_shared<Main_package>(tmp));
    auto pkg_1 = std::make_shared<Main_package>(pkg_1_tmp);
    auto pkg_2 = std::make_shared<Support_package>(pkg_2_tmp);
    auto pkg_3 = std::make_shared<Empty_package>(pkg_3_tmp);
    pkg_2->insert_connected(pkg_1);
    pkg_1->insert_connected(pkg_2);
    pkg_1->insert_connected(pkg_3);
    Controler controler;
    REQUIRE_THROWS(controler.write_package_to_file(pkg_1, "test_file_2.json"));
  }
  SECTION("write package from file(super bad format)") {
    json data;
    data["packages"] = json::array();
    std::ofstream file("test_file_2.json");
    file << data;
    file.close();

    Main_package pkg_1(package_names[0], "batman", "123456", "12344", {});
    Support_package pkg_2(package_names[1], "batman", "123456", "12344", {});
    Main_package tmp(package_names[3], "batman", "123456", "12344", {});
    Empty_package pkg_3(package_names[3], std::make_shared<Main_package>(tmp));
    pkg_2.insert_connected(std::make_shared<Main_package>(pkg_1));
    pkg_1.insert_connected(std::make_shared<Support_package>(pkg_2));
    pkg_1.insert_connected(std::make_shared<Empty_package>(pkg_3));
    Controler controler;
    REQUIRE_THROWS(controler.write_package_to_file(
        std::make_shared<Main_package>(pkg_1), "test_file_2.json"));
  }
}

TEST_CASE("Controler main purpose test") {
  SECTION("Add/Remove") {
    Package_manager pm;
    build_manager_ms(pm, 3, 3, 3);
    REQUIRE(pm.size() == compute_total_packages(3, 3, 3));
    Controler controler;
    REQUIRE_NOTHROW(controler =
                        Controler({"repozitory_1.json", "repozitory_2.json"},
                                  "storage.json", &pm));

    Main_package pkg_1(package_names[0], "batman", "123456", "12344", {});
    Support_package pkg_2(package_names[1], "batman", "123456", "12344", {});
    Main_package tmp(package_names[3], "batman", "123456", "12344", {});
    Empty_package pkg_3(package_names[3], std::make_shared<Main_package>(tmp));
    pkg_1.insert_connected(std::make_shared<Support_package>(pkg_2));
    pkg_1.insert_connected(std::make_shared<Empty_package>(pkg_3));
    std::ofstream file_1("repozitory_1.json");
    std::ofstream file_2("repozitory_2.json");
    json data;
    data["packages"] = json::array();
    file_1 << data;
    file_2 << data;
    file_1.close();
    file_2.close();

    controler.write_package_to_file(std::make_shared<Main_package>(pkg_1),
                                    "repozitory_2.json");
    REQUIRE_NOTHROW(controler.add_package(pkg_2.get_file_name()));
    REQUIRE(pm.size() == compute_total_packages(3, 3, 3) + 1);
    REQUIRE_NOTHROW(controler.add_package(pkg_3.get_file_name()));
    REQUIRE(pm.size() == compute_total_packages(3, 3, 3) + 2);
    REQUIRE_NOTHROW(controler.add_package(pkg_1.get_file_name()));
    REQUIRE(pm.size() == compute_total_packages(3, 3, 3) + 3);
    REQUIRE_THROWS(controler.remove_package(pkg_3.get_file_name()));
    controler.remove_package(pkg_1.get_file_name());
    REQUIRE(pm.size() == compute_total_packages(3, 3, 3) + 1);
    controler.remove_package(pkg_3.get_file_name());
    REQUIRE(pm.size() == compute_total_packages(3, 3, 3));
  }
}