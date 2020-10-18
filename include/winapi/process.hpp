// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "cmd_line.hpp"
#include "handle.hpp"
#include "process_io.hpp"
#include "resource.hpp"

#include <boost/config.hpp>

#include <windows.h>

#include <string>
#include <utility>

namespace winapi {

class Process {
public:
    static Process create(const CommandLine&);
    static Process create(const CommandLine&, process::IO);

    static Process runas(const CommandLine&);

    void wait() const;

    int get_exit_code() const;

    static std::string get_exe_path();

    static Resource get_resource(unsigned int id);
    static std::string get_resource_string(unsigned int id);

private:
    explicit Process(Handle&& handle) : m_handle{std::move(handle)} {}

    static HMODULE get_exe_module();

    Handle m_handle;
};

} // namespace winapi
