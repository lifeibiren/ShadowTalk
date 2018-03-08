cmake_minimum_required(VERSION 2.8)

project(libdtls)

add_compile_options(-std=c++11)
add_definitions(-DBOOST_ENDIAN_DEPRECATED_NAMES)
file(GLOB libdtls_src *.cpp *.cc)
include_directories(.)

add_library(dtls ${libdtls_src})
