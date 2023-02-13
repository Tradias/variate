# Copyright (c) 2022 Dennis Hezel
# 
# This software is released under the MIT License.
# https://opensource.org/licenses/MIT

add_library(variate-coverage-options INTERFACE)

if(VARIATE_TEST_COVERAGE)
    target_compile_options(variate-coverage-options INTERFACE --coverage
                                                                   $<$<CXX_COMPILER_ID:GNU>:-fprofile-abs-path>)

    target_link_options(variate-coverage-options INTERFACE --coverage)

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        find_program(VARIATE_GCOV_PROGRAM gcov)
    endif()
    if(NOT VARIATE_GCOV_PROGRAM)
        find_program(VARIATE_LLVM_COV_PROGRAM NAMES llvm-cov llvm-cov-10 llvm-cov-11 llvm-cov-12 llvm-cov-13
                                                         llvm-cov-14)
        set(_VARIATE_GCOV_COMMAND "${VARIATE_LLVM_COV_PROGRAM} gcov")
    else()
        set(_VARIATE_GCOV_COMMAND "${VARIATE_GCOV_PROGRAM}")
    endif()
    find_program(VARIATE_GCOVR_PROGRAM gcovr REQUIRED)
    add_custom_target(
        variate-test-coverage
        COMMAND "${VARIATE_GCOVR_PROGRAM}" --gcov-executable "${_VARIATE_GCOV_COMMAND}" --sonarqube --output
                "${VARIATE_COVERAGE_OUTPUT_FILE}" --root "${VARIATE_PROJECT_ROOT}"
        WORKING_DIRECTORY "${VARIATE_PROJECT_ROOT}"
        VERBATIM)
endif()
