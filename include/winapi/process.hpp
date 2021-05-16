// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "cmd_line.hpp"
#include "handle.hpp"
#include "process_io.hpp"
#include "resource.hpp"

#include <boost/optional.hpp>

#include <windows.h>

#include <cstdint>
#include <string>
#include <utility>

namespace winapi {

struct ProcessParameters {
    enum ConsoleCreationMode {
        ConsoleNone,
        ConsoleInherit,
        ConsoleNew,
    };

    explicit ProcessParameters(const CommandLine& cmd_line) : cmd_line{cmd_line} {}

    CommandLine cmd_line;
    boost::optional<process::IO> io;
    ConsoleCreationMode console_mode = ConsoleNew;
};

struct ShellParameters : ProcessParameters {
    static ShellParameters runas(const CommandLine& cmd_line) {
        ShellParameters params{cmd_line};
        params.verb = "runas";
        return params;
    }

    explicit ShellParameters(const CommandLine& cmd_line) : ProcessParameters{cmd_line} {}

    boost::optional<std::string> verb;
};

class Process {
public:
    using ID = DWORD;

    static Process create(ProcessParameters);
    static Process create(const CommandLine&);
    static Process create(const CommandLine&, process::IO);

    static Process shell(const ShellParameters&);
    static Process shell(const CommandLine&);

    static Process current();
    static Process open(ID, DWORD permissions = default_permissions());
    static Process open_r(ID);

    static DWORD default_permissions();
    static DWORD read_permissions();

    ID get_id() const { return m_id; }
    const Handle& get_handle() const { return m_handle; }

    bool is_running() const;
    void wait() const;
    void terminate(int ec = 0) const;
    void shut_down(int ec = 0) const;
    int get_exit_code() const;

    std::string get_exe_path() const;

    static Resource get_resource(uint32_t id);
    static std::string get_resource_string(uint32_t id);

private:
    explicit Process(Handle&& handle);
    Process(ID, Handle&& handle);

    static HMODULE get_exe_module();

    ID m_id;
    Handle m_handle;
};

} // namespace winapi
