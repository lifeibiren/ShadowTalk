cmake_minimum_required(VERSION 2.8)

file(GLOB libsml_src *.cpp *.cc)
include_directories(.)

add_library(sml STATIC ${libsml_src})
