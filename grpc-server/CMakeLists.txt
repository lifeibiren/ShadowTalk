file(GLOB grpc-server-src *.cpp *.cc)
include_directories(.)

add_executable(grpc-server ${grpc-server-src})
target_link_libraries(
    grpc-server
    ${Boost_LIBRARIES}
    hw_grpc_proto
    ssl
    crypto
    gpr
    # grpc++
)



