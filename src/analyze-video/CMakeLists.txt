include_directories(${PROJECT_SOURCE_DIR}/src
                    ${PROJECT_BINARY_DIR}/src)

add_executable(analyze-video main.cpp)
target_link_libraries(analyze-video fort-myrmidon)
