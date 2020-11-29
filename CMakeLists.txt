cmake_minimum_required(VERSION 3.16)
project(cpp)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.72.0 COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

include_directories(src)
add_subdirectory(src)

add_subdirectory(test)
add_executable(cpp main.cpp)

target_link_libraries(cpp Language-Tools)
