cmake_minimum_required(VERSION 2.8)

project(server_exe)

add_compile_options(-std=c++11)
add_definitions(-DBOOST_ENDIAN_DEPRECATED_NAMES)
file(GLOB server_src *.cpp *.cc)
include_directories(.)

add_executable(server_bin ${server_src})
target_link_libraries(server_bin libboost_system.a libboost_thread.a libboost_chrono.a dl pthread)
