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
    Stream(Stream&& other) BOOST_NOEXCEPT_OR_NOTHROW;
    Stream& operator=(Stream other) BOOST_NOEXCEPT_OR_NOTHROW;
    void swap(Stream& other) BOOST_NOEXCEPT_OR_NOTHROW;
    Stream(const Stream&) = delete;
};

void swap(Stream& a, Stream& b) BOOST_NOEXCEPT_OR_NOTHROW;

struct Stdin : Stream {
    Stdin();
    explicit Stdin(const std::string& file);
    explicit Stdin(const CanonicalPath& file);
    explicit Stdin(Pipe&);

    // VS 2013 won't generate these automatically.
    Stdin(Stdin&& other) BOOST_NOEXCEPT_OR_NOTHROW;
    Stdin& operator=(Stdin other) BOOST_NOEXCEPT_OR_NOTHROW;
    Stdin(const Stdin&) = delete;
};

struct Stdout : Stream {
    Stdout();
    explicit Stdout(const std::string& file);
    explicit Stdout(const CanonicalPath& file);
    explicit Stdout(Pipe&);

    // VS 2013 won't generate these automatically.
    Stdout(Stdout&& other) BOOST_NOEXCEPT_OR_NOTHROW;
    Stdout& operator=(Stdout other) BOOST_NOEXCEPT_OR_NOTHROW;
    Stdout(const Stdout&) = delete;
};

struct Stderr : Stream {
    Stderr();
    explicit Stderr(const std::string& file);
    explicit Stderr(const CanonicalPath& file);
    explicit Stderr(Pipe&);

    // VS 2013 won't generate these automatically.
    Stderr(Stderr&& other) BOOST_NOEXCEPT_OR_NOTHROW;
    Stderr& operator=(Stderr other) BOOST_NOEXCEPT_OR_NOTHROW;
    Stderr(const Stderr&) = delete;
};

struct IO {
    IO() = default;

    void close();

    process::Stdin std_in;
    process::Stdout std_out;
    process::Stderr std_err;

    // VS 2013 won't generate these automatically.
    IO(IO&& other) BOOST_NOEXCEPT_OR_NOTHROW;
    IO& operator=(IO other) BOOST_NOEXCEPT_OR_NOTHROW;
    void swap(IO& other) BOOST_NOEXCEPT_OR_NOTHROW;
    IO(const IO&) = delete;
};

void swap(IO& a, IO& b) BOOST_NOEXCEPT_OR_NOTHROW;

} // namespace process
} // namespace winapi

namespace std {

template <>
inline void swap(winapi::process::Stream& a, winapi::process::Stream& b) BOOST_NOEXCEPT_OR_NOTHROW {
    a.swap(b);
}

template <>
inline void swap(winapi::process::IO& a, winapi::process::IO& b) BOOST_NOEXCEPT_OR_NOTHROW {
    a.swap(b);
}

} // namespace std
