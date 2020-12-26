// Copyright (C) 2020 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <lexy/dsl/times.hpp>

#include "verify.hpp"
#include <lexy/dsl/value.hpp>

TEST_CASE("times")
{
    SUBCASE("no sep")
    {
        constexpr auto rule = twice(LEXY_LIT("abc") + lexy::dsl::value_c<1>);
        CHECK(lexy::is_rule<decltype(rule)>);

        struct callback
        {
            const char* str;

            constexpr int success(const char* cur, lexy::twice<int> value)
            {
                CONSTEXPR_CHECK(cur - str == 6);
                CONSTEXPR_CHECK(value[0] == 1);
                CONSTEXPR_CHECK(value[1] == 1);
                return 0;
            }

            constexpr int error(test_error<lexy::expected_literal> e)
            {
                CONSTEXPR_CHECK(e.string() == "abc");
                return -1;
            }
        };

        constexpr auto zero = verify<callback>(rule, "");
        CHECK(zero == -1);
        constexpr auto one = verify<callback>(rule, "abc");
        CHECK(one == -1);

        constexpr auto two = verify<callback>(rule, "abcabc");
        CHECK(two == 0);
    }
    SUBCASE("sep")
    {
        constexpr auto rule = twice(LEXY_LIT("abc") + lexy::dsl::value_c<1>, sep(LEXY_LIT(",")));
        CHECK(lexy::is_rule<decltype(rule)>);

        struct callback
        {
            const char* str;

            constexpr int success(const char* cur, lexy::twice<int> value)
            {
                CONSTEXPR_CHECK(cur - str == 7);
                CONSTEXPR_CHECK(value[0] == 1);
                CONSTEXPR_CHECK(value[1] == 1);
                return 0;
            }

            constexpr int error(test_error<lexy::expected_literal> e)
            {
                if (e.string() == "abc")
                    return -1;
                else if (e.string() == ",")
                    return -2;
                else
                    CONSTEXPR_CHECK(false);
            }
        };

        constexpr auto zero = verify<callback>(rule, "");
        CHECK(zero == -1);
        constexpr auto one = verify<callback>(rule, "abc");
        CHECK(one == -2);

        constexpr auto two = verify<callback>(rule, "abc,abc");
        CHECK(two == 0);

        constexpr auto no_sep = verify<callback>(rule, "abcabc");
        CHECK(no_sep == -2);

        constexpr auto trailing_sep = verify<callback>(rule, "abc,abc,");
        CHECK(trailing_sep == 0);
    }
    SUBCASE("trailing_sep")
    {
        constexpr auto rule
            = twice(LEXY_LIT("abc") + lexy::dsl::value_c<1>, trailing_sep(LEXY_LIT(",")));
        CHECK(lexy::is_rule<decltype(rule)>);

        struct callback
        {
            const char* str;

            constexpr int success(const char* cur, lexy::twice<int> value)
            {
                CONSTEXPR_CHECK(value[0] == 1);
                CONSTEXPR_CHECK(value[1] == 1);

                if (cur - str == 7)
                    return 0;
                else if (cur - str == 8)
                    return 1; // trailing sep
                else
                    CONSTEXPR_CHECK(false);
            }

            constexpr int error(test_error<lexy::expected_literal> e)
            {
                if (e.string() == "abc")
                    return -1;
                else if (e.string() == ",")
                    return -2;
                else
                    CONSTEXPR_CHECK(false);
            }
        };

        constexpr auto zero = verify<callback>(rule, "");
        CHECK(zero == -1);
        constexpr auto one = verify<callback>(rule, "abc");
        CHECK(one == -2);

        constexpr auto two = verify<callback>(rule, "abc,abc");
        CHECK(two == 0);

        constexpr auto no_sep = verify<callback>(rule, "abcabc");
        CHECK(no_sep == -2);

        constexpr auto trailing_sep = verify<callback>(rule, "abc,abc,");
        CHECK(trailing_sep == 1);
    }
}

