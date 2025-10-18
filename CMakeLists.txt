cmake_minimum_required(VERSION 3.10)
project(lock_free LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

add_library(lock_free_lib
    src/ring_buffer.cpp
)

target_include_directories(lock_free_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(lock_free src/main.cpp)

target_link_libraries(lock_free lock_free_lib)