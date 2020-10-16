// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "buffer.hpp"

#include <boost/config.hpp>

#include <windows.h>

#include <cstddef>
#include <memory>
#include <utility>

namespace winapi {

class Handle {
public:
    Handle() = default;
    explicit Handle(HANDLE);

    Handle(Handle&& other) BOOST_NOEXCEPT_OR_NOTHROW;
    Handle& operator=(Handle other) BOOST_NOEXCEPT_OR_NOTHROW;

    void swap(Handle& other) BOOST_NOEXCEPT_OR_NOTHROW;

    explicit operator HANDLE() const { return m_impl.get(); }

    bool is_invalid() const;

    void close();

    bool is_std() const;
    static Handle std_in();
    static Handle std_out();
    static Handle std_err();

    Buffer read() const;

    BOOST_STATIC_CONSTEXPR std::size_t max_chunk_size = 16 * 1024;
    bool read_chunk(Buffer& read_chunk) const;

    void write(const void*, std::size_t nb) const;
    void write(const Buffer& buffer) const;

    void inherit(bool yes = true) const;
    void dont_inherit() const { inherit(false); }

private:
    struct Close {
        void operator()(HANDLE) const;
    };

    std::unique_ptr<void, Close> m_impl;

    Handle(const Handle&) = delete;
};

inline void swap(Handle& a, Handle& b) BOOST_NOEXCEPT_OR_NOTHROW {
    a.swap(b);
}

} // namespace winapi

namespace std {

template <>
inline void swap(winapi::Handle& a, winapi::Handle& b) BOOST_NOEXCEPT_OR_NOTHROW {
    a.swap(b);
}

} // namespace std
