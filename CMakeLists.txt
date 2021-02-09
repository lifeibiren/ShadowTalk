cmake_minimum_required(VERSION 3.0)

project(Whisper)

add_compile_options(-std=c++20 -g3)
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

find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)

set(CMAKE_CXX_FLAGS_DEBUG "-fsanitize=address,leak,undefined -g3")
set(CMAKE_C_FLAGS_DEBUG "-fsanitize=address,leak,undefined -g3")
set(CMAKE_CXX_FLAGS "")
# set(CMAKE_C_FLAGS_DEUBG "-fno-inline -fno-omit-frame-pointer ")
# set(CMAKE_CXX_FLAGS_DEBUG "-fno-inline -fno-omit-frame-pointer")


set(_PROTOBUF_LIBPROTOBUF protobuf)
set(_REFLECTION grpc++_reflection)
set(_GRPC_GRPCPP grpc++)
set(_GPRC grpc)

find_program(_PROTOBUF_PROTOC protoc)
find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
# Proto file
get_filename_component(hw_proto ${CMAKE_CURRENT_SOURCE_DIR}/proto/registry.proto ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)

# Generated sources
set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/registry.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/registry.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/registry.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/registry.grpc.pb.h")
add_custom_command(
      OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${hw_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${hw_proto}"
      DEPENDS "${hw_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
# hw_grpc_proto
add_library(hw_grpc_proto
  ${hw_grpc_srcs}
  ${hw_grpc_hdrs}
  ${hw_proto_srcs}
  ${hw_proto_hdrs})
target_link_libraries(hw_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
  ${_GPRC} gpr)

include_directories(common)
# add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(ws)
add_subdirectory(grpc-server)
