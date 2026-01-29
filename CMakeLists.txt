cmake_minimum_required(VERSION 3.14)
project(my_project LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic )
endif()


set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TESTS ${CMAKE_CURRENT_SOURCE_DIR}/tests)


set(LOGIC_SOURCES
    ${SRC}/package/package.cpp
    ${SRC}/package/main_package.cpp
    ${SRC}/package/support_package.cpp
    ${SRC}/package/empty_package.cpp
    ${SRC}/controler/controler.cpp
    ${SRC}/package_manager/package_manager.cpp
)


add_library(my_logic STATIC ${LOGIC_SOURCES})
target_include_directories(my_logic PUBLIC ${SRC})


add_executable(app ${SRC}/application/application.cpp)
target_link_libraries(app PRIVATE my_logic)


find_package(Catch2 REQUIRED)

add_executable(main_test ${TESTS}/main_test.cpp)
target_link_libraries(main_test PRIVATE my_logic Catch2::Catch2WithMain)

add_executable(timing ${TESTS}/timing.cpp)
target_link_libraries(timing PRIVATE my_logic Catch2::Catch2WithMain)


add_executable(map_tests ${TESTS}/map_tests.cpp)
target_include_directories(map_tests PRIVATE ${SRC})
target_link_libraries(map_tests PRIVATE Catch2::Catch2WithMain)