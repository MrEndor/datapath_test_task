include_guard(GLOBAL)

add_library(external_sorting_compiler_warnings INTERFACE)

set(external_sorting_gcc_clang_warnings
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
        -Werror)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(external_sorting_compiler_warnings
            INTERFACE
            ${external_sorting_gcc_clang_warnings})
endif ()
