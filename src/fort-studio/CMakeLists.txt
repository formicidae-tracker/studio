set(SRC_FILES Format.cpp
              Bridge.cpp
              UniverseBridge.cpp
			  MeasurementBridge.cpp
			  GlobalPropertyBridge.cpp
			  IdentifierBridge.cpp
			  SelectedAntBridge.cpp
			  SelectedIdentificationBridge.cpp
			  ExperimentBridge.cpp
			  TagFamilyComboBox.cpp
			  Logger.cpp
			  GlobalPropertyWidget.cpp
			  UniverseEditorWidget.cpp
              # Error.cpp
              # AntListWidget.cpp
              # TaggingWidget.cpp
              # Snapshot.cpp
              # SnapshotIndexer.cpp
              # AntPoseEstimate.cpp
              # TagInFramePointer.cpp
			  # SnapshotViewer.cpp
			  # FramePointerWidget.cpp
			  # AntIdentificationWidget.cpp
			  # QLongSpinBox.cpp
			  # TagExtractor.cpp
			  MainWindow.cpp
			  )

set(HDR_FILES Format.hpp
			  Bridge.hpp
			  UniverseBridge.hpp
			  MeasurementBridge.hpp
			  GlobalPropertyBridge.hpp
			  IdentifierBridge.hpp
			  SelectedAntBridge.hpp
			  SelectedIdentificationBridge.hpp
			  ExperimentBridge.hpp
			  TagFamilyComboBox.hpp
			  Logger.hpp
			  GlobalPropertyWidget.hpp
			  UniverseEditorWidget.hpp
              # Error.hpp
              # AntListWidget.hpp
              # TaggingWidget.hpp
              # Snapshot.hpp
              # SnapshotIndexer.hpp
              # AntPoseEstimate.hpp
              # TagInFramePointer.hpp
			  # SnapshotViewer.hpp
			  # FramePointerWidget.hpp
			  # AntIdentificationWidget.hpp
			  # QLongSpinBox.hpp
			  # utils.hpp
			  # TagExtractor.hpp
			  MainWindow.hpp
			  )


set(UI_FILES GlobalPropertyWidget.ui
             UniverseEditorWidget.ui
             MainWindow.ui
             LoggerWidget.ui
             # AntListWidget.ui
			 # FramePointerWidget.ui
			 # AntIdentificationWidget.ui
			 # TaggingWidget.ui
			 )

set(RCC_FILES )


set(SRC_UTEST_FILES main-check.cpp
                    ExperimentBridgeUTest.cpp
                    GlobalPropertyUTest.cpp
                    )

set(HDR_UTEST_FILES ExperimentBridgeUTest.hpp
                    GlobalPropertyUTest.hpp
                    )

qt5_wrap_ui(UI_HEADERS ${UI_FILES})

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

include_directories(${PROJECT_BINARY_DIR}/src
                    ${PROJECT_BINARY_DIR}/src/myrmidon
                    ${PROJECT_BINARY_DIR}/src/fort-studio
                    ${PROJECT_SOURCE_DIR}/src
                    ${GLOG_INCLUDE_DIRS}
                    ${apriltag_INCLUDE_DIR})

add_library(fort-studio-common STATIC ${SRC_FILES}
                                      ${HDR_FILES}
                                      ${UI_HEADERS}
                                      ${MOC_FILES}
                                      ${PROTO_SRC}
                                      ${PROTO_HDR}
                                      )

target_link_libraries(fort-studio-common Qt5::Widgets Qt5::Concurrent myrmidon fort-tags)

add_executable(fort-studio main.cpp)

target_link_libraries(fort-studio fort-studio-common)

add_executable(fort-studio-tests ${SRC_UTEST_FILES}
                                 ${HDR_UTEST_FILES}
                                 )

target_link_libraries(fort-studio-tests fort-studio-common
                                        myrmidon-utest-common
                                        Qt5::Test
                                        gtest
                                        gmock
                                        )

add_test(NAME fort-studio-tests COMMAND fort-studio-tests)
add_dependencies(check fort-studio-tests)
