#include "network/client.h"
#include "network/network_controler.h"
#include "network/server.h"
#include <algorithm>
#include <arpa/inet.h>
#include <csignal>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef htonll
#define htonll(x) ((uint64_t)htonl((x) & 0xFFFFFFFF) << 32 | htonl((x) >> 32))
#endif
#ifndef ntohll
#define ntohll(x) ((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32 | ntohl((x) >> 32))
#endif

volatile sig_atomic_t stop_flag = 0;

void sigchld_handler(int sig) {
  while (waitpid(-1, nullptr, WNOHANG) > 0)
    ;
}

void sigint_handler(int) { stop_flag = 1; }
int main() {
  std::vector<std::string> repozitories_file_names = {
      "rep_1.json", "rep_2.json", "rep_3.json"};
  std::cout << "=====Server start working...=====" << std::endl;
  try {

    struct sigaction sa = {};

    sa.sa_handler = sigchld_handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGCHLD, &sa, nullptr) == -1) {
      std::cout << "Eror with sigaction SIGCHLD" << std::endl;
      return 1;
    }

    sa.sa_handler = sigint_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, nullptr) == -1) {
      std::cout << "Error with sigaction SIGINT" << std::endl;
      return 1;
    }

    Server_socket server(PORT, INADDR_ANY);

    if (listen(server.get_server_socket(), 10) < 0) {
      return 1;
    }

    while (!stop_flag) {
      Client_socket client = server.accept();

      std::cout << "handle client...." << std::endl;
      json request = client.recv_json();
      if (!(request.contains("user_type") &&
            request.contains("request_type"))) {
        json error;
        error["status"] = "fail";
        error["final"] = "bad request json format";
        client.send_json(error);
        continue;
      }
      if (request["user_type"] == "admin") {
        std::vector<std::shared_ptr<Package_manager>> pm_vec;
        std::vector<std::shared_ptr<Controler>> controler_vec;
        std::vector<std::shared_ptr<Server_network_controler>> server_vec;
        for (const auto &elem : repozitories_file_names) {
          auto pm = std::make_shared<Package_manager>();
          auto controler = std::make_shared<Controler>();

          controler->read_package_manager_from_file(elem, *pm);
          *controler =
              Controler(elem, pm.get()); // pm.get() возвращает сырой указатель

          auto server =
              std::make_shared<Server_network_controler>(controler.get());

          pm_vec.push_back(pm);
          controler_vec.push_back(controler);
          server_vec.push_back(server);
        }
        if (!request.contains("file_name")) {
          json error;
          error["status"] = "fail";
          error["final"] = "bad request json format";
          client.send_json(error);
          continue;
        }
        int index = -1;
        for (int i = 0; i < repozitories_file_names.size(); i++) {
          if (repozitories_file_names[i] ==
              request["file_name"].get<std::string>()) {
            index = i;
          }
        }
        if (index == -1) {
          json error;
          error["status"] = "fail";
          error["final"] = "bad file_name in request";
          client.send_json(error);
          continue;
        } else {
          server_vec[index]->handle(request, client);
        }
        continue;
      }
      pid_t pid = fork();

      if (pid < 0) {
        client.manuly_close();
        continue;
      }

      if (pid == 0) {
        std::signal(SIGINT, SIG_DFL);
        server.manuly_close();
        Controler controler;
        Server_network_controler server_c(&controler);
        server_c.handle(request, client);
        return 0;
      } else {
        client.manuly_close();
        continue;
      }
    }
    std::cout << "=====Server end working...=======" << std::endl;
    return 0;
  } catch (const std::exception &e) {
    std::cout << "Error : " << e.what() << std::endl;
    std::cout << "=====Server end working...=======" << std::endl;

    return 1;
  }
}