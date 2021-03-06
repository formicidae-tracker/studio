cmake_policy(SET CMP0048 NEW)

project(fort-studio VERSION 0.7.3
                    LANGUAGES C CXX)

cmake_minimum_required(VERSION 3.11)

set(VERSION_API ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
if(${PROJECT_VERSION_MAJOR} EQUAL 0)
	set(VERSION_ABI ${VERSION_API})
else(${PROJECT_VERSION_MAJOR} EQUAL 0)
	set(VERSION_ABI ${PROJECT_VERSION_MAJOR})
endif(${PROJECT_VERSION_MAJOR} EQUAL 0)


list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(Eigen3 3.3.4 REQUIRED NO_MODULE)
find_package(Protobuf 3.3.0 REQUIRED)

find_package(TBB REQUIRED)

find_package(OpenCV REQUIRED core imgcodecs imgproc highgui videoio)

find_package(Qt5 COMPONENTS Widgets Concurrent Test Charts REQUIRED)

find_package(yaml-cpp REQUIRED)

include(FetchFortTags)
include(FetchFortHermes)
include(FetchSemver)
include(FetchGoogleTest)

fetch_google_test(VERSION release-1.10.0)
fetch_fort_tags(VERSION v1.4.0)
fetch_fort_hermes(VERSION v0.4.1)
fetch_semver(VERSION v0.2.2)

include(GoogleTest)
include(EnableMakeCheck)
enable_testing()
enable_make_check()




include_directories(${EIGEN3_INCLUDE_DIRS}
                    ${PROTOBUF_INCLUDE_DIRS}
                    ${FORT_HERMES_INCLUDE_DIRS}
                    ${APRILTAG_INCLUDE_DIRS}
                    ${FORT_TAGS_INCLUDE_DIRS}
                    ${HERMES_INCLUDE_DIRS}
                    ${SEMVER_INCLUDE_DIRS}
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


check_cxx_source_compiles("#include <variant>

int main() {
	std::variant<int, double> v, w;
	v = 0;
	w = 12.0;
	return std::get<int>(v);
}
"
	HAS_STD_VARIANT)
if ( NOT HAS_STD_VARIANT )
	message(FATAL_ERROR "fort-myrmidon needs std::variant support: ${HAS_STD_VARIANT}")
endif ( NOT HAS_STD_VARIANT )


option(ENABLE_COVERAGE "Enables coverage generation" Off)

if(ENABLE_COVERAGE)
	include(CodeCoverage)

	enable_coverage()
endif(ENABLE_COVERAGE)


option(UNIT_TEST_CHECK_TIMING "Test timing improvements during unit tests. May fails due to kernel scheduling, disable for automation" Off)
if(UNIT_TEST_CHECK_TIMING)
	add_definitions("-DMYRMIDON_TEST_TIMING")
endif(UNIT_TEST_CHECK_TIMING)

set(INCLUDE_PATH include)
set(INCLUDE_INSTALL_DIR ${INCLUDE_PATH}/fort/myrmidon)
set(LIB_INSTALL_DIR lib )

include(CMakePackageConfigHelpers)
configure_package_config_file(FortMyrmidonConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/FortMyrmidonConfig.cmake
	INSTALL_DESTINATION ${LIB_INSTALL_DIR}/FortMyrmidon/cmake
	PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR INCLUDE_PATH)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/FortMyrmidonConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/FortMyrmidonConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/FortMyrmidonConfigVersion.cmake
        DESTINATION ${LIB_INSTALL_DIR}/FortMyrmidon/cmake )

add_subdirectory(pkgconfig)

add_subdirectory(src)

if(ENABLE_COVERAGE)
	setup_target_for_coverage(NAME coverage
	                          DEPENDENCIES check
	                          LCOV_OPTIONS --exclude "'*UTest.*'"
	                                       --exclude "main-check.cpp"
										   --include "'${PROJECT_SOURCE_DIR}/src/fort/myrmidon/priv/*'"
	                          )
endif(ENABLE_COVERAGE)

option(BUILD_DOCS "Builds the documentation" Off)
option(DOCS_USES_PATCHED_HEADERS "Documentation shoudl use patched system headers" Off)
if(BUILD_DOCS)
	add_subdirectory(docs)
endif(BUILD_DOCS)

add_subdirectory(misc)
