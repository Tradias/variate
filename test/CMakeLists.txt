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

if(Boost_FOUND)
    target_sources(variate-test PRIVATE "boost_tests.cpp")

    target_link_libraries(variate-test PRIVATE Boost::headers)

    set_source_files_properties("boost_tests.cpp" PROPERTIES SKIP_UNITY_BUILD_INCLUSION on)
else()
    target_sources(variate-test PRIVATE "no_boost_tests.cpp")
endif()

add_test(NAME variate-test COMMAND variate-test)
