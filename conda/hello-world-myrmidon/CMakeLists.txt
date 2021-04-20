project(hello-world-myrmidon C CXX)
cmake_minimum_required(VERSION 3.11)

find_package(FortMyrmidon REQUIRED)

include_directories(${FORT_MYRMIDON_INCLUDE_DIRS})

add_executable(hello-world-cmake main.cpp)

target_link_libraries(hello-world-cmake ${FORT_MYRMIDON_LIBRARIES})
