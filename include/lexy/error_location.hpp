// Copyright (C) 2020 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef LEXY_ERROR_LOCATION_HPP_INCLUDED
#define LEXY_ERROR_LOCATION_HPP_INCLUDED

#include <lexy/dsl/base.hpp>
#include <lexy/input/base.hpp>
#include <lexy/lexeme.hpp>

namespace lexy
{
template <typename Reader>
struct error_location
{
    std::size_t line, column;
    /// The entire line where the error occurred.
    lexeme<Reader> context;
};

template <typename Input>
using error_location_for = error_location<input_reader<Input>>;

template <typename Input, typename TokenCP, typename TokenNL>
constexpr auto make_error_location(const Input& input, typename input_reader<Input>::iterator pos,
                                   TokenCP, TokenNL) -> error_location_for<Input>
{
    static_assert(is_token<TokenCP> && is_token<TokenNL>);
    using engine_cp = typename TokenCP::token_engine;
    using engine_nl = typename TokenNL::token_engine;

    auto reader = input.reader();

    // We start at the first line and first column.
    std::size_t cur_line   = 1;
    std::size_t cur_column = 1;
    auto        line_start = reader.cur();

    while (true)
    {
        if (reader.cur() == pos)
        {
            // We found the position of the error.
            break;
        }
        else if (lexy::engine_try_match<engine_nl>(reader))
        {
            // We're at a new line.
            ++cur_line;
            cur_column = 1;
            line_start = reader.cur();
        }
        else if (lexy::engine_try_match<engine_cp>(reader))
        {
            // Next column.
            ++cur_column;
        }
        else if (reader.eof())
        {
            // We have an OOB error position, return what the current position (i.e. the end of the
            // file).
            return error_location_for<Input>{cur_line, cur_column, lexeme(reader, line_start)};
        }
        else
        {
            // Invalid code unit, just ignore it in the column count.
            reader.bump();
        }
    }

    // At this point, we've found the right line and column number and the reader is at the error
    // position.
    // We now need to find the end of the current line to compute context.
    while (true)
    {
        if (lexy::engine_peek<engine_nl>(reader))
            break;
        else if (reader.eof())
            break;
        else
            reader.bump();
    }

    // We've found the right line and column number and the reader is at the end of the line.
    return error_location_for<Input>{cur_line, cur_column, lexeme(reader, line_start)};
}
} // namespace lexy

#endif // LEXY_ERROR_LOCATION_HPP_INCLUDED

