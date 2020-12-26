// Copyright (C) 2020 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef LEXY_DSL_PRODUCTION_HPP_INCLUDED
#define LEXY_DSL_PRODUCTION_HPP_INCLUDED

#include <lexy/dsl/base.hpp>
#include <lexy/dsl/branch.hpp>
#include <lexy/dsl/whitespace.hpp>
#include <lexy/result.hpp>

namespace lexyd
{
template <typename Production>
using _production_rule = std::remove_const_t<decltype(Production::rule)>;

// Not inline: one function per production.
template <typename Rule, typename Handler, typename Reader>
constexpr auto _parse(Handler& handler, Reader& reader) -> typename Handler::result_type
{
    return Rule::template parser<lexy::final_parser>::parse(handler, reader);
}

template <typename Production, typename Rule, typename NextParser>
struct _prd_parser
{
    template <typename Handler, typename Reader, typename... Args>
    LEXY_DSL_FUNC auto parse(Handler& handler, Reader& reader, Args&&... args) ->
        typename Handler::result_type
    {
        auto sub_handler = handler.template sub_handler<Production>(reader);

        if (auto result = _parse<Rule>(sub_handler, reader))
        {
            if constexpr (result.has_void_value())
                return NextParser::parse(handler, reader, LEXY_FWD(args)...);
            else
                return NextParser::parse(handler, reader, LEXY_FWD(args)...,
                                         LEXY_MOV(result).value());
        }
        else
            return typename Handler::result_type(LEXY_MOV(result));
    }
};

template <typename Production, typename Rule = void>
struct _prd : rule_base
{
    using _rule = std::conditional_t<std::is_void_v<Rule>, _production_rule<Production>, Rule>;

    static constexpr bool is_branch = lexy::is_branch<_rule>;

    template <typename Reader>
    struct branch_matcher
    {
        lexy::branch_matcher<_rule, Reader> _impl;

        static constexpr auto is_unconditional = decltype(_impl)::is_unconditional;

        constexpr bool match(Reader& reader)
        {
            return _impl.match(reader);
        }

        template <typename NextParser, typename Handler, typename... Args>
        constexpr auto parse(Handler& handler, Reader& reader, Args&&... args)
        {
            auto sub_handler = handler.template sub_handler<Production>(reader);

            if (auto result = _impl.template parse<lexy::final_parser>(sub_handler, reader))
            {
                if constexpr (result.has_void_value())
                    return NextParser::parse(handler, reader, LEXY_FWD(args)...);
                else
                    return NextParser::parse(handler, reader, LEXY_FWD(args)...,
                                             LEXY_MOV(result).value());
            }
            else
                return typename Handler::result_type(LEXY_MOV(result));
        }
    };

    static constexpr auto has_matcher = false;

    template <typename NextParser>
    using parser = _prd_parser<Production, _rule, NextParser>;

    template <typename Whitespace>
    LEXY_CONSTEVAL auto operator[](Whitespace ws) const
    {
        return whitespaced(*this, ws);
    }
};

/// Parses the production.
template <typename Production>
constexpr auto p = _prd<Production>{};

template <typename Production>
struct _rec : rule_base
{
    static constexpr auto has_matcher = false;

    template <typename NextParser>
    struct parser : _prd_parser<Production, _production_rule<Production>, NextParser>
    {};

    template <typename Whitespace>
    LEXY_CONSTEVAL auto operator[](Whitespace ws) const
    {
        return whitespaced(*this, ws);
    }
};

/// Parses the production, recursively.
/// `dsl::p` requires that the production is already defined in order to propagate a branch
/// condition outwards.
template <typename Production>
constexpr auto recurse = _rec<Production>{};
} // namespace lexyd

#endif // LEXY_DSL_PRODUCTION_HPP_INCLUDED
