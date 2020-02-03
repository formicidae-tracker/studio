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
                priv/proto/AntMetadata.proto
                priv/proto/Vector2d.proto
                priv/proto/Time.proto
                priv/proto/TrackingDataDirectory.proto
                priv/proto/ExperimentFile.proto
                priv/proto/EstimateFile.proto
                priv/proto/TagCloseUpCache.proto
                priv/proto/TagFamily.proto
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

set(HDR_PRIVATE_FILES priv/ForwardDeclaration.hpp
                      priv/LocatableTypes.hpp
                      priv/TimeUtils.hpp
                      priv/Experiment.hpp
                      priv/Identifier.hpp
                      priv/Ant.hpp
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
                      priv/FrameReference.hpp
                      priv/TagCloseUp.hpp
                      priv/SnapshotCache.hpp
                      priv/proto/FileReadWriter.hpp
                      priv/proto/ExperimentReadWriter.hpp
                      priv/proto/TDDCache.hpp
                      priv/proto/IOUtils.hpp
                      utils/Checker.hpp
                      utils/Defer.hpp
                      )


set(HDR_PUBLIC_FILES Frame.hpp
                     Experiment.hpp
                     Ant.hpp
                     Time.hpp
                     utils/FileSystem.hpp)

set(SRC_PRIVATE_FILES priv/TimeValid.cpp
                      priv/LocatableTypes.cpp
                      priv/TimeUtils.cpp
                      priv/Experiment.cpp
                      priv/Identifier.cpp
                      priv/Ant.cpp
                      priv/SegmentIndexer.cpp
                      priv/Identification.cpp
                      priv/TrackingDataDirectory.cpp
                      priv/RawFrame.cpp
                      priv/DeletedReference.cpp
                      priv/ExperimentReadWriter.cpp
                      priv/Isometry2D.cpp
                      priv/MovieSegment.cpp
                      priv/Shape.cpp
                      priv/FrameReference.cpp
                      priv/TagCloseUp.cpp
                      priv/SnapshotCache.cpp
                      priv/proto/FileReadWriter.cpp
                      priv/proto/ExperimentReadWriter.cpp
                      priv/proto/TDDCache.cpp
                      priv/proto/IOUtils.cpp
                      utils/Checker.cpp
                      utils/Defer.cpp
                      )

set(SRC_FILES Frame.cpp
	          Experiment.cpp
	          Ant.cpp
	          Time.cpp
	          )

set(SRC_UTEST_FILES main-check.cpp
                    TestSetup.cpp
                    UtilsUTest.cpp
                    TimeUTest.cpp
                    utils/NotYetImplementedUTest.cpp
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
                    )



set(HDR_UTEST_FILES TestSetup.hpp
                    UtilsUTest.hpp
                    TimeUTest.hpp
                    utils/NotYetImplementedUTest.hpp
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
                    )

add_library(myrmidon SHARED ${SRC_FILES}
                            ${SRC_PRIVATE_FILES}
                            ${HDR_PRIVATE_FILES}
                            ${HDR_PUBLIC_FILES}
                            ${PROTO_HDR}
                            ${PROTO_SRC})

target_link_libraries(myrmidon ${PROTOBUF_LIBRARIES}
                               fort-hermes-cpp
                               ${CXXFS_LIBRARY}
                               fort-tags)


add_executable(myrmidon-tests ${SRC_UTEST_FILES}
                              ${HDR_UTEST_FILES}
                              )
target_link_libraries(myrmidon-tests myrmidon
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
