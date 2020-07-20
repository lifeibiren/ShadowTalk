cmake_minimum_required(VERSION 2.8)

project(Whisper)

add_compile_options(-std=c++17)
add_definitions(-DBOOST_ENDIAN_DEPRECATED_NAMES)

#set(CMAKE_CXX_FLAGS_DEBUG "-fsanitize=address,leak,undefined -g3")
#set(CMAKE_C_FLAGS_DEBUG "-fsanitize=address,leak,undefined -g3")
set(CMAKE_CXX_FLAGS "-std=c++17")
set(CMAKE_C_FLAGS_DEUBG "-g3")
set(CMAKE_CXX_FLAGS_DEBUG "-g3")
include_directories(common)
# add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)
