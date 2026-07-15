# PoseBust — standalone C++26 pose validation (NativePoseQC + upstream bust bridge)
# Copyright 2026 Le Bonhomme Pharma
# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required(VERSION 3.28)
project(PoseBust
    VERSION 0.1.0
    DESCRIPTION "Standalone C++26 PoseBusters-compatible pose validation"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(POSEBUST_BUILD_CLI "Build posebust CLI" ON)
option(POSEBUST_BUILD_TESTS "Build unit tests" ON)
option(POSEBUST_INSTALL "Generate install targets" ON)

# ─── library ────────────────────────────────────────────────────────────────
add_library(posebust
    src/Loaders.cpp
    src/ChecksChemistry.cpp
    src/ChecksGeometry.cpp
    src/ChecksProtein.cpp
    src/Engine.cpp
    src/BustCli.cpp
)
add_library(PoseBust::posebust ALIAS posebust)

target_include_directories(posebust
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if(MSVC)
    target_compile_options(posebust PRIVATE /W4 /permissive- /O2)
    target_compile_definitions(posebust PRIVATE
        _CRT_SECURE_NO_WARNINGS _USE_MATH_DEFINES NOMINMAX)
else()
    target_compile_options(posebust PRIVATE -Wall -Wextra -Wpedantic -O3)
endif()

# ─── CLI ────────────────────────────────────────────────────────────────────
if(POSEBUST_BUILD_CLI)
    add_executable(posebust_cli apps/posebust_main.cpp)
    set_target_properties(posebust_cli PROPERTIES OUTPUT_NAME posebust)
    target_link_libraries(posebust_cli PRIVATE posebust)
    if(MSVC)
        target_compile_options(posebust_cli PRIVATE /W4 /O2)
    else()
        target_compile_options(posebust_cli PRIVATE -Wall -Wextra -O3)
    endif()
endif()

# ─── tests ──────────────────────────────────────────────────────────────────
if(POSEBUST_BUILD_TESTS)
    include(CTest)
    enable_testing()
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
        FIND_PACKAGE_ARGS NAMES GTest
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(posebust_tests
        tests/test_posebust_core.cpp
        tests/test_posebust_loaders.cpp
    )
    target_link_libraries(posebust_tests PRIVATE posebust GTest::gtest_main)
    target_include_directories(posebust_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    include(GoogleTest)
    gtest_discover_tests(posebust_tests)
endif()

# ─── install ────────────────────────────────────────────────────────────────
if(POSEBUST_INSTALL)
    include(GNUInstallDirs)
    install(TARGETS posebust
        EXPORT PoseBustTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    if(POSEBUST_BUILD_CLI)
        install(TARGETS posebust_cli RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
    endif()
    install(DIRECTORY include/posebust DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    install(EXPORT PoseBustTargets
        FILE PoseBustTargets.cmake
        NAMESPACE PoseBust::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PoseBust
    )
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/PoseBustConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/PoseBustConfig.cmake"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/PoseBustTargets.cmake\")\n")
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/PoseBustConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/PoseBustConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PoseBust
    )
endif()

message(STATUS "PoseBust ${PROJECT_VERSION}: CLI=${POSEBUST_BUILD_CLI} tests=${POSEBUST_BUILD_TESTS}")
