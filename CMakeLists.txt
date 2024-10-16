# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(mini-reflect
    LANGUAGES CXX
)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(miniReflectIsRoot YES)
    include(./root-setup.cmake)
endif()

#######################################
# options

option(MINI_REFLECT_HEADER_ONLY "${CMAKE_PROJECT_NAME}: only expose header-only lib" OFF)
option(MINI_REFLECT_BUILD_TESTS "${CMAKE_PROJECT_NAME}: enable testing and build tests" ${miniReflectIsRoot})
option(MINI_REFLECT_BUILD_EXAMPLES "${CMAKE_PROJECT_NAME}: build examples" ${miniReflectIsRoot})
mark_as_advanced(MINI_REFLECT_BUILD_TESTS MINI_REFLECT_BUILD_EXAMPLES)

#######################################
# code
add_subdirectory(code)

if(MINI_REFLECT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

if(MINI_REFLECT_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()
