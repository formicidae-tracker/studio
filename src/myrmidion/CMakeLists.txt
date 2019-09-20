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
MYRMIDION_USE_SYSTEM_CATEGORY)


configure_file(config.h.in myrmidion-config.h)



set(PROTO_FILES Experiment.proto)

protobuf_generate_cpp(PROTO_SRC PROTO_HDR ${PROTO_FILES})

foreach(f ${PROTO_SRC})
	set_property(SOURCE ${f} PROPERTY SKIP_AUTOGEN ON)
endforeach(f ${PROTO_SRC})

foreach(f ${PROTO_HDR})
	set_property(SOURCE ${f} PROPERTY SKIP_AUTOGEN ON)
endforeach(f ${PROTO_HDR})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(HDR_PRIVATE_FILES Experiment.priv.hpp
                      Ant.priv.hpp)

set(HDR_PUBLIC_FILES Frame.hpp
                     Experiment.hpp
	                 Ant.hpp)


set(SRC_FILES Frame.cpp
	          Experiment.cpp
	          Experiment.priv.cpp
	          Ant.cpp
	          Ant.priv.cpp)


add_library(myrmidion SHARED ${SRC_FILES} ${HDR_PUBLIC_FILES} ${PROTO_HDR} ${PROTO_SRC})

target_link_libraries(myrmidion ${PROTOBUF_LIBRARIES})
