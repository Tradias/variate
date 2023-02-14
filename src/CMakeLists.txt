# Copyright (c) 2022 Dennis Hezel
#
# This software is released under the MIT License.
# https://opensource.org/licenses/MIT

# variate
add_library(variate INTERFACE)

add_library(${PROJECT_NAME}::variate ALIAS variate)

target_compile_features(variate INTERFACE cxx_std_20)

target_include_directories(variate INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                                             $<INSTALL_INTERFACE:include>)

# variate sources
if(VARIATE_BUILD_TESTS)
    add_library(variate-sources OBJECT)

    target_sources(variate-sources PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/variate.cpp")

    target_link_libraries(variate-sources PUBLIC variate variate-compile-options variate-coverage-options)

    set_target_properties(variate-sources PROPERTIES COMPILE_WARNING_AS_ERROR on)
endif()
