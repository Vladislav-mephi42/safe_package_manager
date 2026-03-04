#define CATCH_CONFIG_MAIN
#include "controler/controler.h"
#include "package/empty_package.h"
#include "package/main_package.h"
#include "package/package.h"
#include "package/support_package.h"
#include "package_manager/package_manager.h"
#include <catch.hpp>
#include <cmath>
#include <format>
#include <fstream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

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

void build_manager(Package_manager &pm, int root_count, int depth,
                   int branching) {
  int total_packages = compute_total_packages(root_count, depth, branching);
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

TEST_CASE("Packages") {

  SECTION("devide") {
    std::vector<std::shared_ptr<Package>> empty;
    Support_package package("sigma.dep", "vlad", "1", "1", empty);
    std::vector<std::shared_ptr<Package>> parts;
    REQUIRE_NOTHROW(parts = package.devide(5));
    for (int i = 0; i < 5; i++) {
      REQUIRE(parts[i]->get_file_name() ==
              std::format("sigma_part_{}.dep", i + 1));
    }
    std::shared_ptr<Package> package_ptr;
    REQUIRE_NOTHROW(package_ptr = package.connect(parts));
  }
}
TEST_CASE("Package manager") {
  SECTION("add") {
    Package_manager package_manager;
    std::vector<std::shared_ptr<Package>> packages(5);
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
    std::vector<std::shared_ptr<Package>> empty_vec;
    int c = 0;
    for (std::shared_ptr<Package> &el : packages) {

      el = std::make_shared<Main_package>(
          Main_package(package_names[c], "vlad", "2.1", "2.1", empty_vec));

      c++;
    }
    for (std::shared_ptr<Package> &el : packages) {

      for (int i = c; i < (c + 5); i++) {
        Support_package support_package(package_names[c + i - c], "vlad", "2.1",
                                        "2.1", empty_vec);

        el->insert_connected(
            std::make_shared<Support_package>(support_package));
      }
      c += 5;
    }
    for (std::shared_ptr<Package> &el : packages) {

      REQUIRE_NOTHROW(package_manager.add(el));
    }
    Support_package bad_package_1("bad_package_1.dep", "vlad", "2.1", "2.1",
                                  empty_vec);
    Support_package bad_package_2("bad_package_2.dep", "vlad", "2.1", "2.1",
                                  empty_vec);
    auto bad_ptr_1 = std::make_shared<Support_package>(bad_package_1);
    auto bad_ptr_2 = std::make_shared<Support_package>(bad_package_2);
    bad_ptr_1->insert_connected(bad_ptr_2);
    bad_ptr_2->insert_connected(bad_ptr_1);
    REQUIRE_THROWS(package_manager.add(bad_ptr_1));
    REQUIRE_THROWS(package_manager.add(bad_ptr_2));
    Support_package bad_package_3("bad_package_3.dep", "vlad", "2.1", "2.1",
                                  empty_vec);
    auto bad_ptr_3 = std::make_shared<Support_package>(bad_package_3);
    bad_ptr_2->erase_connected(*bad_ptr_2);
    bad_ptr_3->insert_connected(bad_ptr_3);
    bad_ptr_3->insert_connected(bad_ptr_1);
    Package_manager package_manager_2;
    REQUIRE_THROWS(package_manager_2.add(bad_ptr_1));
    Package_manager package_manager_3;
    REQUIRE_THROWS(package_manager_3.add(bad_ptr_2));
    REQUIRE_THROWS(package_manager_3.add(bad_ptr_1));

    Package_manager package_manager_4;

    Support_package fake_bad_package_1("bad_package_1.dep", "vlad", "2.1",
                                       "2.1", empty_vec);

    auto fake_bad_ptr_1 = std::make_shared<Support_package>(fake_bad_package_1);
    bad_ptr_3->erase_connected(*bad_ptr_1);
    bad_ptr_3->insert_connected(fake_bad_ptr_1);
    REQUIRE_THROWS(package_manager_4.add(bad_ptr_1));
    package_manager.cycle_destroy(bad_ptr_1);
    Controler controler;

    package_manager.cycle_destroy(bad_ptr_2);
    package_manager.cycle_destroy(bad_ptr_3);
    package_manager.cycle_destroy(fake_bad_ptr_1);
  }
  SECTION("remove") {
    Package_manager package_manager;
    std::vector<std::shared_ptr<Package>> packages(5);
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
    std::vector<std::shared_ptr<Package>> empty_vec;
    int c = 0;
    for (std::shared_ptr<Package> &el : packages) {

      el = std::make_shared<Main_package>(
          Main_package(package_names[c], "vlad", "2.1", "2.1", empty_vec));

      c++;
    }
    for (std::shared_ptr<Package> &el : packages) {

      for (int i = c; i < (c + 5); i++) {
        Support_package support_package(package_names[c + i - c], "vlad", "2.1",
                                        "2.1", empty_vec);

        el->insert_connected(
            std::make_shared<Support_package>(support_package));
      }
      c += 2;
    }
    for (std::shared_ptr<Package> &el : packages) {

      REQUIRE_NOTHROW(package_manager.add(el));
    }

    for (std::shared_ptr<Package> &el : packages) {

      REQUIRE_NOTHROW(package_manager.remove(el));
    }
  }
  SECTION("remove") {
    Package_manager package_manager;
    std::vector<std::shared_ptr<Package>> packages(5);
    std::vector<std::string> package_names = {"A.dep",
                                              "B.dep",
                                              "C.dep",
                                              "D.dep",
                                              "E.dep",
                                              "F.dep",
                                              "G.dep",
                                              "G1.dep",
                                              "G2.sqlite.dep",
                                              "security.tls.dep",
                                              "filesystem.vfs.dep",
                                              "config.json.dep",
                                              "math.linalg.dep",
                                              "audio.alc.dep",
                                              "render.vulkan.dep",
                                              "testing.gtest.dep",
                                              "io.async.dep",
                                              "shell.bash.dep",
                                              "ipc.message.dep",
                                              "utils.hash.dep",
                                              "net.http.dep",
                                              "script.lua.dep",
                                              "storage.nvme.dep",
                                              "debug.gdb.dep",
                                              "profiler.perf.dep",
                                              "driver.usb.dep",
                                              "kernel.syscall.dep",
                                              "memory.alloc.dep",
                                              "scheduler.rt.dep",
                                              "sensor.gpio.dep",
                                              "serial.uart.dep",
                                              "crypto.openssl.dep",
                                              "ui.qt.dep",
                                              "web.engine.dep",
                                              "cli.argparse.dep",
                                              "data.protobuf.dep",
                                              "image.jpeg.dep",
                                              "video.h264.dep",
                                              "lang.cpp.std.dep",
                                              "lang.python.runtime.dep",
                                              "vm.jit.dep",
                                              "sandbox.seccomp.dep",
                                              "auth.oauth.dep",
                                              "event.loop.dep",
                                              "cache.lru.dep",
                                              "format.csv.dep",
                                              "time.ntp.dep",
                                              "fs.ext4.dep",
                                              "net.bluetooth.dep",
                                              "hw.riscv.dep"};
    std::vector<std::shared_ptr<Package>> empty_vec;
    int c = 0;
    for (std::shared_ptr<Package> &el : packages) {

      el = std::make_shared<Main_package>(
          Main_package(package_names[c], "vlad", "2.1", "2.1", empty_vec));

      c++;
    }
    for (std::shared_ptr<Package> &el : packages) {

      for (int i = c; i < (c + 5); i++) {
        Support_package support_package(package_names[c + i - c], "vlad", "2.1",
                                        "2.1", empty_vec);

        el->insert_connected(
            std::make_shared<Support_package>(support_package));
      }
      c += 5;
    }
    int counter = 0;
    for (auto elem : packages) {
      REQUIRE_NOTHROW(package_manager.add(elem));
    }
    for (auto elem : package_names) {
      if (package_manager.find(elem)) {
        package_manager.remove(elem);
        counter++;
      }
    }
    REQUIRE(counter == 5);
    if (counter == 5) {
    }
  }

  SECTION("remove unuse") {
    Package_manager package_manager;
    std::vector<std::shared_ptr<Package>> packages(5);
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
    std::vector<std::shared_ptr<Package>> empty_vec;
    int c = 0;
    for (std::shared_ptr<Package> &el : packages) {

      el = std::make_shared<Main_package>(
          Main_package(package_names[c], "vlad", "2.1", "2.1", empty_vec));

      c++;
    }
    for (std::shared_ptr<Package> &el : packages) {

      for (int i = c; i < (c + 5); i++) {
        Support_package support_package(package_names[c + i - c], "vlad", "2.1",
                                        "2.1", empty_vec);

        el->insert_connected(
            std::make_shared<Support_package>(support_package));
      }
      c += 2;
    }
    for (std::shared_ptr<Package> &el : packages) {

      REQUIRE_NOTHROW(package_manager.add(el));
    }
    auto size = 18;
    REQUIRE_NOTHROW(package_manager.remove_unuse());
    REQUIRE(package_manager.size() == size);
  }
}

TEST_CASE("Controler") {
  SECTION("Package read/write") {

    Package_manager support_package_manager;
    Package_manager main_package_manager;
    std::vector<std::shared_ptr<Package>> support_packages(5);
    std::vector<std::shared_ptr<Package>> main_packages(5);
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
    std::vector<std::shared_ptr<Package>> empty_vec;
    int c = 0;

    for (std::shared_ptr<Package> &el : main_packages) {

      el = std::make_shared<Main_package>(
          Main_package(package_names[c], "vlad", "2.1", "2.1", empty_vec));

      c++;
    }
    for (std::shared_ptr<Package> &el : main_packages) {

      for (int i = c; i < (c + 5); i++) {
        Main_package main_package(package_names[c + i - c], "vlad", "2.1",
                                  "2.1", empty_vec);

        el->insert_connected(std::make_shared<Main_package>(main_package));
      }
      c += 5;
    }
    Controler controler;

    for (std::shared_ptr<Package> &el : main_packages) {
      std::ostringstream oss;
      controler.write_package(el, oss);

      std::string str = oss.str();
      std::istringstream iss(str);

      std::shared_ptr<Package> new_el = controler.read_package(iss);
      REQUIRE(new_el != nullptr);

      REQUIRE_NOTHROW(main_package_manager.add(new_el));
    }

    std::vector<std::shared_ptr<Package>> empty_packages;
    Empty_package empty;
    for (std::shared_ptr<Package> &el : main_packages) {
      empty = Empty_package(el->get_file_name(), el);
      empty_packages.push_back(std::make_shared<Empty_package>(empty));
    }
    Package_manager empty_manager;
    for (std::shared_ptr<Package> &el : empty_packages) {
      std::ostringstream oss;
      controler.write_package(el, oss);

      std::string str = oss.str();
      std::istringstream iss(str);

      std::shared_ptr<Package> new_el = controler.read_package(iss);
      REQUIRE(new_el != nullptr);

      REQUIRE_NOTHROW(empty_manager.add(new_el));
    }
  }
  SECTION("Package manager read/write") {
    Package_manager package_manager;
    std::vector<std::shared_ptr<Package>> packages(5);
    std::vector<std::string> package_names = {"A.dep",
                                              "B.dep",
                                              "C.dep",
                                              "D.dep",
                                              "E.dep",
                                              "F.dep",
                                              "G.dep",
                                              "G1.dep",
                                              "G2.sqlite.dep",
                                              "security.tls.dep",
                                              "filesystem.vfs.dep",
                                              "config.json.dep",
                                              "math.linalg.dep",
                                              "audio.alc.dep",
                                              "render.vulkan.dep",
                                              "testing.gtest.dep",
                                              "io.async.dep",
                                              "shell.bash.dep",
                                              "ipc.message.dep",
                                              "utils.hash.dep",
                                              "net.http.dep",
                                              "script.lua.dep",
                                              "storage.nvme.dep",
                                              "debug.gdb.dep",
                                              "profiler.perf.dep",
                                              "driver.usb.dep",
                                              "kernel.syscall.dep",
                                              "memory.alloc.dep",
                                              "scheduler.rt.dep",
                                              "sensor.gpio.dep",
                                              "serial.uart.dep",
                                              "crypto.openssl.dep",
                                              "ui.qt.dep",
                                              "web.engine.dep",
                                              "cli.argparse.dep",
                                              "data.protobuf.dep",
                                              "image.jpeg.dep",
                                              "video.h264.dep",
                                              "lang.cpp.std.dep",
                                              "lang.python.runtime.dep",
                                              "vm.jit.dep",
                                              "sandbox.seccomp.dep",
                                              "auth.oauth.dep",
                                              "event.loop.dep",
                                              "cache.lru.dep",
                                              "format.csv.dep",
                                              "time.ntp.dep",
                                              "fs.ext4.dep",
                                              "net.bluetooth.dep",
                                              "hw.riscv.dep"};
    std::vector<std::shared_ptr<Package>> empty_vec;
    int c = 0;
    for (std::shared_ptr<Package> &el : packages) {

      el = std::make_shared<Main_package>(
          Main_package(package_names[c], "vlad", "2.1", "2.1", empty_vec));

      c++;
    }
    for (std::shared_ptr<Package> &el : packages) {

      for (int i = c; i < (c + 5); i++) {
        Support_package support_package(package_names[c + i - c], "vlad", "2.1",
                                        "2.1", empty_vec);

        el->insert_connected(
            std::make_shared<Support_package>(support_package));
      }
      c += 5;
    }
    Controler controler;
    for (std::shared_ptr<Package> &el : packages) {
      std::ostringstream oss;
      controler.write_package(el, oss);

      std::string str = oss.str();
      std::istringstream iss(str);

      std::shared_ptr<Package> new_el = controler.read_package(iss);
      REQUIRE(new_el != nullptr);

      REQUIRE_NOTHROW(package_manager.add(new_el));
    }
    std::ofstream output_file("example.txt");

    if (!output_file.is_open()) {

      throw std::runtime_error("bad file open");
    }

    controler.write_mananger(package_manager, output_file);

    output_file.close();

    std::ifstream input_file("example.txt");

    if (!input_file.is_open()) {

      throw std::runtime_error("bad file open");
    }
    Package_manager new_package_manager;

    REQUIRE_NOTHROW(new_package_manager = controler.read_manager(input_file));
    input_file.close();
    int counter = 0;

    for (auto &file_name : package_names) {

      if (new_package_manager.find(file_name)) {
        if (!package_manager.find(file_name)) {
        }
        new_package_manager.remove(file_name);
        counter++;
      }
    }
    REQUIRE(counter == 5);
  }
}

TEST_CASE("ALL") {
  SECTION("1.txt") {

    std::vector<std::shared_ptr<Package>> packages(5);
    std::vector<std::string> package_names = {
        "A.dep", "B.dep", "C.dep", "D.dep", "E.dep", "F.dep", "G.dep",
        "H.dep", "I.dep", "J.dep", "K.dep", "L.dep", "M.dep", "N.dep",
        "O.dep", "P.dep", "Q.dep", "R.dep", "S.dep", "T.dep", "U.dep",
        "V.dep", "W.dep", "X.dep", "Y.dep", "Z.dep"};
    std::vector<std::shared_ptr<Package>> empty_vec;
    int c = 0;
    for (std::shared_ptr<Package> &el : packages) {

      el = std::make_shared<Main_package>(
          Main_package(package_names[c], "vlad", "2.1", "2.1", empty_vec));

      c++;
    }
    for (std::shared_ptr<Package> &el : packages) {

      for (int i = c; i < (c + 5); i++) {
        Support_package support_package(package_names[c + i - c], "vlad", "2.1",
                                        "2.1", empty_vec);

        el->insert_connected(
            std::make_shared<Support_package>(support_package));
      }
      c += 2;
    }

    Package_manager package_manager;
    for (const auto &elem : packages) {
      REQUIRE_NOTHROW(package_manager.add(elem));
    }

    REQUIRE_THROWS(package_manager.remove(package_names[6]));
    REQUIRE_THROWS(package_manager.remove(package_names[10]));
    REQUIRE_THROWS(package_manager.remove(package_names[19]));
    REQUIRE_THROWS(package_manager.remove(package_names[20]));
    package_manager.remove(package_names[4]);
    REQUIRE(package_manager.size() == 15);
    package_manager.remove(package_names[2]);
    REQUIRE(package_manager.size() == 14);
    package_manager.remove(package_names[0]);
    REQUIRE(package_manager.size() == 11);
  }
  SECTION("1.txt + empty") {

    std::vector<std::shared_ptr<Package>> packages(5);
    std::vector<std::string> package_names = {
        "A.dep", "B.dep", "C.dep", "D.dep", "E.dep", "F.dep", "G.dep",
        "H.dep", "I.dep", "J.dep", "K.dep", "L.dep", "M.dep", "N.dep",
        "O.dep", "P.dep", "Q.dep", "R.dep", "S.dep", "T.dep", "U.dep",
        "V.dep", "W.dep", "X.dep", "Y.dep", "Z.dep"};
    std::vector<std::shared_ptr<Package>> empty_vec;
    int c = 0;
    for (std::shared_ptr<Package> &el : packages) {

      el = std::make_shared<Main_package>(
          Main_package(package_names[c], "vlad", "2.1", "2.1", empty_vec));

      c++;
    }
    c = 0;
    for (std::shared_ptr<Package> &elem : packages) {

      elem = std::make_shared<Empty_package>(
          Empty_package(elem->get_file_name(), elem));

      c++;
    }

    for (std::shared_ptr<Package> &el : packages) {

      for (int i = c; i < (c + 5); i++) {
        Support_package support_package(package_names[c + i - c], "vlad", "2.1",
                                        "2.1", empty_vec);

        el->insert_connected(
            std::make_shared<Support_package>(support_package));
      }
      c += 2;
    }

    Package_manager package_manager;
    for (const auto &elem : packages) {
      REQUIRE_NOTHROW(package_manager.add(elem));
    }
    REQUIRE_THROWS(package_manager.remove(package_names[6]));
    REQUIRE_THROWS(package_manager.remove(package_names[10]));
    REQUIRE_THROWS(package_manager.remove(package_names[19]));
    REQUIRE_THROWS(package_manager.remove(package_names[20]));
    package_manager.remove(package_names[4]);
    REQUIRE(package_manager.size() == 15);
    package_manager.remove(package_names[2]);
    REQUIRE(package_manager.size() == 14);
    package_manager.remove(package_names[0]);
    REQUIRE(package_manager.size() == 11);
  }
  SECTION("2.txt") {

    std::vector<std::shared_ptr<Package>> packages(3);
    std::vector<std::string> package_names = {
        "A.dep", "B.dep", "C.dep", "D.dep", "E.dep", "F.dep", "G.dep",
        "H.dep", "I.dep", "J.dep", "K.dep", "L.dep", "M.dep", "N.dep",
        "O.dep", "P.dep", "Q.dep", "R.dep", "S.dep", "T.dep", "U.dep",
        "V.dep", "W.dep", "X.dep", "Y.dep", "Z.dep"};
    std::vector<std::shared_ptr<Package>> empty_vec;
    int c = 0;
    Support_package n_1(package_names[10], "vlad", "2.1", "2.1", empty_vec);
    Support_package n_2(package_names[11], "vlad", "2.1", "2.1", empty_vec);
    for (std::shared_ptr<Package> &el : packages) {

      el = std::make_shared<Main_package>(Main_package(
          "M" + package_names[c], "vlad", "2.1", "2.1", empty_vec));

      c++;
    }
    c = 0;
    for (std::shared_ptr<Package> &elem : packages) {

      elem = std::make_shared<Empty_package>(
          Empty_package(package_names[c], elem));

      c++;
    }
    unsigned el_c = 0;
    c = 3;
    for (std::shared_ptr<Package> &el : packages) {
      el_c = 0;

      for (int i = c; i < (c + 3); i++) {
        Support_package support_package(package_names[c + i - c], "vlad", "2.1",
                                        "2.1", empty_vec);
        if (c == 5 && el_c == 1) {

          support_package.insert_connected(
              std::make_shared<Support_package>(n_1));
        }
        if (i == 7) {
          support_package.insert_connected(
              std::make_shared<Support_package>(n_1)); // K.dep
          support_package.insert_connected(
              std::make_shared<Support_package>(n_2)); // L.dep
        }

        if (c == 7 && el_c == 1) {

          support_package.insert_connected(
              std::make_shared<Support_package>(n_2));
        }
        el->insert_connected(
            std::make_shared<Support_package>(support_package));
        el_c++;
      }
      c += 2;
    }

    Package_manager package_manager;
    for (const auto &elem : packages) {
      REQUIRE_NOTHROW(package_manager.add(elem));
    }

    int size_1 = package_manager.size();
    REQUIRE_THROWS(package_manager.remove(package_names[6]));
    REQUIRE_THROWS(package_manager.remove(package_names[10]));
    REQUIRE_THROWS(package_manager.remove(package_names[19]));
    REQUIRE_THROWS(package_manager.remove(package_names[20]));
    int size_2 = package_manager.size();
    REQUIRE(size_1 == 12);
    REQUIRE(size_2 == 12);
    package_manager.remove(package_names[0]);
    REQUIRE(package_manager.size() == 9);
    package_manager.remove(package_names[2]);
    REQUIRE(package_manager.size() == 6);
    package_manager.remove(package_names[1]);
    REQUIRE(package_manager.size() == 0);

    for (const auto &elem : packages) {
      REQUIRE_NOTHROW(package_manager.add(elem));
    }
    package_manager.remove(package_names[2]);
    REQUIRE(package_manager.size() == 9);
    package_manager.remove(package_names[1]);
    REQUIRE(package_manager.size() == 4);
    package_manager.remove(package_names[0]);
    REQUIRE(package_manager.size() == 0);
  }
  SECTION("3.txt") {
    Package_manager m_1;
    Package_manager m_2;
    std::vector<std::shared_ptr<Package>> unuse_packages(25);
    std::vector<std::string> package_names = {
        "A.dep", "B.dep", "C.dep", "D.dep", "E.dep", "F.dep", "G.dep",
        "H.dep", "I.dep", "J.dep", "K.dep", "L.dep", "M.dep", "N.dep",
        "O.dep", "P.dep", "Q.dep", "R.dep", "S.dep", "T.dep", "U.dep",
        "V.dep", "W.dep", "X.dep", "Y.dep", "Z.dep"};
    std::vector<std::shared_ptr<Package>> empty_vec;

    for (int i = 0; i < 25; i++) {

      Support_package tmp(package_names[i], "vlad", "2.1", "2.1", empty_vec);
      unuse_packages[i] = std::make_shared<Support_package>(tmp);
    }

    REQUIRE_NOTHROW(build_manager(m_1, 3, 3, 3));
    REQUIRE_NOTHROW(build_manager(m_2, 3, 3, 3));
    for (auto elem : unuse_packages) {
      m_1.add(elem);
      m_2.add(elem);
    }

    REQUIRE(m_1.size() == (120 + unuse_packages.size()));
    REQUIRE(m_2.size() == (120 + unuse_packages.size()));
    REQUIRE_NOTHROW(m_1.remove_unuse());
    REQUIRE_NOTHROW(m_2.remove_unuse_one_thread());
    REQUIRE(m_1.size() == 120);
    REQUIRE(m_2.size() == 120);
  }
}