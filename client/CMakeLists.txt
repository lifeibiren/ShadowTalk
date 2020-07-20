cmake_minimum_required(VERSION 2.8)

project(client_exe)

add_definitions(-DBOOST_ENDIAN_DEPRECATED_NAMES)
file(GLOB client_src *.cpp *.cc)
include_directories(.)
add_subdirectory(sml)

add_executable(client_bin ${client_src})
target_link_libraries(client_bin sml libboost_system.a libboost_thread.a dl pthread cryptopp yaml-cpp ssl crypto)
