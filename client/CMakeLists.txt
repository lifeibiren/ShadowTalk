cmake_minimum_required(VERSION 3.1.0)

project(WhisperClient)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)

file(GLOB client_src *.cpp *.cc *.ui *.qrc)

include_directories(.)

add_executable(
    client_bin
    ${client_src}
)

target_link_libraries(client_bin Qt5::Widgets)
target_link_libraries(
    client_bin
    ${Boost_LIBRARIES}
    cryptopp
    yaml-cpp
    ssl
    crypto
)



# add_subdirectory(sml)