cmake_minimum_required(VERSION 3.21)
project(uwhpc_onboarding CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(uwhpc_benchmark bench/main.cpp)
target_include_directories(uwhpc_benchmark PRIVATE src)

enable_testing()
add_test(NAME correctness COMMAND uwhpc_benchmark --check)
