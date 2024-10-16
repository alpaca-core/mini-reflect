# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
set(tgt mini-reflect-example-serialize)
add_executable(${tgt} serialize.cpp)
target_link_libraries(${tgt} PRIVATE mini-reflect::mini-reflect)
set_target_properties(${tgt} PROPERTIES FOLDER example)
