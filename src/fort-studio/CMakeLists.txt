set(SRC_FILES main.cpp
              MainWindow.cpp
              Error.cpp
              ExperimentController.cpp
              AntListWidget.cpp
              ExperimentInfoWidget.cpp
              TaggingWidget.cpp
              Snapshot.cpp
              SnapshotIndexer.cpp
              AntPoseEstimate.cpp
              TagInFramePointer.cpp
			  SnapshotViewer.cpp
			  FramePointerWidget.cpp
			  AntIdentificationWidget.cpp
			  QLongSpinBox.cpp)

set(HDR_FILES MainWindow.hpp
              ExperimentController.hpp
              Error.hpp
              AntListWidget.hpp
              ExperimentInfoWidget.hpp
              TaggingWidget.hpp
              Snapshot.hpp
              SnapshotIndexer.hpp
              AntPoseEstimate.hpp
              TagInFramePointer.hpp
			  SnapshotViewer.hpp
			  FramePointerWidget.hpp
			  AntIdentificationWidget.hpp
			  QLongSpinBox.hpp
			  utils.hpp)

set(UI_FILES MainWindow.ui
             AntListWidget.ui
             ExperimentInfoWidget.ui
			 FramePointerWidget.ui
			 AntIdentificationWidget.ui)

set(RCC_FILES )


set(PROTO_FILES SnapshotCache.proto Point2d.proto Estimate.proto)

protobuf_generate_cpp(PROTO_SRC PROTO_HDR ${PROTO_FILES})
foreach(f ${PROTO_SRC})
	set_property(SOURCE ${f} PROPERTY SKIP_AUTOGEN ON)
endforeach(f ${PROTO_SRC})

foreach(f ${PROTO_HDR})
	set_property(SOURCE ${f} PROPERTY SKIP_AUTOGEN ON)
endforeach(f ${PROTO_HDR})

include_directories(${PROJECT_BINARY_DIR}/src
                    ${PROJECT_BINARY_DIR}/src/myrmidon
                    ${PROJECT_BINARY_DIR}/src/fort-studio
                    ${PROJECT_SOURCE_DIR}/src
                    ${GLOG_INCLUDE_DIRS}
                    ${apriltag_INCLUDE_DIR})

add_executable(fort-studio ${SRC_FILES}
                           ${HDR_FILES}
                           ${UI_FILES}
                           ${RCC_FILES}
                           ${PROTO_SRC}
                           ${PROTO_HDR})

target_link_libraries(fort-studio Qt5::Widgets Qt5::Concurrent myrmidon legacy-tags)
