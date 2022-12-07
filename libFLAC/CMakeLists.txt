file(GLOB_RECURSE LIB_FLAC_SOURCES  ${CMAKE_CURRENT_LIST_DIR}/library/*.c)

add_library(libFLAC INTERFACE)

target_sources(libFLAC
    INTERFACE
        ${LIB_FLAC_SOURCES}
)

target_include_directories(libFLAC
    INTERFACE
        include
        include/private
)
