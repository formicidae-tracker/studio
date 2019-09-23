set(SRC_FILES main.cpp
              MainWindow.cpp
              Error.cpp
              Experiment.cpp
              AntListWidget.cpp
              ExperimentInfoWidget.cpp)

set(HDR_FILES MainWindow.hpp
              Experiment.hpp
              Error.hpp
              AntListWidget.hpp
              ExperimentInfoWidget.hpp)

set(UI_FILES MainWindow.ui
             AntListWidget.ui
             ExperimentInfoWidget.ui)

set(RCC_FILES )



include_directories(${PROJECT_BINARY_DIR}/src
                    ${PROJECT_BINARY_DIR}/src/myrmidon
                    ${PROJECT_SOURCE_DIR}/src
                    ${GLOG_INCLUDE_DIRS})

add_executable(fort-studio ${SRC_FILES} ${HDR_FILES} ${UI_FILES} ${RCC_FILES})

target_link_libraries(fort-studio Qt5::Widgets myrmidon)
