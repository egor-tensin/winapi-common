// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "handle.hpp"

#include <boost/config.hpp>

#include <utility>

namespace winapi {

class Pipe {
public:
    Pipe();

    // VS 2013 won't generate these automatically.
    Pipe(Pipe&&) BOOST_NOEXCEPT_OR_NOTHROW;
    Pipe& operator=(Pipe) BOOST_NOEXCEPT_OR_NOTHROW;
    void swap(Pipe&) BOOST_NOEXCEPT_OR_NOTHROW;
    Pipe(const Pipe&) = delete;

    Handle& read_end() { return m_read_end; }
    const Handle& read_end() const { return m_read_end; }
    Handle& write_end() { return m_write_end; }
    const Handle& write_end() const { return m_write_end; }

private:
    Handle m_read_end;
    Handle m_write_end;
};

inline void swap(Pipe& a, Pipe& b) BOOST_NOEXCEPT_OR_NOTHROW {
    a.swap(b);
}

} // namespace winapi

namespace std {

template <>
inline void swap(winapi::Pipe& a, winapi::Pipe& b) BOOST_NOEXCEPT_OR_NOTHROW {
    a.swap(b);
}

} // namespace std
