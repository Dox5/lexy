// Copyright (C) 2020 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <lexy/parse.hpp>

#include <doctest/doctest.h>
#include <lexy/dsl/ascii.hpp>
#include <lexy/dsl/brackets.hpp>
#include <lexy/dsl/capture.hpp>
#include <lexy/dsl/list.hpp>
#include <lexy/dsl/option.hpp>
#include <lexy/dsl/production.hpp>
#include <lexy/dsl/punctuator.hpp>
#include <lexy/dsl/sequence.hpp>
#include <lexy/dsl/while.hpp>
#include <lexy/input/string_input.hpp>
#include <vector>

namespace parse_value
{
//=== AST ===//
struct string_pair
{
    lexy::_detail::string_view a;
    lexy::_detail::string_view b;
};

//=== grammar ===//
namespace dsl = lexy::dsl;

struct string_p
{
    static constexpr auto rule = capture(dsl::ascii::alnum + while_(dsl::ascii::alnum));

    static constexpr auto value = lexy::as_string<lexy::_detail::string_view>;
};

struct string_pair_p
{
    static constexpr auto rule
        = dsl::parenthesized(dsl::p<string_p> + dsl::comma + dsl::p<string_p>);

    static constexpr auto value = lexy::construct<string_pair>;
};

using prod = string_pair_p;
} // namespace parse_value

namespace parse_list
{
namespace dsl = lexy::dsl;

using parse_value::string_p;

struct string_list_p
{
    static constexpr auto rule = dsl::parenthesized.opt_list(dsl::p<string_p>, sep(dsl::comma));

    static constexpr auto list = lexy::as_list<std::vector<lexy::_detail::string_view>>;
};

using prod = string_list_p;
} // namespace parse_list

namespace parse_list_value
{
namespace dsl = lexy::dsl;

using parse_value::string_p;

struct string_list_p
{
    static constexpr auto rule = dsl::parenthesized.opt_list(dsl::p<string_p>, sep(dsl::comma));

    static constexpr auto list  = lexy::as_list<std::vector<lexy::_detail::string_view>>;
    static constexpr auto value = lexy::callback<std::size_t>(
        [](const std::vector<lexy::_detail::string_view>& vec) { return vec.size(); });
};

using prod = string_list_p;
} // namespace parse_list_value

TEST_CASE("parse")
{
    SUBCASE("value")
    {
        using namespace parse_value;

        constexpr auto empty = lexy::parse<prod>(lexy::zstring_input(""), lexy::noop);
        CHECK(!empty);

        constexpr auto abc_abc = lexy::parse<prod>(lexy::zstring_input("(abc,abc)"), lexy::noop);
        CHECK(abc_abc);
        CHECK(abc_abc.value().a == "abc");
        CHECK(abc_abc.value().b == "abc");

        constexpr auto abc_123 = lexy::parse<prod>(lexy::zstring_input("(abc,123)"), lexy::noop);
        CHECK(abc_123);
        CHECK(abc_123.value().a == "abc");
        CHECK(abc_123.value().b == "123");
    }
    SUBCASE("list")
    {
        using namespace parse_list;

        auto empty = lexy::parse<prod>(lexy::zstring_input(""), lexy::noop);
        CHECK(!empty);

        auto parens = lexy::parse<prod>(lexy::zstring_input("()"), lexy::noop);
        CHECK(parens);
        CHECK(parens.value().empty());

        auto abc = lexy::parse<prod>(lexy::zstring_input("(abc)"), lexy::noop);
        CHECK(abc);
        CHECK(abc.value().size() == 1);
        CHECK(abc.value().at(0) == "abc");

        auto abc_abc = lexy::parse<prod>(lexy::zstring_input("(abc,abc)"), lexy::noop);
        CHECK(abc_abc);
        CHECK(abc_abc.value().size() == 2);
        CHECK(abc_abc.value().at(0) == "abc");
        CHECK(abc_abc.value().at(1) == "abc");

        auto abc_123 = lexy::parse<prod>(lexy::zstring_input("(abc,123)"), lexy::noop);
        CHECK(abc_123);
        CHECK(abc_123.value().size() == 2);
        CHECK(abc_123.value().at(0) == "abc");
        CHECK(abc_123.value().at(1) == "123");

        auto abc_abc_123 = lexy::parse<prod>(lexy::zstring_input("(abc,abc,123)"), lexy::noop);
        CHECK(abc_abc_123);
        CHECK(abc_abc_123.value().size() == 3);
        CHECK(abc_abc_123.value().at(0) == "abc");
        CHECK(abc_abc_123.value().at(1) == "abc");
        CHECK(abc_abc_123.value().at(2) == "123");
    }
    SUBCASE("list_value")
    {
        using namespace parse_list_value;

        auto empty = lexy::parse<prod>(lexy::zstring_input(""), lexy::noop);
        CHECK(!empty);

        auto parens = lexy::parse<prod>(lexy::zstring_input("()"), lexy::noop);
        CHECK(parens);
        CHECK(parens.value() == 0);

        auto abc = lexy::parse<prod>(lexy::zstring_input("(abc)"), lexy::noop);
        CHECK(abc);
        CHECK(abc.value() == 1);

        auto abc_abc = lexy::parse<prod>(lexy::zstring_input("(abc,abc)"), lexy::noop);
        CHECK(abc_abc);
        CHECK(abc_abc.value() == 2);

        auto abc_123 = lexy::parse<prod>(lexy::zstring_input("(abc,123)"), lexy::noop);
        CHECK(abc_123);
        CHECK(abc_123.value() == 2);

        auto abc_abc_123 = lexy::parse<prod>(lexy::zstring_input("(abc,abc,123)"), lexy::noop);
        CHECK(abc_abc_123);
        CHECK(abc_abc_123.value() == 3);
    }
}
