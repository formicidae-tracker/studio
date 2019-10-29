cmake_policy(SET CMP0048 NEW)

project(fort-studio VERSION 0.1.0
                    LANGUAGES C CXX)

cmake_minimum_required(VERSION 3.11)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)


find_package(Eigen3 3.3.4 REQUIRED NO_MODULE)
find_package(Protobuf 3.3.0 REQUIRED)
find_package(FortHermesCpp 0.2.0 REQUIRED)

find_package(Qt5 COMPONENTS Widgets Concurrent REQUIRED)

include(FetchContent)
FetchContent_Declare(googletest
                     GIT_REPOSITORY https://github.com/google/googletest.git
                     GIT_TAG        release-1.8.1
                     )

FetchContent_Declare(apriltag
                     GIT_REPOSITORY https://github.com/AprilRobotics/apriltag.git
                     GIT_TAG        v3.0.0)


FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
	FetchContent_Populate(googletest)
	add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif(NOT googletest_POPULATED)
include(GoogleTest)
include(EnableMakeCheck)
enable_testing()
enable_make_check()


FetchContent_GetProperties(apriltag)
if(NOT apriltag_POPULATED)
	FetchContent_Populate(apriltag)
	add_subdirectory(${apriltag_SOURCE_DIR} ${apriltag_BINARY_DIR})
endif(NOT apriltag_POPULATED)


include_directories(${EIGEN3_INCLUDE_DIRS}
                    ${PROTOBUF_INCLUDE_DIRS}
                    ${FORT_HERMES_CPP_INCLUDE_DIRS}
                    ${apriltag_SOURCE_DIR}
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
	set(COVERAGE_GCOVR_EXCLUDES ${apriltag_SOURCE_DIR}/*
	                            ${googletest_SOURCE_DIR}/*
	                            ${PROJECT_SOURCE_DIR}/src/fort-studio/*
	                            ${PROJECT_SOURCE_DIR}/src/legacy-tags/*
	                            ${PROJECT_BINARY_DIR}/src/fort-studio/*
	                            ${PROJECT_BINARY_DIR}/src/myrmidon/Experiment.pb.*
	                            )

	setup_target_for_coverage_gcovr_html(NAME myrmidon-coverage EXECUTABLE ctest -V -j ${PROCESSOR_COUNT} DEPENDENCIES myrmidon-tests)
endif(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_BUILD_TYPE STREQUAL "Debug")

option(BUILD_DOCS "Builds the documentation" Off)

if(BUILD_DOCS)
	add_subdirectory(docs)
endif(BUILD_DOCS)
