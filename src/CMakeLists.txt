add_subdirectory(fort/myrmidon)

option(BUILD_STUDIO On "Build FORT Studio")
option(BUILD_EXTRA_TOOLS Off "Build FORT Studio")

if(BUILD_STUDIO)

add_subdirectory(fort/studio)

endif(BUILD_STUDIO)

if(BUILD_EXTRA_TOOLS)

add_subdirectory(capsule-collision-tester)

add_subdirectory(analyze-video)

endif(BUILD_EXTRA_TOOLS)

set(MYRMIDON_API_DOC_FILES ${MYRMIDON_API_DOC_FILES} PARENT_SCOPE)
set(MYRMIDON_PRIVATE_DOC_FILES ${MYRMIDON_PRIVATE_DOC_FILES} PARENT_SCOPE)
