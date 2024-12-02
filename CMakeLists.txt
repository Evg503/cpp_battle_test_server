cmake_minimum_required(VERSION 3.22)
project(sw_battle_project)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES src/*.cpp src/*.hpp)
get_filename_component(full_path_test_cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/test-main.cpp ABSOLUTE)
get_filename_component(full_path_main_cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp ABSOLUTE)

list(REMOVE_ITEM SOURCES ${full_path_test_cpp} ${full_path_main_cpp})

add_executable(test_sw_battle_test ${SOURCES} ${full_path_test_cpp})
add_executable(sw_battle_test ${SOURCES} ${full_path_main_cpp})

target_include_directories(test_sw_battle_test PUBLIC src/)
target_include_directories(sw_battle_test PUBLIC src/)
