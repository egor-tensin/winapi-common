// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include "cmd_line.hpp"
#include "handle.hpp"

#include <utility>

namespace winapi {

class Process {
public:
    static Process create(const CommandLine&);

    void wait();

private:
    explicit Process(Handle&& handle) : m_handle{std::move(handle)} {}

    Handle m_handle;
};

} // namespace winapi
