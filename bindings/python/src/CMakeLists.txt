set(SRC_FILES Experiment.cpp)

pybind11_add_module(py_fort_myrmidon ${SRC_FILES})
target_link_libraries(py_fort_myrmidon PRIVATE ${FORT_MYRMIDON_LIBRARIES})
