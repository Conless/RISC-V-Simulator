cmake_minimum_required(VERSION 3.11)

project(RISC-V-Simulator)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

set(CMAKE_CXX_FLAGS "-Ofast")

include_directories(src/include)

add_subdirectory(src)

add_executable(code src/main.cpp)

target_link_libraries(code simulator)
