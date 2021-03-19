// Copyright (C) 2020-2021 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef LEXY_INPUT_FILE_HPP_INCLUDED
#define LEXY_INPUT_FILE_HPP_INCLUDED

#include <lexy/_detail/lazy_init.hpp>
#include <lexy/_detail/std.hpp>
#include <lexy/input/base.hpp>
#include <lexy/input/buffer.hpp>

namespace lexy
{
/// Errors that might occur while reading the file.
enum class file_error
{
    _success,
    /// An internal OS error, such as failure to read from the file.
    os_error,
    /// The file was not found.
    file_not_found,
    /// The file cannot be opened.
    permission_denied,
};
} // namespace lexy

namespace lexy::_detail
{
using file_callback = void (*)(void* user_data, const char* memory, std::size_t size);

// Reads the entire contents of the specified file into memory.
// On success, invokes the callback before freeing the memory.
// On error, returns the error without invoking the callback.
//
// Do not change ABI, especially with different build configurations!
file_error read_file(const char* path, file_callback cb, void* user_data);
} // namespace lexy::_detail

namespace lexy
{
template <typename Encoding       = default_encoding,
          typename MemoryResource = _detail::default_memory_resource>
class read_file_result
{
public:
    using encoding  = Encoding;
    using char_type = typename encoding::char_type;

    explicit operator bool() const noexcept
    {
        return _ec == file_error::_success;
    }

    file_error error() const noexcept
    {
        LEXY_PRECONDITION(!*this);
        return _ec;
    }

    const char_type* data() const noexcept
    {
        LEXY_PRECONDITION(*this);
        return _buffer.data();
    }
    std::size_t size() const noexcept
    {
        LEXY_PRECONDITION(*this);
        return _buffer.size();
    }

    auto reader() const& noexcept
    {
        LEXY_PRECONDITION(*this);
        return _buffer.reader();
    }

public:
    // Pretend these two don't exist.
    explicit read_file_result(file_error                               ec,
                              lexy::buffer<Encoding, MemoryResource>&& buffer) noexcept
    : _buffer(LEXY_MOV(buffer)), _ec(ec)
    {}
    explicit read_file_result(file_error ec, MemoryResource* resource) noexcept
    : _buffer(resource), _ec(ec)
    {
        LEXY_PRECONDITION(!*this);
    }

private:
    lexy::buffer<Encoding, MemoryResource> _buffer;
    file_error                             _ec;
};

/// Reads the file at the specified path into a buffer.
template <typename Encoding          = default_encoding,
          encoding_endianness Endian = encoding_endianness::bom,
          typename MemoryResource    = _detail::default_memory_resource>
auto read_file(const char*     path,
               MemoryResource* resource = _detail::get_memory_resource<MemoryResource>())
    -> read_file_result<Encoding, MemoryResource>
{
    struct user_data_t
    {
        lexy::buffer<Encoding, MemoryResource> buffer;
        MemoryResource*                        resource;

        user_data_t(MemoryResource* resource) : buffer(resource), resource(resource) {}
    } user_data(resource);

    auto error = _detail::read_file(
        path,
        [](void* _user_data, const char* memory, std::size_t size) {
            auto user_data = static_cast<user_data_t*>(_user_data);

            user_data->buffer
                = lexy::make_buffer_from_raw<Encoding, Endian>(memory, size, user_data->resource);
        },
        &user_data);

    return read_file_result(error, LEXY_MOV(user_data.buffer));
}
} // namespace lexy

#endif // LEXY_INPUT_FILE_HPP_INCLUDED

