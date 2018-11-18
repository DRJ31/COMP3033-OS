cmake_minimum_required(VERSION 3.12)
project(Homework3)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS -lpthread)
add_executable(synchronization sync.c)