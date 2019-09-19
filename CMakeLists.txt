cmake_policy(SET CMP0048 NEW)

project(fort-studio VERSION 0.1.0
                    LANGUAGES C CXX)

cmake_minimum_required(VERSION 3.2)


list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(EnableMakeCheck)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)

find_package(Eigen3 REQUIRED)
find_package(Protobuf 3.3.0 REQUIRED)


include_directories(${EIGEN3_INCLUDE_DIRS}
                    ${PROTOBUF_INCLUDE_DIRS})

enable_testing()
enable_make_check()

add_subdirectory(src)
