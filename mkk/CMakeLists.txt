cmake_minimum_required(VERSION 3.20)
project(ass3)

set(CMAKE_CXX_STANDARD 14)

add_executable(ass3 main.cpp)

target_compile_options(ass3 PUBLIC -fsanitize=address)
target_link_options(ass3 PUBLIC -fsanitize=address)
