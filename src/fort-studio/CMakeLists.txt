set(SRC_FILES main.cpp
			  UniverseBridge.cpp
			  MeasurementBridge.cpp
			  ExperimentBridge.cpp
			  IdentifierBridge.cpp
			  SelectedAntBridge.cpp
			  SelectedIdentificationBridge.cpp
			  ExperimentController.cpp
			  TagFamilyComboBox.cpp
			  ExperimentEditorWidget.cpp
              # MainWindow.cpp
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
			  )

set(HDR_FILES UniverseBridge.hpp
			  MeasurementBridge.hpp
			  ExperimentBridge.hpp
			  IdentifierBridge.hpp
			  SelectedAntBridge.hpp
			  SelectedIdentificationBridge.hpp
			  ExperimentController.hpp
			  TagFamilyComboBox.hpp
			  ExperimentEditorWidget.hpp
			  # MainWindow.hpp
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
			  )

set(UI_FILES MainWindow.ui
             AntListWidget.ui
             ExperimentEditorWidget.ui
			 FramePointerWidget.ui
			 AntIdentificationWidget.ui
			 TaggingWidget.ui
			 )

set(RCC_FILES )

#pinclude_directories(${CMAKE_CURRENT_BINARY_DIR})

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

add_executable(fort-studio ${SRC_FILES}
                           ${HDR_FILES}
                           ${UI_HEADERS}
                           ${MOC_FILES}
                           ${PROTO_SRC}
                           ${PROTO_HDR})

target_link_libraries(fort-studio Qt5::Widgets Qt5::Concurrent myrmidon fort-tags)
