cmake_policy(SET CMP0048 NEW)

project(fort-studio VERSION 0.1.0
                    LANGUAGES C CXX)

cmake_minimum_required(VERSION 3.11)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)


find_package(Eigen3 3.3.4 REQUIRED NO_MODULE)
find_package(Protobuf 3.3.0 REQUIRED)

find_package(OpenCV REQUIRED core imgcodecs imgproc)

find_package(Qt5 COMPONENTS Widgets Concurrent REQUIRED)

include(FetchContent)
FetchContent_Declare(googletest
                     GIT_REPOSITORY https://github.com/google/googletest.git
                     GIT_TAG        release-1.8.1
                     )

FetchContent_Declare(fort-tags
                     GIT_REPOSITORY https://github.com/formicidae-tracker/fort-tags.git
                     GIT_TAG        v1.1.0)

FetchContent_Declare(hermes
                     GIT_REPOSITORY https://github.com/formicidae-tracker/hermes.git
                     GIT_TAG        v0.2.3)



FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
	FetchContent_Populate(googletest)
	add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
endif(NOT googletest_POPULATED)
include(GoogleTest)
include(EnableMakeCheck)
enable_testing()
enable_make_check()


FetchContent_GetProperties(fort-tags)
if(NOT fort-tags_POPULATED)
	FetchContent_Populate(fort-tags)
	add_subdirectory(${fort-tags_SOURCE_DIR} ${fort-tags_BINARY_DIR})
endif(NOT fort-tags_POPULATED)


FetchContent_GetProperties(hermes)
if(NOT hermes_POPULATED)
	FetchContent_Populate(hermes)
	add_subdirectory(${hermes_SOURCE_DIR} ${hermes_BINARY_DIR})
endif(NOT hermes_POPULATED)
set(HERMES_INCLUDE_DIRS ${hermes_SOURCE_DIR}/src ${hermes_BINARY_DIR}/src)


include_directories(${EIGEN3_INCLUDE_DIRS}
                    ${PROTOBUF_INCLUDE_DIRS}
                    ${FORT_HERMES_CPP_INCLUDE_DIRS}
                    ${APRILTAG_INCLUDE_DIRS}
                    ${fort-tags_SOURCE_DIR}/include
                    ${HERMES_INCLUDE_DIRS}
                    )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)



include(CheckCXXSourceCompiles)


set(CMAKE_REQUIRED_FLAGS "-lstdc++fs")

check_cxx_source_compiles("#include <filesystem>

int main(){
	std::filesystem::path p;
	return 0;
}
"
	USE_CXX17_FS_LIBRARY
)


if(USE_CXX17_FS_LIBRARY)
	set(CXXFS_LIBRARY "-lstdc++fs")
else(USE_CXX17_FS_LIBRARY)

	find_package(Boost 1.60.0 REQUIRED COMPONENTS filesystem)
	include_directories(${Boost_filesystem_INCLUDE_DIRS})
	set(CXXFS_LIBRARY Boost::filesystem)
	set(MYRMIDON_USE_BOOST_FILESYSTEM 1)
endif(USE_CXX17_FS_LIBRARY)

if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_BUILD_TYPE STREQUAL "Debug")
	include(CodeCoverage)

	APPEND_COVERAGE_COMPILER_FLAGS()
endif(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_BUILD_TYPE STREQUAL "Debug")

add_subdirectory(src)

if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(COVERAGE_GCOVR_EXCLUDES ${PROJECT_SOURCE_DIR}/src/fort-studio/*
	                            ${PROJECT_BINARY_DIR}/src/fort-studio/*
	                            ${PROJECT_BINARY_DIR}/src/*
	                            ${hermes_SOURCE_DIR}/*
	                            ${hermes_BINARY_DIR}/*
	                            ${fort-tags_SOURCE_DIR}/*
	                            ${fort-tags_BINARY_DIR}/*
	                            ${googletest_SOURCE_DIR}/*
	                            ${googletest_BINARY_DIR}/*
	                            )

	setup_target_for_coverage_gcovr_html(NAME myrmidon-coverage EXECUTABLE ctest -V -j ${PROCESSOR_COUNT} DEPENDENCIES myrmidon-tests)
endif(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_BUILD_TYPE STREQUAL "Debug")

option(BUILD_DOCS "Builds the documentation" Off)

if(BUILD_DOCS)
	add_subdirectory(docs)
endif(BUILD_DOCS)
