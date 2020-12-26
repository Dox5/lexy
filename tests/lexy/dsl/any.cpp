// Copyright (C) 2020 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <lexy/dsl/any.hpp>

#include "verify.hpp"
#include <lexy/input/buffer.hpp>
#include <lexy/match.hpp>

TEST_CASE("dsl::any")
{
    constexpr auto atom = lexy::dsl::any;

    constexpr auto empty = atom_matches(atom, "");
    CHECK(empty);
    CHECK(empty.count == 0);

    constexpr auto non_empty = atom_matches(atom, "abc");
    CHECK(non_empty);
    CHECK(non_empty.count == 3);
}

