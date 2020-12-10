include(FetchContent)
include(CMakeParseArguments)

function(fetch_semver)
	cmake_parse_arguments(OPTS "" "VERSION" "" ${ARGN})

	FetchContent_Declare(semver
	                     GIT_REPOSITORY https://github.com/Neargye/semver.git
	                     GIT_TAG        ${OPTS_VERSION}
	                     )

	FetchContent_GetProperties(semver)
	if(NOT semver_POPULATED)
		FetchContent_Populate(semver)
		add_subdirectory(${semver_SOURCE_DIR} ${semver_BINARY_DIR})
	endif(NOT semver_POPULATED)

	set(SEMVER_INCLUDE_DIRS ${semver_SOURCE_DIR}/include PARENT_SCOPE)

endfunction(fetch_semver)
