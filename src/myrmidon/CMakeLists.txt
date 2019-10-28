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



set(PROTO_FILES Experiment.proto)

protobuf_generate_cpp(PROTO_SRC PROTO_HDR ${PROTO_FILES})

foreach(f ${PROTO_SRC})
	set_property(SOURCE ${f} PROPERTY SKIP_AUTOGEN ON)
endforeach(f ${PROTO_SRC})

foreach(f ${PROTO_HDR})
	set_property(SOURCE ${f} PROPERTY SKIP_AUTOGEN ON)
endforeach(f ${PROTO_HDR})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(HDR_PRIVATE_FILES priv/ForwardDeclaration.hpp
                      priv/Experiment.hpp
                      priv/Identifier.hpp
                      priv/Ant.hpp
                      priv/Identification.hpp
                      priv/TrackingDataDirectory.hpp
                      priv/FramePointer.hpp
                      priv/DeletedReference.hpp
                      priv/ExperimentReadWriter.hpp
                      priv/ProtobufExperimentReadWriter.hpp
					  utils/NotYetImplemented.hpp
					  priv/Isometry2D.hpp)


set(HDR_PUBLIC_FILES Frame.hpp
                     Experiment.hpp
                     Ant.hpp)

set(SRC_FILES Frame.cpp
	          Experiment.cpp
	          priv/Experiment.cpp
	          priv/Identifier.cpp
	          Ant.cpp
	          priv/Ant.cpp
	          priv/Identification.cpp
	          priv/TrackingDataDirectory.cpp
	          priv/FramePointer.cpp
	          priv/DeletedReference.cpp
	          priv/ExperimentReadWriter.cpp
	          priv/ProtobufExperimentReadWriter.cpp
	          utils/ProtobufFileReadWriter.cpp
			  priv/Isometry2D.cpp)

set(SRC_UTEST_FILES main-check.cpp
                    TestSetup.cpp
                    priv/ExperimentUTest.cpp
                    priv/TrackingDataDirectoryUTest.cpp
                    priv/AntUTest.cpp
                    priv/IdentifierUTest.cpp
                    priv/IdentificationUTest.cpp
                    priv/FramePointerUTest.cpp
                    priv/ProtobufExperimentReadWriterUTest.cpp
                    priv/Isometry2DUTest.cpp)

set(HDR_UTEST_FILES TestSetup.hpp
                    priv/ExperimentUTest.hpp
                    priv/TrackingDataDirectoryUTest.hpp
                    priv/AntUTest.hpp
                    priv/Identifier.hpp
                    priv/Identification.hpp
                    priv/FramePointerUTest.hpp
                    priv/ProtobufExperimentReadWriterUTest.hpp
                    priv/Isometry2DUTest.hpp)

add_library(myrmidon SHARED ${SRC_FILES}
                            ${HDR_PUBLIC_FILES}
                            ${PROTO_HDR}
                            ${PROTO_SRC})

target_link_libraries(myrmidon ${PROTOBUF_LIBRARIES}
                               ${FORT_HERMES_CPP_LIBRARIES}
                               ${CXXFS_LIBRARY})


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
