cmake_minimum_required(VERSION 3.20)

add_library(external_sorting_core STATIC
        config/TapeConfigParser.cpp
        tape/FileTape.cpp
        sorting/TapeSorter.cpp
)

target_include_directories(external_sorting_core
        PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(external_sorting_core
        PUBLIC
        external_sorting_project_options
        external_sorting_compiler_warnings
        yaml-cpp::yaml-cpp
)

external_sorting_enable_sanitizers(external_sorting_core)
external_sorting_enable_coverage(external_sorting_core)

add_executable(external_sorting main.cpp)

target_link_libraries(external_sorting
        PRIVATE
        external_sorting_core
        CLI11::CLI11
)

external_sorting_enable_sanitizers(external_sorting)
external_sorting_enable_coverage(external_sorting)
