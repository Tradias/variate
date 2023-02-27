# Copyright (c) 2022 Dennis Hezel
#
# This software is released under the MIT License.
# https://opensource.org/licenses/MIT

cmake_minimum_required(VERSION 3.14...3.24)

project(
    variate
    VERSION 0.1.0
    DESCRIPTION "Deduce the template parameter of a variant based on every return path"
    LANGUAGES CXX)

include(GNUInstallDirs)

# user options
option(VARIATE_INSTALL "Create the install target" on)
set(VARIATE_CMAKE_CONFIG_INSTALL_DIR
    "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    CACHE STRING "Install directory for CMake config files")

# maintainer options
option(VARIATE_BUILD_TESTS "Build tests and examples" off)
option(VARIATE_TEST_COVERAGE "Compile tests with --coverage" off)
set(VARIATE_COVERAGE_OUTPUT_FILE
    "${CMAKE_CURRENT_BINARY_DIR}/sonarqube-coverage.xml"
    CACHE FILEPATH "Output file for SonarQube XML coverage files")
option(VARIATE_ENABLE_DYNAMIC_ANALYSIS "Compile tests with -fsanitize=undefined,address or /fsanitize=address" off)

set(VARIATE_PROJECT_ROOT "${CMAKE_CURRENT_LIST_DIR}")

if(VARIATE_BUILD_TESTS)
    message(STATUS "CMake version: ${CMAKE_VERSION}")
    message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

    include("${CMAKE_CURRENT_LIST_DIR}/cmake/VariateCompileOptions.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/cmake/VariateInstallGitHooks.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/cmake/VariateCoverage.cmake")
endif()

add_subdirectory(src)

if(VARIATE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

if(VARIATE_INSTALL)
    include("${CMAKE_CURRENT_LIST_DIR}/cmake/VariateInstallation.cmake")
endif()
