add_executable(external_sorting_tests
        unit/TapeConfigParserTest.cpp
        unit/FileTapeTest.cpp
        unit/TapeSorterTest.cpp)

target_include_directories(external_sorting_tests
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(external_sorting_tests
        PRIVATE
        EXTERNAL_SORTING_FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures")

target_link_libraries(external_sorting_tests
        PRIVATE
        external_sorting_core
        GTest::gtest_main)

if (TARGET gtest)
    get_target_property(gtest_include_dirs gtest INTERFACE_INCLUDE_DIRECTORIES)
    if (gtest_include_dirs)
        target_include_directories(external_sorting_tests SYSTEM
                PRIVATE
                ${gtest_include_dirs})
    endif ()
endif ()

external_sorting_enable_sanitizers(external_sorting_tests)
external_sorting_enable_coverage(external_sorting_tests)

include(GoogleTest)
gtest_discover_tests(external_sorting_tests)
