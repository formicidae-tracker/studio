#!/bin/bash

set -x -e

declare -a CMAKE_PLATFORM_FLAGS

if [[ ${HOST} =~ .*darwin.* ]]; then
	CMAKE_PLATFORM_FLAGS+=(-DCMAKE_OSX_SYSROOT="${CONDA_BUILD_SYSROOT}")
	export LDFLAGS=$(echo "${LDFLAGS}" | sed "s/-Wl,-dead_strip_dylibs//g")
else
	CMAKE_PLATFORM_FLAGS+=(-DCMAKE_TOOLCHAIN_FILE="${RECIPE_DIR}/cross-linux.cmake")
fi


if [[ ${DEBUG_C} == yes ]]; then
  CMAKE_BUILD_TYPE=Debug
else
  CMAKE_BUILD_TYPE=Release
fi

cmake -DCMAKE_INSTALL_PREFIX=${PREFIX} \
      -DCMAKE_INSTALL_LIBDIR="lib" \
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
      -DCMAKE_C_FLAGS_RELEASE=${CFLAGS} \
      -DCMAKE_C_FLAGS_DEBUG=${CFLAGS} \
	  -DBUILD_STUDIO=Off \
      ${CMAKE_PLATFORM_FLAGS[@]} \
      ${SRC_DIR}

make -j${CPU_COUNT} ${VERBOSE_CM} all check

make install -j${CPU_COUNT}
