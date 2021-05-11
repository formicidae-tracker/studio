function(version_from_git)
	find_program(GIT_EXECUTABLE git REQUIRED)

	execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags
	                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	                RESULT_VARIABLE   git_result
	                OUTPUT_VARIABLE   git_describe
	                ERROR_VARIABLE    git_error
	                OUTPUT_STRIP_TRAILING_WHITESPACE
	                ERROR_STRIP_TRAILING_WHITESPACE
	                )

	if(NOT git_result EQUAL 0)
		message(FATAL_ERROR "Git describe failed: ${git_error}")
	endif(NOT git_result EQUAL 0)

	execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0
	                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	                RESULT_VARIABLE   git_result
	                OUTPUT_VARIABLE   git_tag
	                ERROR_VARIABLE    git_error
	                OUTPUT_STRIP_TRAILING_WHITESPACE
	                ERROR_STRIP_TRAILING_WHITESPACE
	                )

    if(NOT git_result EQUAL 0)
		message(FATAL_ERROR "Git describe failed: ${git_error}")
	endif(NOT git_result EQUAL 0)



	if(git_tag MATCHES "^v(0|[1-9][0-9]*)[.](0|[1-9][0-9]*)[.](0|[1-9][0-9]*)(-[.0-9A-Za-z-]+)?([+][.0-9A-Za-z-]+)?$")
		set(version_major ${CMAKE_MATCH_1})
		set(version_minor ${CMAKE_MATCH_2})
		set(version_patch ${CMAKE_MATCH_3})
		set(identifiers   ${CMAKE_MATCH_4})
		set(metadata      ${CMAKE_MATCH_5})
	else(git_tag MATCHES "^v(0|[1-9][0-9]*)[.](0|[1-9][0-9]*)[.](0|[1-9][0-9]*)(-[.0-9A-Za-z-]+)?([+][.0-9A-Za-z-]+)?$")
		message( FATAL_ERROR "Git tag isn't valid semantic version: [${git_tag}]" )
	endif(git_tag MATCHES "^v(0|[1-9][0-9]*)[.](0|[1-9][0-9]*)[.](0|[1-9][0-9]*)(-[.0-9A-Za-z-]+)?([+][.0-9A-Za-z-]+)?$")

	if(NOT git_tag STREQUAL git_describe)
		string( REGEX MATCH "-([0-9]+)-g([0-9a-f]+)$" git_hash "${git_describe}")
		set(git_ahead ${CMAKE_MATCH_1})
		set(git_hash ${CMAKE_MATCH_2})

		if("${metadata}" STREQUAL "")
			set(metadata "+${git_ahead}.${git_hash}")
		else("${metadata}" STREQUAL "")
			set(metadata "${metadata}.${git_ahead}.${git_hash}")
		endif("${metadata}" STREQUAL "")

	endif(NOT git_tag STREQUAL git_describe)

	set(semver ${version_major}.${version_minor}.${version_patch}${identifiers}${metadata})
	set(version ${version_major}.${version_minor}.${version_patch})
	set(CMAKE_PROJECT_VERSION ${version} PARENT_SCOPE)
	set(PROJECT_VERSION ${version} PARENT_SCOPE)
	set(${CMAKE_PROJECT_NAME}_VERSION ${version} PARENT_SCOPE)
	set(PROJECT_SEMVER ${semver} PARENT_SCOPE)
	set(${CMAKE_PROJECT_NAME}_SEMVER ${semver} PARENT_SCOPE)
	set(PROJECT_VERSION_MAJOR ${version_major} PARENT_SCOPE)
	set(PROJECT_VERSION_MINOR ${version_minor} PARENT_SCOPE)
	set(PROJECT_VERSION_PATCH ${version_patch} PARENT_SCOPE)

	set(PROJECT_VERSION_API ${version_major}.${version_minor} PARENT_SCOPE)
	if(${version_major} EQUAL 0)
		set(PROJECT_VERSION_ABI ${version_major}.${version_minor} PARENT_SCOPE)
	else(${version_major} EQUAL 0)
		set(PROJECT_VERSION_ABI ${version_major} PARENT_SCOPE)
	endif(${version_major} EQUAL 0)

endfunction(version_from_git)
