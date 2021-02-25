// Copyright (C) 2020-2021 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef LEXY_DSL_CHOICE_HPP_INCLUDED
#define LEXY_DSL_CHOICE_HPP_INCLUDED

#include <lexy/dsl/base.hpp>
#include <lexy/dsl/branch.hpp>
#include <lexy/error.hpp>

namespace lexy
{
struct exhausted_choice
{
    static LEXY_CONSTEVAL auto name()
    {
        return "exhausted choice";
    }
};
} // namespace lexy

namespace lexyd
{
template <typename NextParser, typename... R>
struct _chc_parser;
template <typename NextParser>
struct _chc_parser<NextParser>
{
    template <typename Context, typename Reader, typename... Args>
    LEXY_DSL_FUNC bool parse(Context& context, Reader& reader, Args&&...)
    {
        auto err = lexy::make_error<Reader, lexy::exhausted_choice>(reader.cur());
        context.error(err);
        return false;
    }
};
template <typename NextParser, typename H, typename... T>
struct _chc_parser<NextParser, H, T...>
{
    template <typename Context, typename Reader, typename... Args>
    LEXY_DSL_FUNC bool parse(Context& context, Reader& reader, Args&&... args)
    {
        if constexpr (H::is_unconditional_branch)
        {
            return lexy::rule_parser<H, NextParser>::parse(context, reader, LEXY_FWD(args)...);
        }
        else
        {
            auto result
                = lexy::rule_parser<H, NextParser>::try_parse(context, reader, LEXY_FWD(args)...);
            if (result == lexy::rule_try_parse_result::backtracked)
                // Try the next branch.
                return _chc_parser<NextParser, T...>::parse(context, reader, LEXY_FWD(args)...);
            else
                // Return true/false depending on result.
                return static_cast<bool>(result);
        }
    }
};

template <typename... R>
struct _chc : rule_base
{
    template <typename NextParser>
    using parser = _chc_parser<NextParser, R...>;
};

template <typename R, typename S>
LEXY_CONSTEVAL auto operator|(R, S)
{
    static_assert(lexy::is_branch<R>, "choice requires a branch condition");
    static_assert(lexy::is_branch<S>, "choice requires a branch condition");
    return _chc<R, S>{};
}
template <typename... R, typename S>
LEXY_CONSTEVAL auto operator|(_chc<R...>, S)
{
    static_assert(lexy::is_branch<S>, "choice requires a branch condition");
    return _chc<R..., S>{};
}
template <typename R, typename... S>
LEXY_CONSTEVAL auto operator|(R, _chc<S...>)
{
    static_assert(lexy::is_branch<R>, "choice requires a branch condition");
    return _chc<R, S...>{};
}
template <typename... R, typename... S>
LEXY_CONSTEVAL auto operator|(_chc<R...>, _chc<S...>)
{
    return _chc<R..., S...>{};
}
} // namespace lexyd

#endif // LEXY_DSL_CHOICE_HPP_INCLUDED

