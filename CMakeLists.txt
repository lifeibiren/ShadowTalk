cmake_minimum_required(VERSION 2.8)

project(Whisper)

add_compile_options(-std=c++11)
add_definitions(-DBOOST_ENDIAN_DEPRECATED_NAMES)

include_directories(libcry)
include_directories(libdtls)
include_directories(.)
add_subdirectory(libcry)
add_subdirectory(libdtls)
set(
    common_src sml.cpp peer.cpp message.cpp
    message_piece.cpp peer_context.cpp
    )
add_library(sml OBJECT ${common_src})


set(common_libs dtls cry boost_system boost_thread dl pthread)
add_executable(server $<TARGET_OBJECTS:sml> "server.cpp")
target_link_libraries(server ${common_libs})

add_executable(client $<TARGET_OBJECTS:sml> "client.cpp")
target_link_libraries(client ${common_libs})

