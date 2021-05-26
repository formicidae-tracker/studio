option(BUILD_PYTHON "Build python bindings" Off)

set(SRC_FILES Experiment.cpp)

if(BUILD_PYTHON)


	include_directories(${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)

	find_package(pybind11 REQUIRED)
	pybind11_add_module(py_fort_myrmidon ${SRC_FILES})
	target_link_libraries(py_fort_myrmidon PRIVATE fort-myrmidon)


endif(BUILD_PYTHON)
