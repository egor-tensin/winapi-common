// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "handle.hpp"
#include "path.hpp"
#include "pipe.hpp"

#include <string>
#include <utility>

namespace winapi {
namespace process {

struct Stream {
    Stream(Handle&& handle) : handle{std::move(handle)} {}

    Handle handle;
};

/** @brief Redirect child process's stdin. */
struct Stdin : Stream {
    /** No redirection. */
    Stdin();
    /** Make child process read form a file. */
    explicit Stdin(const std::string& file);
    /** @overload */
    explicit Stdin(const CanonicalPath& file);
    /** Make child process read form a pipe. */
    explicit Stdin(Pipe&);
};

/** @brief Redirect child process's stdout. */
struct Stdout : Stream {
    /** No redirection. */
    Stdout();
    /** Redirect child process's stdout to a file. */
    explicit Stdout(const std::string& file);
    /** @overload */
    explicit Stdout(const CanonicalPath& file);
    /** Redirect child process's stdout to a pipe. */
    explicit Stdout(Pipe&);
};

/** @brief Redirect child process's stderr. */
struct Stderr : Stream {
    /** No redirection. */
    Stderr();
    /** Redirect child process's stderr to a file. */
    explicit Stderr(const std::string& file);
    /** @overload */
    explicit Stderr(const CanonicalPath& file);
    /** Redirect child process's stderr to a pipe. */
    explicit Stderr(Pipe&);
};

/** @brief Child process IO settings. */
struct IO {
    IO() = default;

    void close();

    Stdin std_in;
    Stdout std_out;
    Stderr std_err;
};

} // namespace process
} // namespace winapi
