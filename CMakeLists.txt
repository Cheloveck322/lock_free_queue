cmake_minimum_required(VERSION 3.10)
project(lock_free LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(lock_free src/main.cpp)