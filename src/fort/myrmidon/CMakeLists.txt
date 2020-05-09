include_directories(${OPENCV_INCLUDE_DIRS})

include(CheckCXXSourceRuns)

Check_CXX_Source_Runs(
"
#include <system_error>


int main() {
	if (std::error_code(EDOM,std::system_category()) == std::errc::argument_out_of_domain ) {
		return 0;
	}
	return 1;
}
"
MYRMIDON_USE_SYSTEM_CATEGORY)

configure_file(config.h.in myrmidon-config.h)


set(PROTO_FILES priv/proto/Experiment.proto
                priv/proto/AntDescription.proto
                priv/proto/Vector2d.proto
                priv/proto/Shapes.proto
                priv/proto/Time.proto
                priv/proto/TrackingDataDirectory.proto
                priv/proto/ExperimentFile.proto
                priv/proto/EstimateFile.proto
                priv/proto/TagCloseUpCache.proto
                priv/proto/TagFamily.proto
                priv/proto/Measurement.proto
                priv/proto/Zone.proto
                priv/proto/Space.proto
                priv/proto/AntMetadata.proto
                priv/proto/TagStatisticsCache.proto
                )


protobuf_generate_cpp(PROTO_SRC PROTO_HDR ${PROTO_FILES})

foreach(f ${PROTO_SRC})
	set_property(SOURCE ${f} PROPERTY SKIP_AUTOGEN ON)
endforeach(f ${PROTO_SRC})

foreach(f ${PROTO_HDR})
	set_property(SOURCE ${f} PROPERTY SKIP_AUTOGEN ON)
endforeach(f ${PROTO_HDR})

include_directories(${PROJECT_BINARY_DIR}/src
                    ${PROJECT_SOURCE_DIR}/src)

set(HDR_PRIVATE_FILES utils/StringManipulation.hpp
                      priv/ForwardDeclaration.hpp
                      priv/LocatableTypes.hpp
                      priv/TimeUtils.hpp
                      priv/DenseMap.hpp
                      priv/Space.hpp
                      priv/Zone.hpp
                      priv/Experiment.hpp
                      priv/Identifier.hpp
                      priv/Ant.hpp
                      priv/AntShapeType.hpp
                      priv/Identification.hpp
                      priv/SegmentIndexer.hpp
                      priv/TrackingDataDirectory.hpp
                      priv/RawFrame.hpp
                      priv/DeletedReference.hpp
                      priv/ExperimentReadWriter.hpp
                      utils/NotYetImplemented.hpp
                      priv/Isometry2D.hpp
                      priv/TimeValid.hpp
                      priv/MovieSegment.hpp
                      priv/Types.hpp
                      priv/Shape.hpp
                      priv/Capsule.hpp
                      priv/Circle.hpp
                      priv/Polygon.hpp
                      priv/FrameReference.hpp
                      priv/TagCloseUp.hpp
                      priv/AntMetadata.hpp
                      priv/proto/FileReadWriter.hpp
                      priv/proto/ExperimentReadWriter.hpp
                      priv/proto/TDDCache.hpp
                      priv/proto/IOUtils.hpp
                      utils/Checker.hpp
                      utils/Defer.hpp
                      priv/AntPoseEstimate.hpp
                      priv/Measurement.hpp
                      priv/KDTree.hpp
                      priv/KDTree.impl.hpp
                      priv/InteractionSolver.hpp
                      priv/TagStatistics.hpp
                      )


set(HDR_PUBLIC_FILES ForwardDeclaration.hpp
                     Types.hpp
                     Color.hpp
                     Time.hpp
                     Identification.hpp
                     Ant.hpp
                     Zone.hpp
                     Space.hpp
                     Experiment.hpp
                     utils/FileSystem.hpp)

set(SRC_PRIVATE_FILES utils/StringManipulation.cpp
                      priv/TimeValid.cpp
                      priv/Types.cpp
                      priv/LocatableTypes.cpp
                      priv/TimeUtils.cpp
                      priv/DenseMap.cpp
                      priv/Space.cpp
                      priv/Zone.cpp
                      priv/Experiment.cpp
                      priv/Identifier.cpp
                      priv/Ant.cpp
                      priv/AntShapeType.cpp
                      priv/SegmentIndexer.cpp
                      priv/Identification.cpp
                      priv/TrackingDataDirectory.cpp
                      priv/RawFrame.cpp
                      priv/DeletedReference.cpp
                      priv/ExperimentReadWriter.cpp
                      priv/Isometry2D.cpp
                      priv/MovieSegment.cpp
                      priv/Shape.cpp
                      priv/Capsule.cpp
                      priv/Circle.cpp
                      priv/Polygon.cpp
                      priv/FrameReference.cpp
                      priv/TagCloseUp.cpp
                      priv/AntMetadata.cpp
                      priv/proto/FileReadWriter.cpp
                      priv/proto/ExperimentReadWriter.cpp
                      priv/proto/TDDCache.cpp
                      priv/proto/IOUtils.cpp
                      utils/Checker.cpp
                      utils/Defer.cpp
                      priv/AntPoseEstimate.cpp
                      priv/Measurement.cpp
                      priv/KDTree.cpp
                      priv/InteractionSolver.cpp
                      priv/TagStatistics.cpp
                      )

set(SRC_FILES ForwardDeclaration.cpp
              Types.cpp
              Color.cpp
              Time.cpp
              Identification.cpp
              Ant.cpp
              Zone.cpp
              Space.cpp
              Experiment.cpp
	          )

set(COMMON_UTEST_SRC_FILES TestSetup.cpp
                           UtilsUTest.cpp
                           )

set(COMMON_UTEST_HDR_FILES TestSetup.hpp
                           UtilsUTest.hpp
                           )


set(SRC_UTEST_FILES main-check.cpp
                    TimeUTest.cpp
                    utils/NotYetImplementedUTest.cpp
                    priv/UtilsUTest.cpp
                    priv/DenseMapUTest.cpp
                    priv/TimedMapUTest.cpp
                    priv/SpaceUTest.cpp
                    priv/ZoneUTest.cpp
                    priv/ExperimentUTest.cpp
                    priv/TimeUtilsUTest.cpp
                    priv/TrackingDataDirectoryUTest.cpp
                    priv/AntUTest.cpp
                    priv/IdentifierUTest.cpp
                    priv/IdentificationUTest.cpp
                    priv/RawFrameUTest.cpp
                    priv/Isometry2DUTest.cpp
                    priv/SegmentIndexerUTest.cpp
                    priv/TimeValidUTest.cpp
                    priv/MovieSegmentUTest.cpp
                    priv/FrameReferenceUTest.cpp
                    priv/TagCloseUpUTest.cpp
                    priv/proto/IOUtilsUTest.cpp
                    priv/proto/FileReadWriterUTest.cpp
                    priv/proto/ExperimentReadWriterUTest.cpp
                    priv/proto/TDDCacheUTest.cpp
                    priv/AntPoseEstimateUTest.cpp
                    priv/MeasurementUTest.cpp
                    priv/ShapeUTest.cpp
                    priv/CircleUTest.cpp
                    priv/CapsuleUTest.cpp
                    priv/PolygonUTest.cpp
                    priv/KDTreeUTest.cpp
                    priv/AntMetadataUTest.cpp
                    priv/InteractionSolverUTest.cpp
                    priv/TagStatisticsUTest.cpp
                    )



set(HDR_UTEST_FILES TimeUTest.hpp
                    utils/NotYetImplementedUTest.hpp
                    priv/UtilsUTest.hpp
                    priv/DenseMapUTest.hpp
                    priv/TimedMapUTest.hpp
                    priv/SpaceUTest.hpp
                    priv/ZoneUTest.hpp
                    priv/ExperimentUTest.hpp
                    priv/TimeUtilsUTest.hpp
                    priv/TrackingDataDirectoryUTest.hpp
                    priv/AntUTest.hpp
                    priv/IdentifierUTest.hpp
                    priv/IdentificationUTest.hpp
                    priv/RawFrameUTest.hpp
                    priv/Isometry2DUTest.hpp
                    priv/SegmentIndexerUTest.hpp
                    priv/TimeValidUTest.hpp
                    priv/MovieSegmentUTest.hpp
                    priv/FrameReferenceUTest.hpp
                    priv/TagCloseUpUTest.hpp
                    priv/proto/IOUtilsUTest.hpp
                    priv/proto/FileReadWriterUTest.hpp
                    priv/proto/ExperimentReadWriterUTest.hpp
                    priv/proto/TDDCacheUTest.hpp
                    priv/AntPoseEstimateUTest.hpp
                    priv/MeasurementUTest.hpp
                    priv/ShapeUTest.hpp
                    priv/CircleUTest.hpp
                    priv/CapsuleUTest.hpp
                    priv/PolygonUTest.hpp
                    priv/KDTreeUTest.hpp
                    priv/AntMetadataUTest.hpp
                    priv/InteractionSolverUTest.hpp
                    priv/TagStatisticsUTest.hpp
                    )

add_library(fort-myrmidon SHARED ${SRC_FILES}
                                 ${SRC_PRIVATE_FILES}
                                 ${HDR_PRIVATE_FILES}
                                 ${HDR_PUBLIC_FILES}
                                 ${PROTO_HDR}
                                 ${PROTO_SRC})

target_link_libraries(fort-myrmidon ${PROTOBUF_LIBRARIES}
                                    fort-hermes-cpp
                                    ${CXXFS_LIBRARY}
                                    fort-tags
                                    ${OpenCV_LIBS}
                                    )

set_target_properties(fort-myrmidon PROPERTIES
                                    VERSION ${VERSION_API}
                                    SOVERSION ${VERSION_ABI})

install(TARGETS fort-myrmidon DESTINATION lib)
install(FILES ${HDR_PUBLIC_FILES} DESTINATION include/fort/myrmidon)

add_library(myrmidon-utest-common STATIC ${COMMON_UTEST_SRC_FILES}
                                         ${COMMON_UTEST_HDR_FILES}
                                         ${PROTO_HDR}
                                         )
target_link_libraries(myrmidon-utest-common gtest gmock fort-myrmidon)

add_executable(myrmidon-tests ${SRC_UTEST_FILES}
                              ${HDR_UTEST_FILES}
                              )
target_link_libraries(myrmidon-tests fort-myrmidon
                                     myrmidon-utest-common
                                     gtest)

add_test(NAME myrmidon-tests COMMAND myrmidon-tests)
add_dependencies(check myrmidon-tests)

foreach(h ${HDR_PUBLIC_FILES})
	list(APPEND MYRMIDON_API_DOC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${h})
endforeach(h ${HDR_PUBLIC_FILES})
set(MYRMIDON_API_DOC_FILES ${MYRMIDON_API_DOC_FILES} PARENT_SCOPE)

foreach(h ${HDR_PRIVATE_FILES})
	list(APPEND MYRMIDON_PRIVATE_DOC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${h})
endforeach(h ${HDR_PRIVATE_FILES})
set(MYRMIDON_PRIVATE_DOC_FILES ${MYRMIDON_PRIVATE_DOC_FILES} PARENT_SCOPE)

add_executable(benchmarks benchmark/main.cpp)
target_link_libraries(benchmarks fort-myrmidon)
