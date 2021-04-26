option(BUILD_XDESKTOP_ENTRY "Build the freedesktop menu entry" OFF)

if(BUILD_XDESKTOP_ENTRY)
	find_program(IMAGEMAGICK_CONVERT convert DOC "ImageMagick convert utility form icon generation")
	if (NOT IMAGEMAGICK_CONVERT)
		message(FATAL_ERROR "Missing required imagemagick convert command")
	endif (NOT IMAGEMAGICK_CONVERT)
	configure_file(${PROJECT_SOURCE_DIR}/misc/fort-studio.desktop.in
	               ${PROJECT_BINARY_DIR}/misc/fort-studio.desktop
	               @ONLY)
    add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/misc/fort-studio.svg
	                   COMMAND ${CMAKE_COMMAND} ARGS -E
	                                                  copy
	                                                  ${PROJECT_SOURCE_DIR}/resources/icons/fort-studio.svg
	                                                  ${PROJECT_BINARY_DIR}/misc/fort-studio.svg
		               DEPENDS ${PROJECT_SOURCE_DIR}/resources/icons/fort-studio.svg
		               COMMENT "Generating FORT Studio scalable icon")

    add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/misc/fort-studio.png
	                   COMMAND ${IMAGEMAGICK_CONVERT} ARGS ${PROJECT_BINARY_DIR}/misc/fort-studio.svg
	                                                       -resize 48x48
	                                                       ${PROJECT_BINARY_DIR}/misc/fort-studio.png
	                   DEPENDS ${PROJECT_BINARY_DIR}/misc/fort-studio.svg
	                   COMMENT "Generating FORT Studio 48x48 icon")
    add_custom_target(generate-freedesktop-entry ALL
	                  DEPENDS ${PROJECT_BINARY_DIR}/misc/fort-studio.png
	                          ${PROJECT_BINARY_DIR}/misc/fort-studio.svg)
    install(FILES ${PROJECT_BINARY_DIR}/misc/fort-studio.svg
	        DESTINATION share/icons/hicolor/scalable/apps/)
    install(FILES ${PROJECT_BINARY_DIR}/misc/fort-studio.png
	        DESTINATION share/icons/hicolor/48x48/apps/)
    install(FILES ${PROJECT_BINARY_DIR}/misc/fort-studio.desktop
	        DESTINATION share/applications)

endif(BUILD_XDESKTOP_ENTRY)
