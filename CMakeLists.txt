cmake_minimum_required(VERSION 3.26)
project(osi_hw_8 C)

set(CMAKE_C_STANDARD 23)

add_executable(osi_hw_8 main.c
        reader.h
        reader.c
        writer.c
        writer.h
        main.h)
