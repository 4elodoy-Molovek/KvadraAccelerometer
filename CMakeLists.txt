cmake_minimum_required(VERSION 3.16)
project(KvadraAccelerometer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(gRPC CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(Threads REQUIRED)

option(BUILD_SERVER "Build Linux Server" ON)
option(BUILD_ANDROID_CLIENTS "Build Android NDK Clients" OFF)

add_subdirectory(proto)
add_subdirectory(common)

if(BUILD_SERVER)
    message(STATUS "Configuring Linux Server...")
    add_subdirectory(server)
endif()

if(BUILD_ANDROID_CLIENTS)
    message(STATUS "Configuring Android Clients...")
    add_subdirectory(client_node_a)
    add_subdirectory(client_node_b)
endif()