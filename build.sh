#!/bin/bash

BUILD_DIR="build"

# Check if the build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
fi

cd "$BUILD_DIR" || exit

export CC=clang
export CXX=clang++

cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug
ninja

