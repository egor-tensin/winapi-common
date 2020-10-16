// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "handle.hpp"
#include "pipe.hpp"

#include <string>
#include <utility>

namespace winapi {
namespace process {

struct Stream {
    Stream(Handle&& handle) : handle{std::move(handle)} {}

    Handle handle;
};

struct Stdin : Stream {
    Stdin();
    explicit Stdin(const std::string& file);
    explicit Stdin(Pipe&);
};

struct Stdout : Stream {
    Stdout();
    explicit Stdout(const std::string& file);
    explicit Stdout(Pipe&);
};

struct Stderr : Stream {
    Stderr();
    explicit Stderr(const std::string& file);
    explicit Stderr(Pipe&);
};

} // namespace process
} // namespace winapi
