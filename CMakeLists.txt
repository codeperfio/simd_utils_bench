cmake_minimum_required(VERSION 2.8.12.2)
project(simd_utils_bench)
set(CMAKE_C_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(AVX REQUIRED)
include(detect_platform.cmake)
include(compile_options.cmake) 

include_directories(deps/simd_utils)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)

add_subdirectory(deps/benchmark)
include_directories(deps/benchmark/include)

add_executable(simd_utils_bench benchmarks.cpp)
target_link_libraries(simd_utils_bench benchmark::benchmark)
