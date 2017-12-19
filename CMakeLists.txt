cmake_minimum_required(VERSION 2.8)

project(ShadowTalk)

add_compile_options(-std=c++11)
set(
    common_src sml.cpp udp_server.cpp peer.cpp message.cpp
    )
add_library(sml OBJECT ${common_src})
add_subdirectory(libcry)
include_directories(libcry)

set(common_libs boost_system boost_thread pthread cry)
add_executable(server $<TARGET_OBJECTS:sml> "server.cpp")
target_link_libraries(server ${common_libs})

add_executable(client $<TARGET_OBJECTS:sml> "client.cpp")
target_link_libraries(client ${common_libs})

