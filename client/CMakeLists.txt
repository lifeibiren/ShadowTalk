file(GLOB client_src *.cpp *.cc)
include_directories(.)
add_subdirectory(sml)

add_executable(client_bin ${client_src})
target_link_libraries(
    client_bin
    sml
    ${Boost_LIBRARIES}
    cryptopp
    yaml-cpp
    ssl
    crypto
)
