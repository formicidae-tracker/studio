set(SRC_FILES tag36ARTag.c
 	          tag36h10.c)


set(HDR_FILES tag36ARTag.h
              tag36h10.h)


add_library(legacy-tags STATIC ${HDR_FILES}
                               ${SRC_FILES})

target_link_libraries(legacy-tags apriltag)
