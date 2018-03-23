cmake_minimum_required(VERSION 2.8)

file(GLOB libdtls_src *.cpp *.cc)
include_directories(.)

add_library(common STATIC ${libdtls_src})
