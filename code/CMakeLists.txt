# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
set(tgt mini-reflect-headers)
add_library(${tgt} INTERFACE)
target_include_directories(${tgt} INTERFACE .)
target_compile_features(${tgt} INTERFACE cxx_std_11)
add_library(mini-reflect::headers ALIAS ${tgt})

if(NOT MINI_REFLECT_HEADER_ONLY)
    set(tgt mini-reflect)
    add_library(${tgt}
        mini-reflect.cpp
    )
    add_library(mini-reflect::mini-reflect ALIAS ${tgt})
    target_link_libraries(${tgt} PUBLIC mini-reflect::headers)

    if(BUILD_SHARED_LIBS)
        target_compile_definitions(${tgt} PUBLIC MINI_REFLECT_SHARED)
        if(NOT WIN32)
            target_compile_options(${tgt} PRIVATE
                -fvisibility=hidden
                -fvisibility-inlines-hidden
            )
        endif()
    endif()
endif()
