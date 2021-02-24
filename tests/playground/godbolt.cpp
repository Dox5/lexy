// Copyright (C) 2020-2021 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#define LEXY_PLAYGROUND_PRODUCTION production
#include "../../docs/assets/cpp/godbolt_prefix.cpp"

struct production
{
    static constexpr auto rule = LEXY_LIT("hello");
};

#include "../../docs/assets/cpp/godbolt_main.cpp"

