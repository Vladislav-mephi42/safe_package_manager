cmake_minimum_required(VERSION 3.14)
project(my_project LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


#if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#   add_compile_options(-Wall -Wextra -Wpedantic)
#endif()


set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TESTS ${CMAKE_CURRENT_SOURCE_DIR}/tests)

include(FetchContent)


FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)


FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.0
)


FetchContent_MakeAvailable(json catch2)


set(LOGIC_SOURCES
    ${SRC}/package/package.cpp
    ${SRC}/package/main_package.cpp
    ${SRC}/package/support_package.cpp
    ${SRC}/package/empty_package.cpp
    ${SRC}/controler/controler.cpp
    ${SRC}/controler/strategy.cpp
    ${SRC}/package_manager/package_manager.cpp
    ${SRC}/network/handle_strategy.cpp
)


add_library(my_logic STATIC ${LOGIC_SOURCES})


target_include_directories(my_logic PUBLIC ${SRC})


target_link_libraries(my_logic PUBLIC nlohmann_json::nlohmann_json)

add_executable(client_app ${SRC}/application/client_app.cpp)
target_link_libraries(client_app PRIVATE my_logic)

add_executable(server_app ${SRC}/application/server_app.cpp)
target_link_libraries(server_app PRIVATE my_logic)

add_executable(admin_app ${SRC}/application/admin_app.cpp)
target_link_libraries(admin_app PRIVATE my_logic)

add_executable(main_test ${TESTS}/main_test.cpp)
target_link_libraries(main_test PRIVATE my_logic Catch2::Catch2WithMain)


add_executable(map_tests ${TESTS}/map_tests.cpp)
target_link_libraries(map_tests PRIVATE my_logic Catch2::Catch2WithMain)