// Copyright (C) 2020-2021 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef LEXY_DSL_LOOP_HPP_INCLUDED
#define LEXY_DSL_LOOP_HPP_INCLUDED

#include <lexy/dsl/base.hpp>

namespace lexyd
{
struct _break : rule_base
{
    template <typename NextParser>
    struct parser
    {
        template <typename Context, typename Reader, typename... Args>
        LEXY_DSL_FUNC auto parse(Context& context, Reader&, Args&&...)
        {
            // We set loop break on the member with the specified id.
            context.get(_break{}).loop_break = true;

            return typename Context::result_type(lexy::result_empty);
        }
    };
};

/// Exits a loop().
constexpr auto break_ = _break{};
} // namespace lexyd

namespace lexyd
{
template <typename Rule>
struct _loop : rule_base
{
    template <typename NextParser>
    struct parser
    {
        template <typename Context, typename Reader, typename... Args>
        LEXY_DSL_FUNC auto parse(Context& context, Reader& reader, Args&&... args) ->
            typename Context::result_type
        {
            struct flag
            {
                bool loop_break = false;
            };

            auto loop_context = context.insert(_break{}, flag{});
            while (!loop_context.get(_break{}).loop_break)
            {
                using parser = lexy::rule_parser<Rule, lexy::context_discard_parser>;
                auto result  = parser::parse(loop_context, reader);
                if (result.has_error())
                    return LEXY_MOV(result);
            }

            return NextParser::parse(context, reader, LEXY_FWD(args)...);
        }
    };
};

/// Repeatedly matches the rule until a break rule matches.
template <typename Rule>
LEXY_CONSTEVAL auto loop(Rule)
{
    return _loop<Rule>{};
}
} // namespace lexyd

#endif // LEXY_DSL_LOOP_HPP_INCLUDED

