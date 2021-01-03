// Copyright (C) 2020-2021 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef LEXY_DETAIL_STRING_VIEW_HPP_INCLUDED
#define LEXY_DETAIL_STRING_VIEW_HPP_INCLUDED

#include <lexy/_detail/assert.hpp>
#include <lexy/_detail/config.hpp>

namespace lexy::_detail
{
template <typename CharT>
class basic_string_view
{
public:
    using char_type = CharT;

    //=== constructor ===//
    constexpr basic_string_view() noexcept : _ptr(nullptr), _size(0u) {}

    constexpr basic_string_view(const char_type* str) noexcept : _ptr(str), _size(0u)
    {
        while (*str++)
            ++_size;
    }

    constexpr basic_string_view(const char_type* ptr, std::size_t size) noexcept
    : _ptr(ptr), _size(size)
    {}

    constexpr basic_string_view(const char_type* begin, const char_type* end) noexcept
    : _ptr(begin), _size(std::size_t(end - begin))
    {
        LEXY_PRECONDITION(begin <= end);
    }

    //=== access ===//
    using iterator = const char_type*;

    constexpr iterator begin() const noexcept
    {
        return _ptr;
    }
    constexpr iterator end() const noexcept
    {
        return _ptr + _size;
    }

    constexpr bool empty() const noexcept
    {
        return _size == 0u;
    }
    constexpr std::size_t size() const noexcept
    {
        return _size;
    }
    constexpr std::size_t length() const noexcept
    {
        return _size;
    }

    constexpr char_type operator[](std::size_t i) const noexcept
    {
        LEXY_PRECONDITION(i <= _size);
        return _ptr[i];
    }
    constexpr char_type front() const noexcept
    {
        LEXY_PRECONDITION(!empty());
        return *_ptr;
    }
    constexpr char_type back() const noexcept
    {
        LEXY_PRECONDITION(!empty());
        return _ptr[_size - 1];
    }

    constexpr const char_type* data() const noexcept
    {
        return _ptr;
    }

    //=== operations ===//
    static constexpr std::size_t npos = std::size_t(-1);

    constexpr void remove_prefix(std::size_t n) noexcept
    {
        LEXY_PRECONDITION(n <= _size);
        _ptr += n;
        _size -= n;
    }
    constexpr void remove_suffix(std::size_t n) noexcept
    {
        LEXY_PRECONDITION(n <= _size);
        _size -= n;
    }

    constexpr basic_string_view substr(std::size_t pos, std::size_t length = npos) const noexcept
    {
        LEXY_PRECONDITION(pos < _size);
        if (length >= _size - pos)
        {
            auto begin = _ptr + pos;
            auto end   = _ptr + _size;
            return basic_string_view(begin, end);
        }
        else
            return basic_string_view(_ptr + pos, length);
    }

    constexpr bool starts_with(basic_string_view prefix) const
    {
        return substr(0, prefix.size()) == prefix;
    }

    constexpr std::size_t find(basic_string_view str, std::size_t pos = 0) const noexcept
    {
        for (auto i = pos; i < length(); ++i)
        {
            if (substr(i, str.length()) == str)
                return i;
        }

        return npos;
    }
    constexpr std::size_t find(CharT c, std::size_t pos = 0) const noexcept
    {
        return find(basic_string_view(&c, 1), pos);
    }

    //=== comparison ===//
    friend constexpr bool operator==(basic_string_view<CharT> lhs,
                                     basic_string_view<CharT> rhs) noexcept
    {
        if (lhs.size() != rhs.size())
            return false;

        for (auto a = lhs.begin(), b = rhs.begin(); a != lhs.end(); ++a, ++b)
            if (*a != *b)
                return false;

        return true;
    }

    friend constexpr bool operator!=(basic_string_view<CharT> lhs,
                                     basic_string_view<CharT> rhs) noexcept
    {
        return !(lhs == rhs);
    }

private:
    const CharT* _ptr;
    std::size_t  _size;
};
using string_view = basic_string_view<char>;
} // namespace lexy::_detail

#endif // LEXY_DETAIL_STRING_VIEW_HPP_INCLUDED
