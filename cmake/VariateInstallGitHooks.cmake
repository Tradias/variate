# Copyright (c) 2022 Dennis Hezel
# 
# This software is released under the MIT License.
# https://opensource.org/licenses/MIT

find_package(Git)

function(VARIATE_create_init_git_hooks_target)
    if(TARGET variate-init-git-hooks)
        return()
    endif()

    set(VARIATE_GIT_HOOKS_TARGET_DIR "${CMAKE_SOURCE_DIR}/.git/hooks")
    set(VARIATE_GIT_HOOKS_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/git-hooks/")

    if(NOT EXISTS "${VARIATE_GIT_HOOKS_TARGET_DIR}/pre-commit"
       OR NOT EXISTS "${VARIATE_GIT_HOOKS_TARGET_DIR}/VariatePreCommit.cmake")
        message(
            AUTHOR_WARNING
                "Initialize clang-format and cmake-format pre-commit hooks by building the CMake target variate-init-git-hooks."
        )
    endif()

    find_program(VARIATE_CMAKE_FORMAT_PROGRAM cmake-format)
    find_program(VARIATE_CLANG_FORMAT_PROGRAM clang-format)

    if(NOT VARIATE_CMAKE_FORMAT_PROGRAM OR NOT VARIATE_CLANG_FORMAT_PROGRAM)
        message(
            AUTHOR_WARNING
                "Cannot create init-git-hooks target with\ncmake-format: ${VARIATE_CMAKE_FORMAT_PROGRAM}\nclang-format: ${VARIATE_CLANG_FORMAT_PROGRAM}"
        )
        return()
    endif()

    set(VARIATE_INIT_GIT_HOOKS_SOURCES "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/hooks/pre-commit.in"
                                            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/hooks/VariatePreCommit.cmake.in")
    configure_file("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/hooks/pre-commit.in"
                   "${VARIATE_GIT_HOOKS_SOURCE_DIR}/pre-commit" @ONLY NEWLINE_STYLE UNIX)
    configure_file("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/hooks/VariatePreCommit.cmake.in"
                   "${VARIATE_GIT_HOOKS_SOURCE_DIR}/VariatePreCommit.cmake" @ONLY NEWLINE_STYLE UNIX)

    set(_VARIATE_command_arguments
        "-DGIT_HOOKS_TARGET_DIR=${VARIATE_GIT_HOOKS_TARGET_DIR}"
        "-DGIT_HOOKS_SOURCE_DIR=${VARIATE_GIT_HOOKS_SOURCE_DIR}" -P
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/VariateGitHooksInstaller.cmake")
    string(REPLACE ";" " " _VARIATE_pretty_command_arguments "${_VARIATE_command_arguments}")
    add_custom_target(
        variate-init-git-hooks
        DEPENDS ${VARIATE_INIT_GIT_HOOKS_SOURCES}
        SOURCES ${VARIATE_INIT_GIT_HOOKS_SOURCES}
        COMMAND ${CMAKE_COMMAND} ${_VARIATE_command_arguments}
        COMMENT "cmake ${_VARIATE_pretty_command_arguments}"
        VERBATIM)
endfunction()

if(GIT_FOUND)
    VARIATE_create_init_git_hooks_target()
endif()
