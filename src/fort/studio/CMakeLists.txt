include(FindGLog)

option(DEBUG_VIDEO_PLAYER Off "Debugs the video player with expensive STDERR output")
if(NOT DEBUG_VIDEO_PLAYER)
	add_definitions("-DFORT_STUDIO_VIDEO_PLAYER_NDEBUG=1")
endif(NOT DEBUG_VIDEO_PLAYER)

option(DEBUG_CONCURRENT_LOADER Off "Debugs the concurrent Frame loader with expensive output")
if(NOT DEBUG_CONCURRENT_LOADER)
	add_definitions("-DFORT_STUDIO_CONC_LOADER_NDEBUG=1")
endif(NOT DEBUG_CONCURRENT_LOADER)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SRC_FILES Format.cpp
              MyrmidonTypes/Conversion.cpp
              bridge/AntGlobalModel.cpp
              bridge/Bridge.cpp
              bridge/UniverseBridge.cpp
              bridge/MeasurementBridge.cpp
              bridge/IdentifierBridge.cpp
              bridge/AntDisplayBridge.cpp
              bridge/TagCloseUpBridge.cpp
			  bridge/GlobalPropertyBridge.cpp
			  bridge/SelectedAntBridge.cpp
			  bridge/ExperimentBridge.cpp
			  bridge/IdentifiedFrameConcurrentLoader.cpp
			  bridge/AntShapeTypeBridge.cpp
			  bridge/AntMetadataBridge.cpp
			  bridge/MovieBridge.cpp
			  bridge/ZoneBridge.cpp
			  bridge/StatisticsBridge.cpp
			  widget/TrackingDataDirectoryLoader.cpp
			  widget/base/TagFamilyComboBox.cpp
			  widget/base/ColorComboBox.cpp
			  widget/base/TimeEditorWidget.cpp
			  widget/base/QLongSpinBox.cpp
			  widget/base/QActionButton.cpp
			  widget/vectorgraphics/VectorialView.cpp
			  widget/vectorgraphics/VectorialScene.cpp
			  widget/vectorgraphics/Handle.cpp
			  widget/vectorgraphics/Shape.cpp
			  widget/vectorgraphics/Vector.cpp
			  widget/vectorgraphics/Capsule.cpp
			  widget/vectorgraphics/Polygon.cpp
			  widget/vectorgraphics/Circle.cpp
			  widget/Logger.cpp
			  widget/GlobalPropertyWidget.cpp
			  widget/UniverseEditorWidget.cpp
			  widget/SpaceChoiceDialog.cpp
			  widget/MeasurementTypeWidget.cpp
			  widget/AntListWidget.cpp
			  widget/AntShapeTypeEditorWidget.cpp
			  widget/FrameFinderDialog.cpp
			  widget/AntMetadataWidget.cpp
			  widget/TrackingVideoFrame.cpp
			  widget/TrackingVideoWidget.cpp
			  widget/TrackingVideoPlayer.cpp
			  widget/TrackingVideoControl.cpp
			  widget/ZonesEditorWidget.cpp
			  widget/CloneShapeDialog.cpp
			  widget/TagStatisticsWidget.cpp
			  widget/CloseUpScroller.cpp
			  widget/TagCloseUpExplorer.cpp
			  widget/IdentificationListWidget.cpp
			  workspace/Workspace.cpp
			  workspace/TagStatisticsWorkspace.cpp
			  workspace/GeneralWorkspace.cpp
			  workspace/IdentificationWorkspace.cpp
			  workspace/AntGeometryWorkspace.cpp
			  workspace/ZoningWorkspace.cpp
			  workspace/AntMetadataWorkspace.cpp
			  workspace/VisualizationWorkspace.cpp
			  MainWindow.cpp
			  )

set(HDR_FILES Format.hpp
              MyrmidonTypes/Ant.hpp
              MyrmidonTypes/AntMetadata.hpp
              MyrmidonTypes/AntShapeType.hpp
              MyrmidonTypes/Color.hpp
              MyrmidonTypes/Conversion.hpp
              MyrmidonTypes/Experiment.hpp
              MyrmidonTypes/Identification.hpp
              MyrmidonTypes/Measurement.hpp
              MyrmidonTypes/MovieSegment.hpp
              MyrmidonTypes/Space.hpp
              MyrmidonTypes/TagCloseUp.hpp
              MyrmidonTypes/TrackingDataDirectory.hpp
              MyrmidonTypes/Types.hpp
              MyrmidonTypes/Zone.hpp
              Utils.hpp
              bridge/AntGlobalModel.cpp
			  bridge/Bridge.hpp
			  bridge/UniverseBridge.hpp
			  bridge/MeasurementBridge.hpp
			  bridge/TagCloseUpBridge.hpp
			  bridge/GlobalPropertyBridge.hpp
			  bridge/IdentifierBridge.hpp
			  bridge/AntDisplayBridge.hpp
			  bridge/SelectedAntBridge.hpp
			  bridge/ExperimentBridge.hpp
			  bridge/IdentifiedFrameConcurrentLoader.hpp
			  bridge/AntShapeTypeBridge.hpp
			  bridge/AntMetadataBridge.hpp
			  bridge/MovieBridge.hpp
			  bridge/ZoneBridge.hpp
			  bridge/StatisticsBridge.hpp
			  widget/TrackingDataDirectoryLoader.cpp
			  widget/base/TagFamilyComboBox.hpp
			  widget/base/ColorComboBox.hpp
			  widget/base/TimeEditorWidget.hpp
			  widget/base/QLongSpinBox.hpp
			  widget/base/QActionButton.hpp
			  widget/vectorgraphics/VectorialView.hpp
			  widget/vectorgraphics/VectorialScene.hpp
			  widget/vectorgraphics/Handle.hpp
			  widget/vectorgraphics/Shape.hpp
			  widget/vectorgraphics/Vector.hpp
			  widget/vectorgraphics/Capsule.hpp
			  widget/vectorgraphics/Polygon.hpp
			  widget/vectorgraphics/Circle.hpp
			  widget/Logger.hpp
			  widget/GlobalPropertyWidget.hpp
			  widget/UniverseEditorWidget.hpp
			  widget/SpaceChoiceDialog.hpp
			  widget/MeasurementTypeWidget.hpp
			  widget/AntListWidget.hpp
			  widget/AntShapeTypeEditorWidget.hpp
			  widget/FrameFinderDialog.hpp
			  widget/AntMetadataWidget.hpp
			  widget/TrackingVideoFrame.hpp
			  widget/TrackingVideoWidget.hpp
			  widget/TrackingVideoPlayer.hpp
			  widget/TrackingVideoControl.hpp
			  widget/ZonesEditorWidget.hpp
			  widget/CloneShapeDialog.hpp
			  widget/TagStatisticsWidget.hpp
			  widget/CloseUpScroller.hpp
			  widget/TagCloseUpExplorer.hpp
			  widget/IdentificationListWidget.hpp
			  workspace/Workspace.hpp
			  workspace/TagStatisticsWorkspace.cpp
			  workspace/GeneralWorkspace.cpp
			  workspace/IdentificationWorkspace.cpp
			  workspace/AntGeometryWorkspace.cpp
			  workspace/ZoningWorkspace.cpp
			  workspace/AntMetadataWorkspace.cpp
			  workspace/VisualizationWorkspace.cpp
			  MainWindow.hpp
			  )


set(UI_FILES widget/GlobalPropertyWidget.ui
             widget/UniverseEditorWidget.ui
             widget/LoggerWidget.ui
             widget/SpaceChoiceDialog.ui
             widget/MeasurementTypeWidget.ui
             widget/AntListWidget.ui
             widget/base/TimeEditorWidget.ui
             widget/AntShapeTypeEditorWidget.ui
             widget/FrameFinderDialog.ui
             widget/AntMetadataWidget.ui
             widget/TrackingVideoControl.ui
             widget/ZonesEditorWidget.ui
             widget/CloneShapeDialog.ui
             widget/TagStatisticsWidget.ui
             widget/TagCloseUpExplorer.ui
             widget/IdentificationListWidget.ui
             workspace/GeneralWorkspace.ui
             workspace/TagStatisticsWorkspace.ui
             workspace/IdentificationWorkspace.ui
             workspace/AntGeometryWorkspace.ui
             workspace/ZoningWorkspace.ui
             workspace/AntMetadataWorkspace.ui
             workspace/VisualizationWorkspace.ui
             MainWindow.ui
			 )

set(RCC_FILES resources/resources.qrc)


set(SRC_UTEST_FILES utest/main.cpp
                    utest/ExperimentBridgeUTest.cpp
                    utest/GlobalPropertyUTest.cpp
                    utest/UniverseUTest.cpp
                    utest/MeasurementUTest.cpp
                    utest/IdentifierUTest.cpp
                    utest/AntShapeTypeUTest.cpp
                    utest/AntMetadataUTest.cpp
                    utest/ZoneUTest.cpp
                    )

set(HDR_UTEST_FILES utest/ExperimentBridgeUTest.hpp
                    utest/GlobalPropertyUTest.hpp
                    utest/UniverseUTest.hpp
                    utest/MeasurementUTest.hpp
                    utest/IdentifierUTest.hpp
                    utest/AntShapeTypeUTest.hpp
                    utest/AntMetadataUTest.hpp
                    utest/ZoneUTest.hpp
                    )

include_directories(${PROJECT_BINARY_DIR}/src
                    ${PROJECT_BINARY_DIR}/src/myrmidon
                    ${PROJECT_BINARY_DIR}/src/fort-studio
                    ${PROJECT_SOURCE_DIR}/src
                    ${GLOG_INCLUDE_DIRS}
                    ${apriltag_INCLUDE_DIR}
                    ${GLOG_INCLUDE_DIRS})

add_library(fort-studio-common STATIC ${SRC_FILES}
                                      ${HDR_FILES}
                                      ${UI_FILES}
                                      ${RCC_FILES}
                                      )

target_link_libraries(fort-studio-common Qt5::Widgets
                                         Qt5::Concurrent
                                         Qt5::Charts
                                         fort-myrmidon
                                         fort-tags
                                         Threads::Threads
                                         ${GLOG_LIBRARIES})

add_executable(fort-studio main.cpp)

target_link_libraries(fort-studio fort-studio-common)

set_target_properties(fort-studio PROPERTIES VERSION ${PROJECT_VERSION}
                                             SOVERSION ${VERSION_ABI})

install(TARGETS fort-studio DESTINATION bin)

add_executable(fort-studio-tests ${SRC_UTEST_FILES}
                                 ${HDR_UTEST_FILES}
                                 )

set_target_properties(fort-studio-tests PROPERTIES AUTOUIC_SEARCH_PATHS ${PROJECT_SOURCE_DIR}/src/fort/studio/widget)

target_link_libraries(fort-studio-tests fort-studio-common
                                        myrmidon-utest-common
                                        Qt5::Test
                                        gtest
                                        gmock
                                        )

add_test(NAME fort-studio-tests COMMAND fort-studio-tests)
add_dependencies(check fort-studio-tests)
