file(GLOB ws_src *.cpp *.cc)
include_directories(.)

add_executable(ws ${ws_src})
target_link_libraries(
    ws
    ${Boost_LIBRARIES}
)
