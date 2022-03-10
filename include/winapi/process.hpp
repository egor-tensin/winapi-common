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

/** @brief Process parameters for Process::create(). */
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

/** @brief Process parameters for Process::shell(). */
struct ShellParameters : ProcessParameters {
    static ShellParameters runas(const CommandLine& cmd_line) {
        ShellParameters params{cmd_line};
        params.verb = "runas";
        return params;
    }

    explicit ShellParameters(const CommandLine& cmd_line) : ProcessParameters{cmd_line} {}

    boost::optional<std::string> verb;
};

/**
 * @brief Create a new process or open an existing process.
 */
class Process {
public:
    using ID = DWORD;

    /** Create a new process using ProcessParameters. */
    static Process create(ProcessParameters);
    /** Create a new process using the given command line. */
    static Process create(const CommandLine&);
    /** Create a new process using the given command line and IO settings. */
    static Process create(const CommandLine&, process::IO);

    /** Create a new shell process using ShellParameters. */
    static Process shell(const ShellParameters&);
    /** Create a new shell process using the given command line. */
    static Process shell(const CommandLine&);

    /** Open the current process. */
    static Process current();
    /**
     * Open an existing process.
     * @param id          Process ID.
     * @param permissions Required permissions.
     */
    static Process open(ID id, DWORD permissions = default_permissions());
    /** Open an existing process with read permissions. */
    static Process open_r(ID);

    /** Permissions that allow to query process's status. */
    static DWORD default_permissions();
    /** Permissions that allows to read process's memory. */
    static DWORD read_permissions();

    /** Get this process's ID. */
    ID get_id() const { return m_id; }
    /** Get this process's handle. */
    const Handle& get_handle() const { return m_handle; }

    /** Check if this process is running (i.e. not terminated). */
    bool is_running() const;
    /** Wait for the process to terminate. */
    void wait() const;
    /** Make this process terminate with an exit code. */
    void terminate(int ec = 0) const;
    /** Same as calling terminate() and wait(). */
    void shut_down(int ec = 0) const;
    /** Get terminated process's exit code. */
    int get_exit_code() const;

    /** Get this process's executable path. */
    std::string get_exe_path() const;

    /** Get a binary resource from the process's executable. */
    static Resource get_resource(uint32_t id);
    /**
     * Get a string resource from the process's executable.
     * @return UTF-8 string.
     */
    static std::string get_resource_string(uint32_t id);

private:
    explicit Process(Handle&& handle);
    Process(ID, Handle&& handle);

    static HMODULE get_exe_module();

    ID m_id;
    Handle m_handle;
};

} // namespace winapi
