cmake_policy(SET CMP0048 NEW)

project(fort-studio VERSION 0.1.0
                    LANGUAGES C CXX)

cmake_minimum_required(VERSION 3.2)


find_package(Eigen3 REQUIRED)

include_directories(${EIGEN3_INCLUDE_DIRS})


add_subdirectory(src)
