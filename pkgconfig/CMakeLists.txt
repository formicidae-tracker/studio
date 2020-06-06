set(PKG_FILE fort-myrmidon.pc.in)
if (USE_CXX17_FS_LIBRARY)
	set(PKG_FS_LIBRARY "-lstdc++fs")
else (USE_CXX17_FS_LIBRARY)
	set(PKG_FS_LIBRTARY "-lboost_filesystem" )
endif(USE_CXX17_FS_LIBRARY)

if(${OpenCV_VERSION} VERSION_GREATER_EQUAL "4.0.0")
	set(OPENCV_PKG_REQUIRE opencv4)
else(${OpenCV_VERSION} VERSION_GREATER_EQUAL "4.0.0")
	set(OPENCV_PKG_REQUIRE opencv)
endif(${OpenCV_VERSION} VERSION_GREATER_EQUAL "4.0.0")

configure_file(${PKG_FILE} fort-myrmidon.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fort-myrmidon.pc
		DESTINATION lib/pkgconfig
		)
