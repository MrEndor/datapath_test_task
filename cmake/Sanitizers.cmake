include_guard(GLOBAL)

function(external_sorting_enable_sanitizers target)
    if (NOT EXTERNAL_SORTING_ENABLE_SANITIZERS)
        return()
    endif ()

    if (NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(WARNING "Sanitizers requested but compiler is not GCC/Clang; skipping")
        return()
    endif ()

    set(sanitizer_flags
            -fsanitize=address,undefined
            -fno-omit-frame-pointer
            -fno-sanitize-recover=all)

    target_compile_options(${target} PRIVATE ${sanitizer_flags})
    target_link_options(${target} PRIVATE ${sanitizer_flags})
endfunction()
