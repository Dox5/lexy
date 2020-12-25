// Copyright (C) 2020 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <lexy/engine/success.hpp>

#include "verify.hpp"

TEST_CASE("engine_success")
{
    using engine = lexy::engine_success;
    CHECK(lexy::engine_is_matcher<engine>);

    constexpr auto empty = engine_matches<engine>("");
    CHECK(empty);
    CHECK(empty.count == 0);

    constexpr auto abc = engine_matches<engine>("abc");
    CHECK(abc);
    CHECK(abc.count == 0);
}

