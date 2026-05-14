include_guard(GLOBAL)

function(external_sorting_enable_coverage target)
    if (NOT EXTERNAL_SORTING_ENABLE_COVERAGE)
        return()
    endif ()

    if (NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(WARNING "Coverage requested but compiler is not GCC/Clang; skipping")
        return()
    endif ()

    target_compile_options(${target} PRIVATE --coverage -O0 -g)
    target_link_options(${target} PRIVATE --coverage)
endfunction()

if (EXTERNAL_SORTING_ENABLE_COVERAGE)
    find_program(GCOVR_EXECUTABLE gcovr)
    if (GCOVR_EXECUTABLE)
        set(coverage_output_dir "${CMAKE_BINARY_DIR}/coverage")
        file(MAKE_DIRECTORY "${coverage_output_dir}")

        add_custom_target(coverage_report
                COMMAND ${CMAKE_COMMAND} -E make_directory "${coverage_output_dir}"
                COMMAND ${GCOVR_EXECUTABLE}
                --root "${CMAKE_SOURCE_DIR}"
                --filter "${CMAKE_SOURCE_DIR}/src/"
                --exclude "${CMAKE_SOURCE_DIR}/src/main.cpp"
                --html-details "${coverage_output_dir}/index.html"
                --xml "${coverage_output_dir}/coverage.xml"
                --print-summary
                --fail-under-line 80
                WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
                COMMENT "Generating coverage report (gcovr)")
    else ()
        message(WARNING "gcovr not found; coverage_report target unavailable")
    endif ()
endif ()
