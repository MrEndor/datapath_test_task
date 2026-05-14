cmake_minimum_required(VERSION 3.20)

project(external_sorting LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Emit compile_commands.json" FORCE)

include(cmake/ProjectOptions.cmake)
include(cmake/CompilerWarnings.cmake)
include(cmake/Sanitizers.cmake)
include(cmake/Coverage.cmake)
include(cmake/Dependencies.cmake)

add_subdirectory(src)

if (EXTERNAL_SORTING_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()
