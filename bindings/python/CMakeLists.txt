cmake_minimum_required(VERSION 3.4...3.18)

project(py-fort-myrmidon)

find_package(pybind11 REQUIRED)
find_package(FortMyrmidon REQUIRED)

include_directories(${FORT_MYRMIDON_INCLUDE_DIRS})

add_subdirectory(src)

# EXAMPLE_VERSION_INFO is defined by setup.py and passed into the C++ code as a
# define (VERSION_INFO) here.
target_compile_definitions(py_fort_myrmidon PRIVATE VERSION_INFO=${PY_FORT_MYRMIDON_VERSION_INFO})
