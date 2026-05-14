include_guard(GLOBAL)

add_library(external_sorting_project_options INTERFACE)

target_compile_features(external_sorting_project_options
        INTERFACE
        cxx_std_23)

set_target_properties(external_sorting_project_options
        PROPERTIES
        INTERFACE_CXX_EXTENSIONS OFF)

option(EXTERNAL_SORTING_BUILD_TESTS       "Build unit and e2e tests"        OFF)
option(EXTERNAL_SORTING_ENABLE_COVERAGE   "Instrument for line coverage"    OFF)
option(EXTERNAL_SORTING_ENABLE_SANITIZERS "Enable ASAN + UBSAN on targets"  OFF)
