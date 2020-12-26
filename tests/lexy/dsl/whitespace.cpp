// Copyright (C) 2020 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <lexy/dsl/whitespace.hpp>

#include "verify.hpp"
#include <lexy/dsl/if.hpp>
#include <lexy/dsl/label.hpp>

TEST_CASE("dsl::whitespaced()")
{
    SUBCASE("simple")
    {
        constexpr auto rule = whitespaced(LEXY_LIT("abc"), LEXY_LIT(" "));
        CHECK(lexy::is_rule<decltype(rule)>);
        CHECK(lexy::is_branch<decltype(rule)>);

        struct callback
        {
            const char* str;

            constexpr int success(const char* cur)
            {
                return int(cur - str) - 3;
            }

            constexpr int error(test_error<lexy::expected_literal> e)
            {
                CONSTEXPR_CHECK(e.string() == "abc");
                return -1;
            }
        };

        constexpr auto empty = rule_matches<callback>(rule, "");
        CHECK(empty == -1);

        constexpr auto abc = rule_matches<callback>(rule, "abc");
        CHECK(abc == 0);

        constexpr auto space_abc = rule_matches<callback>(rule, " abc");
        CHECK(space_abc == 1);
        constexpr auto space_space_abc = rule_matches<callback>(rule, "  abc");
        CHECK(space_space_abc == 2);
    }
    SUBCASE("branch")
    {
        constexpr auto rule_ = whitespaced(LEXY_LIT("abc") >> lexy::dsl::id<0>, LEXY_LIT(" "));
        CHECK(lexy::is_rule<decltype(rule_)>);
        CHECK(lexy::is_branch<decltype(rule_)>);

        constexpr auto rule = if_(rule_);

        struct callback
        {
            const char* str;

            constexpr int success(const char* cur)
            {
                CONSTEXPR_CHECK(cur == str);
                return 0;
            }
            constexpr int success(const char* cur, lexy::id<0>)
            {
                return int(cur - str);
            }
        };

        constexpr auto empty = rule_matches<callback>(rule, "");
        CHECK(empty == 0);

        constexpr auto abc = rule_matches<callback>(rule, "abc");
        CHECK(abc == 3);

        constexpr auto space_abc = rule_matches<callback>(rule, " abc");
        CHECK(space_abc == 4);
        constexpr auto space_space_abc = rule_matches<callback>(rule, "  abc");
        CHECK(space_space_abc == 5);
    }
}

