set(SRC_FILES Metadata.cpp
	          Ant.cpp)

set(HDR_FILES Metadata.hpp
	          Ant.hpp)

add_library(myrmidon SHARED ${SRC_FILES} ${HDR_FILES})
