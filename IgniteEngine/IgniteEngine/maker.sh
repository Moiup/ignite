#!/bin/bash

if [ ! -e build ]
then
    mkdir build
    cd build
    cmake ..
    cd ..
fi

echo $0
echo $1

cd build

if [ ! -z "$1" ]
then
    make $1
else
    make
fi

cd ..
