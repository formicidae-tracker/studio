set(SRC_FILES Time.cpp
              Types.cpp
              Experiment.cpp
              Ant.cpp
              Query.cpp
              main.cpp
              )

set(HDR_FILES BindMethods.hpp)

pybind11_add_module(py_fort_myrmidon ${SRC_FILES} ${HDR_FILES})
target_link_libraries(py_fort_myrmidon PRIVATE ${FORT_MYRMIDON_LIBRARIES})
