add_library(libbget INTERFACE)

target_sources(libbget
    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/bget.c
)

target_include_directories(libbget
    INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
)
