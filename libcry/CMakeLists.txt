cmake_minimum_required(VERSION 2.8)

project(libcry)

add_compile_options(-std=c++11)
file(GLOB libcry_src *.cpp *.cc)
include_directories(.)
add_library(cry ${libcry_src})
