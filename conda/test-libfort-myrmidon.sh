#!/bin/bash

set -x -e

declare -a CMAKE_PLATFORM_FLAGS

if [[ ${HOST} =~ .*darwin.* ]]; then
	CMAKE_PLATFORM_FLAGS+=(-DCMAKE_OSX_SYSROOT="${CONDA_BUILD_SYSROOT}")
	export LDFLAGS=$(echo "${LDFLAGS}" | sed "s/-Wl,-dead_strip_dylibs//g")
else
	CMAKE_PLATFORM_FLAGS+=(-DCMAKE_TOOLCHAIN_FILE="../cross-linux.cmake")
fi

cd hello-world-myrmidon
mkdir build
cd build
cmake ${CMAKE_PLATFORM_FLAGS[@]} ..

make

cd ..

g++ -o hello-world-pkg-config main.cpp --std=c++17 $(pkg-config --cflags --libs fort-myrmidon)
