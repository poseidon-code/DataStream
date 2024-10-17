cmake_minimum_required(VERSION 3.25)
project(Usage LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)



add_subdirectory(DataStream)

add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark PRIVATE DataStream)
