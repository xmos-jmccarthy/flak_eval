cmake_minimum_required(VERSION 3.21)

## Disable in-source build.
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source build is not allowed! Please specify a build folder.\n\tex:cmake -B build")
endif()

## Project declaration
project(flak_eval)

## Enable languages for project
enable_language(CXX C ASM)

## Add frameworks
add_subdirectory(libFLAC)

#**********************
# Gather Sources
#**********************
file(GLOB_RECURSE APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.c )
set(APP_INCLUDES
    ${CMAKE_CURRENT_LIST_DIR}/src
)

#**********************
# Flags
#**********************
set(APP_COMPILER_FLAGS
    -Os
    -g
    -report
    -fxscope
    -mcmodel=large
    -Wno-xcore-fptrgroup
    ${CMAKE_CURRENT_LIST_DIR}/src/XCORE-AI-EXPLORER.xn
)

set(APP_COMPILE_DEFINITIONS
    DEBUG_PRINT_ENABLE=1
)

set(APP_LINK_OPTIONS
    -report
    ${CMAKE_CURRENT_LIST_DIR}/src/XCORE-AI-EXPLORER.xn
)

set(APP_COMMON_LINK_LIBRARIES
    libFLAC
)

add_executable(flak_eval EXCLUDE_FROM_ALL)
target_sources(flak_eval PUBLIC ${APP_SOURCES})
target_include_directories(flak_eval PUBLIC ${APP_INCLUDES})
target_compile_definitions(flak_eval PUBLIC ${APP_COMPILE_DEFINITIONS} THIS_XCORE_TILE=0)
target_compile_options(flak_eval PRIVATE ${APP_COMPILER_FLAGS})
target_link_libraries(flak_eval PUBLIC ${APP_COMMON_LINK_LIBRARIES})
target_link_options(flak_eval PRIVATE ${APP_LINK_OPTIONS})
