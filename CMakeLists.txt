
cmake_minimum_required(VERSION 3.15)

project(Assignment06 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(assignment06 assignment06.c)

target_compile_options(assignment06 PRIVATE -Wall -Wextra)
