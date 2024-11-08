cmake_minimum_required(VERSION 3.20.0)

project(fastObj)

set(HEADERS
    fast_obj.h
)

set(SRC
    fast_obj.c
)

add_library(fastObj ${SRC} ${HEADERS})

target_include_directories(fastObj PUBLIC .)