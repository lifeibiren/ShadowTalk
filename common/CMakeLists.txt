cmake_minimum_required(VERSION 2.8)

file(GLOB commonSrc *.cpp *.cc)
include_directories(.)

add_library(common STATIC ${commonSrc})
