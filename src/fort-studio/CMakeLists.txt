set(SRC_FILES main.cpp)

set(HDR_FILES )

set(UI_FILES )

include_directories(${PROJECT_BINARY_DIR}/src ${PROJECT_SOURCE_DIR}/src)

add_executable(fort-studio ${SRC_FILES} ${HDR_FILES})
