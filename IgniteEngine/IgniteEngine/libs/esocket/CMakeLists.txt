cmake_minimum_required(VERSION 3.20.0)

project(ESocket)

if(MSVC)
    file(GLOB HEADERS
        esocket/inc/*.h
        esocket/inc/windows/*.h
    )

    file(GLOB SRC
        esocket/src/*.cpp
        esocket/src/windows/*.cpp
    )

else()
    file(GLOB HEADERS
        esocket/inc/*.h
        esocket/inc/linux/*.h
    )

    file(GLOB SRC
        esocket/src/*.cpp
        esocket/src/linux/*.cpp
    )
endif()

add_library(ESocket ${SRC} ${HEADERS})

target_include_directories(ESocket PUBLIC .)