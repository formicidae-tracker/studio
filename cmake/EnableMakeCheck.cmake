# Module to enable the make check target
# 
# it defines this two commands :
#   enable_make_check() - enable make check target
#     enable the make check target.  
#   add_check_test(NAME name 
#                  FILES files 
#                  [LIBRARIES lib1 lib2]
#                  [INCLUDE_DIRS dir1 dir2 dir3] )
#     add a new test to add to the make check target:
#       - NAME : name of the test
#       - FILES : files to compile
#       - LIBRARIES : library to link the test executable against
#       - INCLUDE_DIRS : where to find the needed include file
#       


function(ENABLE_MAKE_CHECK)
	set(MAKE_CHECK_TEST_COMMAND ${CMAKE_CTEST_COMMAND} -V)
	add_custom_target(check COMMAND ${MAKE_CHECK_TEST_COMMAND})
endfunction(ENABLE_MAKE_CHECK)

include(CMakeParseArguments)

function(ADD_CHECK_TEST)
	set(oneValueArgs NAME OPTIONS)
	set(multiValueArgs FILES LIBRARIES INCLUDE_DIRS)
	cmake_parse_arguments(ARGS "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	if(NOT ARGS_NAME)
		message(FATAL_ERROR "You should provide a NAME to add_check_test")
	endif(NOT ARGS_NAME)
	
	if(NOT ARGS_FILES)
		message(FATAL_ERROR "You should provide at least one FILES to add_check_test")
	endif(NOT ARGS_FILES)
	set(target_name ${ARGS_NAME}-tests)
	if(ARGS_INCLUDE_DIRS)
		include_directories(${ARGS_INCLUDE_DIRS})
	endif(ARGS_INCLUDE_DIRS)
	add_executable(${target_name} EXCLUDE_FROM_ALL ${ARGS_FILES})
	target_link_libraries(${target_name} ${ARGS_LIBRARIES})
	if(ARGS_OPTIONS)
		add_test(NAME ${target_name} COMMAND ${target_name} ${ARGS_OPTIONS})
	else(ARGS_OPTIONS)
		add_test(NAME ${target_name} COMMAND ${target_name})
	endif(ARGS_OPTIONS)
	add_dependencies(check ${target_name})
endfunction(ADD_CHECK_TEST)
