# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
include(./get_cpm.cmake)

CPMAddPackage(gh:iboB/doctest-lib@2.4.11)

set(tgt test-mini-reflect)
add_executable(${tgt} test.cpp)
target_link_libraries(${tgt} PRIVATE
    doctest::main
    mini-reflect::mini-reflect
)
set_target_properties(${tgt} PROPERTIES FOLDER "test")

add_test(NAME mini-reflect COMMAND ${tgt})

