find_program(CLDOC_EXECUTABLE cldoc)

if(NOT CLDOC_EXECUTABLE)
	message(FATAL_ERROR "Could not find cldoc")
endif(NOT CLDOC_EXECUTABLE)

add_custom_target(docs
	COMMAND ${CLDOC_EXECUTABLE} generate -I/usr/include/eigen3 -- --report --output ${PROJECT_SOURCE_DIR}/docs/ --language c++ --merge docs/cldoc src/myrmidon/priv/Identifier.hpp
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
