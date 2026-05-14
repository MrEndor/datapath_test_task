include_guard(GLOBAL)

include("${CMAKE_CURRENT_LIST_DIR}/CPM.cmake")

CPMAddPackage(
        NAME yaml-cpp
        GITHUB_REPOSITORY jbeder/yaml-cpp
        GIT_TAG yaml-cpp-0.9.0
        OPTIONS
        "YAML_CPP_BUILD_TESTS OFF"
        "YAML_CPP_BUILD_TOOLS OFF"
        "YAML_CPP_BUILD_CONTRIB OFF")

if (yaml-cpp_ADDED)
    set_target_properties(yaml-cpp PROPERTIES SYSTEM ON)
endif ()

CPMAddPackage(
        NAME CLI11
        GITHUB_REPOSITORY CLIUtils/CLI11
        VERSION 2.4.2)

if (EXTERNAL_SORTING_BUILD_TESTS)
    CPMAddPackage(
            NAME googletest
            GITHUB_REPOSITORY google/googletest
            VERSION 1.17.0
            OPTIONS
            "INSTALL_GTEST OFF"
            "gtest_force_shared_crt ON"
            "BUILD_GMOCK OFF")
endif ()
