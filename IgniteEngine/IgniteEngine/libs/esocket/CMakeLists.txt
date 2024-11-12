cmake_minimum_required(VERSION 3.20.0)

project(ESocket)

file(GLOB_RECURSE HEADERS
    esocket/inc/*.h
)

file(GLOB_RECURSE SRC
    esocket/src/*.cpp
)

add_library(ESocket ${SRC} ${HEADERS})

target_include_directories(ESocket PUBLIC .)