// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "handle.hpp"
#include "path.hpp"

#include <string>
#include <utility>

namespace winapi {

class File : private Handle {
public:
    explicit File(Handle&& handle) : Handle{std::move(handle)} {}

    static Handle open_r(const std::string&);
    static Handle open_r(const CanonicalPath&);
    static Handle open_w(const std::string&);
    static Handle open_w(const CanonicalPath&);

    static void remove(const std::string&);
    static void remove(const CanonicalPath&);

    using Handle::close;

    using Handle::read;
    using Handle::write;
};

} // namespace winapi
