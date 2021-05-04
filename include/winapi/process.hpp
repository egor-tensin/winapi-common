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

    explicit ProcessParameters(const CommandLine& cmd_line) : cmd_line(cmd_line) {}

    // VS 2013 won't generate these automatically.
    ProcessParameters(ProcessParameters&&) BOOST_NOEXCEPT_OR_NOTHROW;
    ProcessParameters& operator=(ProcessParameters) BOOST_NOEXCEPT_OR_NOTHROW;
    void swap(ProcessParameters& other) BOOST_NOEXCEPT_OR_NOTHROW;
    ProcessParameters(const ProcessParameters&) = delete;

    CommandLine cmd_line;
    boost::optional<process::IO> io;
    ConsoleCreationMode console_mode = ConsoleNew;
};

inline void swap(ProcessParameters& a, ProcessParameters& b) BOOST_NOEXCEPT_OR_NOTHROW {
    a.swap(b);
}

struct ShellParameters : ProcessParameters {
    static ShellParameters runas(const CommandLine& cmd_line) {
        ShellParameters params{cmd_line};
        params.verb = "runas";
        return params;
    }

    explicit ShellParameters(const CommandLine& cmd_line) : ProcessParameters(cmd_line) {}

    // VS 2013 won't generate these automatically.
    ShellParameters(ShellParameters&&) BOOST_NOEXCEPT_OR_NOTHROW;
    ShellParameters& operator=(ShellParameters) BOOST_NOEXCEPT_OR_NOTHROW;
    void swap(ShellParameters& other) BOOST_NOEXCEPT_OR_NOTHROW;
    ShellParameters(const ShellParameters&) = delete;

    boost::optional<std::string> verb;
};

inline void swap(ShellParameters& a, ShellParameters& b) BOOST_NOEXCEPT_OR_NOTHROW {
    a.swap(b);
}

class Process {
public:
    static Process create(ProcessParameters);
    static Process create(const CommandLine&);
    static Process create(const CommandLine&, process::IO);

    static Process shell(const ShellParameters&);
    static Process shell(const CommandLine&);

    // VS 2013 won't generate these automatically.
    Process(Process&&) BOOST_NOEXCEPT_OR_NOTHROW;
    Process& operator=(Process) BOOST_NOEXCEPT_OR_NOTHROW;
    void swap(Process& other) BOOST_NOEXCEPT_OR_NOTHROW;
    Process(const Process&) = delete;

    bool is_running() const;
    void wait() const;
    void terminate(int ec = 0) const;
    void shut_down(int ec = 0) const;
    int get_exit_code() const;

    static std::string get_exe_path();

    static Resource get_resource(uint32_t id);
    static std::string get_resource_string(uint32_t id);

private:
    explicit Process(Handle&& handle) : m_handle(std::move(handle)) {}

    static HMODULE get_exe_module();

    Handle m_handle;
};

inline void swap(Process& a, Process& b) BOOST_NOEXCEPT_OR_NOTHROW {
    a.swap(b);
}

} // namespace winapi

namespace std {

template <>
inline void swap(winapi::ProcessParameters& a,
                 winapi::ProcessParameters& b) BOOST_NOEXCEPT_OR_NOTHROW {
    a.swap(b);
}

template <>
inline void swap(winapi::ShellParameters& a, winapi::ShellParameters& b) BOOST_NOEXCEPT_OR_NOTHROW {
    a.swap(b);
}

template <>
inline void swap(winapi::Process& a, winapi::Process& b) BOOST_NOEXCEPT_OR_NOTHROW {
    a.swap(b);
}

} // namespace std
