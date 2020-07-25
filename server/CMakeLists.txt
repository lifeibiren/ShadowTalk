file(GLOB server_src *.cpp *.cc)
include_directories(.)

add_executable(server_bin ${server_src})
target_link_libraries(
    server_bin
    ${Boost_LIBRARIES}
    cryptopp
    yaml-cpp
    ssl
    crypto
)
