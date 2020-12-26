// Copyright (C) 2020 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef LEXY_DSL_PEEK_HPP_INCLUDED
#define LEXY_DSL_PEEK_HPP_INCLUDED

#include <lexy/dsl/base.hpp>
#include <lexy/dsl/whitespace.hpp>

namespace lexyd
{
template <typename Pattern>
struct _peek : rule_base
{
    static constexpr auto has_matcher = true;

    struct matcher
    {
        template <typename Reader>
        LEXY_DSL_FUNC bool match(Reader& reader)
        {
            auto copy = reader;
            return Pattern::matcher::match(copy);
        }
    };

    static constexpr auto is_branch = true;

    template <typename Reader>
    struct branch_matcher
    {
        static constexpr auto is_unconditional = false;

        constexpr bool match(Reader& reader)
        {
            auto copy = reader;
            return Pattern::matcher::match(copy);
        }

        template <typename NextParser, typename Handler, typename... Args>
        constexpr auto parse(Handler& handler, Reader& reader, Args&&... args)
        {
            return NextParser::parse(handler, reader, LEXY_FWD(args)...);
        }
    };

    template <typename NextParser>
    struct parser
    {
        struct _continuation
        {
            template <typename Handler, typename Reader, typename... Args>
            LEXY_DSL_FUNC auto parse(Handler& handler, Reader& reader, Reader save, Args&&... args)
                -> typename Handler::result_type
            {
                // We continue with the reader as it was before we parsed the pattern.
                reader = LEXY_MOV(save);
                return NextParser::parse(handler, reader, LEXY_FWD(args)...);
            }
        };

        template <typename Handler, typename Reader, typename... Args>
        LEXY_DSL_FUNC auto parse(Handler& handler, Reader& reader, Args&&... args) ->
            typename Handler::result_type
        {
            // We parse the Pattern but remember the previous reader to reset it.
            return Pattern::template parser<_continuation>::parse(handler, reader, Reader(reader),
                                                                  LEXY_FWD(args)...);
        }
    };

    template <typename Whitespace>
    LEXY_CONSTEVAL auto operator[](Whitespace ws) const
    {
        return whitespaced(*this, ws);
    }
};

/// Check if at this reader position, Pattern would match, but don't actually consume any characters
/// if it does.
template <typename Pattern>
LEXY_CONSTEVAL auto peek(Pattern)
{
    static_assert(lexy::is_pattern<Pattern>);
    return _peek<Pattern>{};
}
} // namespace lexyd

namespace lexy
{
struct unexpected
{
    static LEXY_CONSTEVAL auto name()
    {
        return "unexpected";
    }
};
} // namespace lexy

namespace lexyd
{
template <typename Pattern>
struct _peekn : rule_base
{
    static constexpr auto has_matcher = true;

    struct matcher
    {
        template <typename Reader>
        LEXY_DSL_FUNC bool match(Reader& reader)
        {
            auto copy = reader;
            return !Pattern::matcher::match(copy);
        }
    };

    static constexpr auto is_branch = true;

    template <typename Reader>
    struct branch_matcher
    {
        static constexpr auto is_unconditional = false;

        constexpr bool match(Reader& reader) const
        {
            auto copy = reader;
            return Pattern::matcher::match(copy);
        }

        template <typename NextParser, typename Handler, typename... Args>
        constexpr auto parse(Handler& handler, Reader& reader, Args&&... args)
        {
            return NextParser::parse(handler, reader, LEXY_FWD(args)...);
        }
    };

    template <typename NextParser>
    struct parser
    {
        template <typename Handler, typename Reader, typename... Args>
        LEXY_DSL_FUNC auto parse(Handler& handler, Reader& reader, Args&&... args) ->
            typename Handler::result_type
        {
            auto copy = reader;
            if (auto pos = copy.cur(); Pattern::matcher::match(copy))
            {
                auto e = lexy::make_error<Reader, lexy::unexpected>(pos, copy.cur());
                return LEXY_MOV(handler).error(e);
            }

            return NextParser::parse(handler, reader, LEXY_FWD(args)...);
        }
    };

    template <typename Whitespace>
    LEXY_CONSTEVAL auto operator[](Whitespace ws) const
    {
        return whitespaced(*this, ws);
    }
};

/// Check if at this reader position, Pattern would not match, but don't actually consume any
/// characters if it does.
template <typename Pattern>
LEXY_CONSTEVAL auto peek_not(Pattern)
{
    static_assert(lexy::is_pattern<Pattern>);
    return _peekn<Pattern>{};
}
} // namespace lexyd

#endif // LEXY_DSL_PEEK_HPP_INCLUDED

