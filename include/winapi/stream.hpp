// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "handle.hpp"
#include "path.hpp"
#include "pipe.hpp"

#include <boost/config.hpp>

#include <string>
#include <utility>

namespace winapi {
namespace process {

struct Stream {
    Stream(Handle&& handle) : handle{std::move(handle)} {}

    Handle handle;

    // VS 2013 won't generate these automatically.

    Stream(Stream&& other) BOOST_NOEXCEPT_OR_NOTHROW { swap(other); }

    Stream& operator=(Stream other) BOOST_NOEXCEPT_OR_NOTHROW {
        swap(other);
        return *this;
    }

    void swap(Stream& other) BOOST_NOEXCEPT_OR_NOTHROW {
        using std::swap;
        swap(handle, other.handle);
    }

    Stream(const Stream&) = delete;
};

inline void swap(Stream& a, Stream& b) BOOST_NOEXCEPT_OR_NOTHROW {
    a.swap(b);
}

struct Stdin : Stream {
    Stdin();
    explicit Stdin(const std::string& file);
    explicit Stdin(const CanonicalPath& file);
    explicit Stdin(Pipe&);

    // VS 2013 won't generate these automatically.

    Stdin(Stdin&& other) BOOST_NOEXCEPT_OR_NOTHROW : Stream{std::move(other)} {}

    Stdin& operator=(Stdin other) BOOST_NOEXCEPT_OR_NOTHROW {
        Stream::operator=(std::move(other));
        return *this;
    }

    Stdin(const Stdin&) = delete;
};

struct Stdout : Stream {
    Stdout();
    explicit Stdout(const std::string& file);
    explicit Stdout(const CanonicalPath& file);
    explicit Stdout(Pipe&);

    // VS 2013 won't generate these automatically.

    Stdout(Stdout&& other) BOOST_NOEXCEPT_OR_NOTHROW : Stream{std::move(other)} {}

    Stdout& operator=(Stdout other) BOOST_NOEXCEPT_OR_NOTHROW {
        Stream::operator=(std::move(other));
        return *this;
    }

    Stdout(const Stdout&) = delete;
};

struct Stderr : Stream {
    Stderr();
    explicit Stderr(const std::string& file);
    explicit Stderr(const CanonicalPath& file);
    explicit Stderr(Pipe&);

    // VS 2013 won't generate these automatically.

    Stderr(Stderr&& other) BOOST_NOEXCEPT_OR_NOTHROW : Stream{std::move(other)} {}

    Stderr& operator=(Stderr other) BOOST_NOEXCEPT_OR_NOTHROW {
        Stream::operator=(std::move(other));
        return *this;
    }

    Stderr(const Stderr&) = delete;
};

} // namespace process
} // namespace winapi

namespace std {

template <>
inline void swap(winapi::process::Stream& a, winapi::process::Stream& b) BOOST_NOEXCEPT_OR_NOTHROW {
    a.swap(b);
}

} // namespace std
