cmake_minimum_required(VERSION 3.15)
project(LockFreeQueues LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)s

add_executable(lock_free
    src/main.cpp
)
target_link_libraries(lock_free pthread)

# unit-тесты
find_package(GTest REQUIRED)

add_executable(test_queue
    tests/test_queue.cpp
)

target_link_libraries(test_queue
    GTest::gtest
    GTest::gtest_main
    pthread
)

enable_testing()
add_test(NAME LockFreeTests COMMAND test_queue)

# banchmark 
find_package(benchmark REQUIRED)

add_executable(benchmark_queue
    benchmarks/benchmark_queue.cpp
)

target_link_libraries(benchmark_queue
    benchmark::benchmark
    pthread
)