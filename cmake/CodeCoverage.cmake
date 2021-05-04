# Copyright (c) 2012 - 2017, Lars Bilke
# Copyright (c) 2020 - 2021 Alexandre Tuleu
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Changes: 2020-11-12: Rewrite by A. Tuleu
# Changes: 2021-05-04: Moves to lcov
#

include(CMakeParseArguments)


find_program(LCOV_EXECUTABLE lcov REQUIRED)
find_program(GENHTML_EXECUTABLE genhtml REQUIRED)



function(enable_coverage)
	if("${CMAKE_CXX_COMPILER_ID}" MATCHES "(Apple)?[Cc]lang")
		if("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS 3)
			message(FATAL_ERROR "Clang version must be 3.0.0 or greater! Aborting...")
		endif()
	elseif(NOT CMAKE_COMPILER_IS_GNUCXX)
		message(FATAL_ERROR "Compiler is not GNU gcc! Aborting...")
	endif()
	set(COVERAGE_COMPILER_FLAGS "--coverage -fprofile-arcs -ftest-coverage")
	set(CMAKE_BUILD_TYPE "Debug" PARENT_SCOPE)
	set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Forced to Debug mode by enable_covergae()" FORCE)
	set(CMAKE_CXX_FLAGS_DEBUG "-g -O0" PARENT_SCOPE)
	set(CMAKE_C_FLAGS_DEBUG "-g -O0" PARENT_SCOPE)
	if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
		link_libraries(gcov)
	else(CMAKE_C_COMPILER_ID STREQUAL "GNU")
		link_libraries("--coverage")
	endif(CMAKE_C_COMPILER_ID STREQUAL "GNU")

	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COVERAGE_COMPILER_FLAGS}" PARENT_SCOPE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COVERAGE_COMPILER_FLAGS}" PARENT_SCOPE)

endfunction(enable_coverage)


function(setup_target_for_coverage)
	set(options NONE)
	set(oneValueArgs NAME)
	set(multiValueArgs DEPENDENCIES LCOV_OPTIONS)
	cmake_parse_arguments(opts "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	add_custom_target(${opts_NAME}
	                  COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/${opts_NAME}
	                  COMMAND ${LCOV_EXECUTABLE} --directory ${CMAKE_CURRENT_BINARY_DIR}
	                                             --base-directory ${PROJECT_SOURCE_DIR}
	                                             --output-file ${PROJECT_BINARY_DIR}/${opts_NAME}/lcov.info
												 -c
	                                            ${opts_LCOV_OPTIONS}
	                  COMMAND ${GENHTML_EXECUTABLE} ${PROJECT_BINARY_DIR}/${opts_NAME}/lcov.info
	                          --output ${PROJECT_BINARY_DIR}/${opts_NAME}
	                  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
	                  DEPENDS ${opts_DEPENDENCIES}
	                  COMMENT "Producing HTML report for ${opts_NAME}"
	                  )

	add_custom_command(TARGET ${opts_NAME} POST_BUILD
	                   COMMAND ;
	                   COMMENT "Open ./${opts_NAME}/index.html in your browser to view the coverage report."
	                   )

endfunction(setup_target_for_coverage)
