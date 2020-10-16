// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "cmd_line.hpp"
#include "handle.hpp"
#include "stream.hpp"

#include <utility>

namespace winapi {

class Process {
public:
    struct IO {
        process::Stdin std_in;
        process::Stdout std_out;
        process::Stderr std_err;

        void close();
    };

    static Process create(const CommandLine&);
    static Process create(const CommandLine&, IO);

    void wait();

private:
    explicit Process(Handle&& handle) : m_handle{std::move(handle)} {}

    Handle m_handle;
};

} // namespace winapi
