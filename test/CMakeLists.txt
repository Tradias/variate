# Copyright (c) 2022 Dennis Hezel
#
# This software is released under the MIT License.
# https://opensource.org/licenses/MIT

# tests
add_executable(variate-test)

target_sources(variate-test PRIVATE "main.cpp" "test/framework.cpp")

target_include_directories(variate-test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(variate-test PRIVATE variate-sources)

target_precompile_headers(variate-test PRIVATE "test/precompiled_header.hpp")

add_test(NAME variate-test COMMAND variate-test)
