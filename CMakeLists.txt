cmake_minimum_required(VERSION 2.8)

project(Whisper)

add_compile_options(-std=c++11)
add_definitions(-DBOOST_ENDIAN_DEPRECATED_NAMES)

include_directories(common)
add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)
#include_directories(libcry)
#include_directories(libdtls)
#include_directories(.)
#add_subdirectory(libcry)
#add_subdirectory(libdtls)


#set(dtls cry libboost_system.a libboost_thread.a dl pthread)

#add_executable(client $<TARGET_OBJECTS:sml> "client.cpp")
#target_link_libraries(client ${common_libs})

