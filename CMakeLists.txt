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
add_subdirectory(libFLAC_bgets)
add_subdirectory(bget)

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
    -mcmodel=large
    -Wno-xcore-fptrgroup
    ${CMAKE_CURRENT_LIST_DIR}/src/XCORE-AI-EXPLORER.xn
)

set(APP_COMPILE_DEFINITIONS
    FLAC__INTEGER_ONLY_LIBRARY=1
)

set(APP_LINK_OPTIONS
    -report
    ${CMAKE_CURRENT_LIST_DIR}/src/XCORE-AI-EXPLORER.xn
)

set(APP_COMMON_LINK_LIBRARIES
    libbget
)

set(CH_CNTS 2 4 6 8)

foreach(CH_CNT ${CH_CNTS})
    add_executable(flak_eval_${CH_CNT} EXCLUDE_FROM_ALL)
    target_sources(flak_eval_${CH_CNT} PUBLIC ${APP_SOURCES})
    target_include_directories(flak_eval_${CH_CNT} PUBLIC ${APP_INCLUDES})
    target_compile_definitions(flak_eval_${CH_CNT} PUBLIC CHAN_CNT=${CH_CNT} ${APP_COMPILE_DEFINITIONS} THIS_XCORE_TILE=0)
    target_compile_options(flak_eval_${CH_CNT} PRIVATE ${APP_COMPILER_FLAGS})
    target_link_libraries(flak_eval_${CH_CNT} PUBLIC libFLAC ${APP_COMMON_LINK_LIBRARIES})
    target_link_options(flak_eval_${CH_CNT} PRIVATE ${APP_LINK_OPTIONS})

    add_executable(flak_eval_ddr_${CH_CNT} EXCLUDE_FROM_ALL)
    target_sources(flak_eval_ddr_${CH_CNT} PUBLIC ${APP_SOURCES})
    target_include_directories(flak_eval_ddr_${CH_CNT} PUBLIC ${APP_INCLUDES})
    target_compile_definitions(flak_eval_ddr_${CH_CNT} PUBLIC CHAN_CNT=${CH_CNT} USE_BGETS=1 ${APP_COMPILE_DEFINITIONS} THIS_XCORE_TILE=0)
    target_compile_options(flak_eval_ddr_${CH_CNT} PRIVATE ${APP_COMPILER_FLAGS})
    target_link_libraries(flak_eval_ddr_${CH_CNT} PUBLIC libFLAC_bgets ${APP_COMMON_LINK_LIBRARIES})
    target_link_options(flak_eval_ddr_${CH_CNT} PRIVATE ${APP_LINK_OPTIONS})

    add_custom_target(run_flak_eval_${CH_CNT}
        COMMAND xrun --io flak_eval_${CH_CNT}.xe
        DEPENDS flak_eval_${CH_CNT}
        COMMENT
        "Run application"
        VERBATIM
    )
    add_custom_target(run_flak_eval_ddr_${CH_CNT}
        COMMAND xrun --io flak_eval_ddr_${CH_CNT}.xe
        DEPENDS flak_eval_ddr_${CH_CNT}
        COMMENT
        "Run application"
        VERBATIM
    )
endforeach()
