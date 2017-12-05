cmake_minimum_required(VERSION 2.8)

project(ShadowTalk)

add_compile_options(-std=c++11)
set(common_src sml.cpp udp_server.cpp peer.cpp message.cpp key.cpp)
set(common_libs boost_system boost_thread)
add_executable(server "server.cpp" ${common_src})
target_link_libraries(server ${common_libs})

add_executable(client "client.cpp" ${common_src})
target_link_libraries(client ${common_libs} pthread)

