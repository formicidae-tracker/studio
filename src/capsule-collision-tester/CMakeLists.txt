set(SRC_FILES main.cpp
	          CapsuleCollisionDetecter.cpp)

set(HDR_FILES CapsuleCollisionDetecter.hpp)

set(UI_FILES CapsuleCollisionDetecter.ui)

qt5_wrap_ui(UI_HEADERS ${UI_FILES})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(MOC_FILES )
foreach(H ${HDR_FILES})
	file(READ ${CMAKE_CURRENT_SOURCE_DIR}/${H} HFILE)
	foreach(m ${CMAKE_AUTOMOC_MACRO_NAMES})
		string(REGEX MATCH ${m} NEED_MOC ${HFILE})
		if(NOT "${NEED_MOC}" STREQUAL "")
			qt5_generate_moc(${H} ${H}.moc.cpp)
			list(APPEND MOC_FILES ${H}.moc.cpp)
		endif(NOT "${NEED_MOC}" STREQUAL "")
	endforeach(m ${AUTOMOC_MACRO_NAMES})
endforeach(H ${HDR_FILES})


add_executable(capsule-collision-tester ${SRC_FILES}
                                        ${HDR_FILES}
                                        ${MOC_FILES}
                                        ${UI_HEADERS}
                                        )

target_link_libraries(capsule-collision-tester Qt5::Widgets)
