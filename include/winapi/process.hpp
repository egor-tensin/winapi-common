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
#include <boost/optional.hpp>

#include <windows.h>

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
    explicit ShellParameters(const CommandLine& cmd_line) : ProcessParameters{cmd_line} {}

    static ShellParameters runas(const CommandLine& cmd_line) {
        ShellParameters params{cmd_line};
        params.verb = "runas";
        return params;
    }

    boost::optional<std::string> verb;
};

class Process {
public:
    static Process create(ProcessParameters);
    static Process create(const CommandLine&);
    static Process create(const CommandLine&, process::IO);

    static Process shell(const ShellParameters&);
    static Process shell(const CommandLine&);

    bool is_running() const;
    void wait() const;
    void terminate(int ec = 0) const;
    void shut_down(int ec = 0) const;
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
