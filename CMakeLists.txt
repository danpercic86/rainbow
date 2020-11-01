cmake_minimum_required(VERSION 3.17)
project(rainbow C)

include_directories(includes)
set(CMAKE_C_STANDARD 11)

add_executable(rainbow
        src/main.c
        includes/main.h
        )