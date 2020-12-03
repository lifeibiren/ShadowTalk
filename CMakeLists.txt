cmake_minimum_required(VERSION 3.0)

project(Whisper)

add_compile_options(-std=c++17 -g3)
add_definitions(-DBOOST_ENDIAN_DEPRECATED_NAMES)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost COMPONENTS system coroutine)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
    add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
endif()

#set(CMAKE_CXX_FLAGS_DEBUG "-fsanitize=address,leak,undefined -g3")
#set(CMAKE_C_FLAGS_DEBUG "-fsanitize=address,leak,undefined -g3")
set(CMAKE_CXX_FLAGS "")
set(CMAKE_C_FLAGS_DEUBG "-fno-inline -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_DEBUG "-fno-inline -fno-omit-frame-pointer")
include_directories(common)
# add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(ws)
