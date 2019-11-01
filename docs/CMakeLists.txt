find_program(CLDOC_EXECUTABLE cldoc)

if(NOT CLDOC_EXECUTABLE)
	message(FATAL_ERROR "Could not find cldoc")
endif(NOT CLDOC_EXECUTABLE)


add_custom_target(docs-api
	COMMAND ${CLDOC_EXECUTABLE}
	        generate
	        -I/usr/include/eigen3
	        --
	        --report
	        --output
	        ${PROJECT_SOURCE_DIR}/docs/api
	        --language c++
	        --merge docs/cldoc/api
	        ${MYRMIDON_API_DOC_FILES}
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_custom_target(docs-private
	COMMAND ${CLDOC_EXECUTABLE}
	        generate
	        -std=c++17
	        -I/usr/include/eigen3
	        -I${PROJECT_BINARY_DIR}/src/
	        -I${PROJECT_SOURCE_DIR}/src/
	        --
	        --report
	        --output
	        ${PROJECT_SOURCE_DIR}/docs/private
	        --language c++
	        --merge docs/cldoc/private
	        --merge docs/cldoc/api
	        ${MYRMIDON_PRIVATE_DOC_FILES}
	        ${MYRMIDON_API_DOC_FILES}
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
