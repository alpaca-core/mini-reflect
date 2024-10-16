// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "mini-reflect.hpp"

#if MINI_REFLECT_IMPLEMENT
#define MINI_REFLECT_SYM
#else
#define MINI_REFLECT_SYM inline
#endif

namespace reflect {
MINI_REFLECT_SYM reflection_info::~reflection_info() noexcept = default; // export vtable
} // namespace reflect
