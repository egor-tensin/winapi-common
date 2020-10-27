// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include <winapi/cmd_line.hpp>
#include <winapi/error.hpp>
#include <winapi/process.hpp>
#include <winapi/process_io.hpp>
#include <winapi/resource.hpp>
#include <winapi/utf8.hpp>

#include <boost/config.hpp>

// clang-format off
#include <windows.h>
#include <shellapi.h>
// clang-format on

#include <cstddef>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace winapi {
namespace {

typedef std::vector<wchar_t> EscapedCommandLine;

EscapedCommandLine escape_command_line(const CommandLine& cmd_line) {
    const auto unicode_cmd_line = widen(cmd_line.to_string());
    EscapedCommandLine buffer;
    buffer.reserve(unicode_cmd_line.size() + 1);
    buffer.assign(unicode_cmd_line.cbegin(), unicode_cmd_line.cend());
    buffer.emplace_back(L'\0');
    return buffer;
}

Handle create_process(ProcessParameters& params) {
    /*
     * When creating a new console process, the options are:
     * 1) inherit the parent console (the default),
     * 2) CREATE_NO_WINDOW,
     * 3) CREATE_NEW_CONSOLE,
     * 4) DETACHED_PROCESS.
     *
     * Child processes can inherit the console.
     * By that I mean they will display their output in the same window.
     * If both the child process and the parent process read from stdin, there
     * is no way to say which process will read any given input byte.
     *
     * There's an excellent guide into all the intricacies of the CreateProcess
     * system call at
     *
     *     https://github.com/rprichard/win32-console-docs/blob/master/README.md
     *
     * Another useful link is https://ikriv.com/dev/cpp/ConsoleProxy/flags.
     */
    BOOST_STATIC_CONSTEXPR DWORD default_dwCreationFlags = CREATE_UNICODE_ENVIRONMENT;

    STARTUPINFOW startup_info;
    std::memset(&startup_info, 0, sizeof(startup_info));
    startup_info.cb = sizeof(startup_info);

    if (params.io) {
        startup_info.dwFlags |= STARTF_USESTDHANDLES;
        startup_info.hStdInput = static_cast<HANDLE>(params.io->std_in.handle);
        startup_info.hStdOutput = static_cast<HANDLE>(params.io->std_out.handle);
        startup_info.hStdError = static_cast<HANDLE>(params.io->std_err.handle);
    }

    auto dwCreationFlags = default_dwCreationFlags;

    switch (params.console_mode) {
        case ProcessParameters::ConsoleNone:
            dwCreationFlags |= CREATE_NO_WINDOW;
            break;
        case ProcessParameters::ConsoleInherit:
            // This is the default.
            break;
        case ProcessParameters::ConsoleNew:
            dwCreationFlags |= CREATE_NEW_CONSOLE;
            break;
    }

    PROCESS_INFORMATION child_info;
    std::memset(&child_info, 0, sizeof(child_info));

    {
        auto cmd_line = escape_command_line(params.cmd_line);

        const auto ret = ::CreateProcessW(NULL,
                                          cmd_line.data(),
                                          NULL,
                                          NULL,
                                          TRUE,
                                          dwCreationFlags,
                                          NULL,
                                          NULL,
                                          &startup_info,
                                          &child_info);

        if (!ret) {
            throw error::windows(GetLastError(), "CreateProcessW");
        }
    }

    if (params.io) {
        params.io->close();
    }

    Handle process{child_info.hProcess};
    Handle thread{child_info.hThread};

    return process;
}

Handle shell_execute(const ShellParameters& params) {
    const auto lpVerb = params.verb ? widen(*params.verb) : L"open";
    const auto lpFile = widen(params.cmd_line.get_argv0());
    const auto lpParameters = widen(params.cmd_line.args_to_string());

    BOOST_STATIC_CONSTEXPR unsigned long default_fMask =
        SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;

    auto fMask = default_fMask;
    auto nShow = SW_SHOWDEFAULT;

    switch (params.console_mode) {
        case ProcessParameters::ConsoleNone:
            nShow = SW_HIDE;
            break;
        case ProcessParameters::ConsoleInherit:
            fMask |= SEE_MASK_NO_CONSOLE;
            break;
        case ProcessParameters::ConsoleNew:
            // This is the default.
            break;
    }

    SHELLEXECUTEINFOW info;
    std::memset(&info, 0, sizeof(info));
    info.cbSize = sizeof(info);
    info.fMask = fMask;
    info.lpVerb = lpVerb.c_str();
    info.lpFile = lpFile.c_str();
    if (!lpParameters.empty())
        info.lpParameters = lpParameters.c_str();
    info.nShow = nShow;

    if (!::ShellExecuteExW(&info)) {
        throw error::windows(GetLastError(), "ShellExecuteExW");
    }

    return Handle{info.hProcess};
}

} // namespace

ProcessParameters::ProcessParameters(ProcessParameters&& other) BOOST_NOEXCEPT_OR_NOTHROW
    : cmd_line(std::move(other.cmd_line)),
      io(std::move(other.io)),
      console_mode(std::move(other.console_mode)) {}

ProcessParameters& ProcessParameters::operator=(ProcessParameters other) BOOST_NOEXCEPT_OR_NOTHROW {
    swap(other);
    return *this;
}

void ProcessParameters::swap(ProcessParameters& other) BOOST_NOEXCEPT_OR_NOTHROW {
    using std::swap;
    swap(cmd_line, other.cmd_line);
    swap(io, other.io);
    swap(console_mode, other.console_mode);
}

ShellParameters::ShellParameters(ShellParameters&& other) BOOST_NOEXCEPT_OR_NOTHROW
    : ProcessParameters(std::move(other)),
      verb(std::move(verb)) {}

ShellParameters& ShellParameters::operator=(ShellParameters other) BOOST_NOEXCEPT_OR_NOTHROW {
    swap(other);
    return *this;
}

void ShellParameters::swap(ShellParameters& other) BOOST_NOEXCEPT_OR_NOTHROW {
    using std::swap;
    ProcessParameters::swap(other);
    swap(verb, other.verb);
}

Process Process::create(ProcessParameters params) {
    return Process{create_process(params)};
}

Process Process::create(const CommandLine& cmd_line) {
    ProcessParameters params{cmd_line};
    return create(std::move(params));
}

Process Process::create(const CommandLine& cmd_line, process::IO io) {
    ProcessParameters params{cmd_line};
    params.io = std::move(io);
    return create(std::move(params));
}

Process Process::shell(const ShellParameters& params) {
    return Process{shell_execute(params)};
}

Process Process::shell(const CommandLine& cmd_line) {
    ShellParameters params{cmd_line};
    return shell(params);
}

Process::Process(Process&& other) BOOST_NOEXCEPT_OR_NOTHROW {
    swap(other);
}

Process& Process::operator=(Process other) BOOST_NOEXCEPT_OR_NOTHROW {
    swap(other);
    return *this;
}

void Process::swap(Process& other) BOOST_NOEXCEPT_OR_NOTHROW {
    using std::swap;
    swap(m_handle, other.m_handle);
}

bool Process::is_running() const {
    const auto ret = ::WaitForSingleObject(static_cast<HANDLE>(m_handle), 0);

    switch (ret) {
        case WAIT_OBJECT_0:
            return false;
        case WAIT_TIMEOUT:
            return true;
        case WAIT_FAILED:
            throw error::windows(GetLastError(), "WaitForSingleObject");
        default:
            // Shouldn't happen.
            throw error::custom(ret, "WaitForSingleObject");
    }
}

void Process::wait() const {
    const auto ret = ::WaitForSingleObject(static_cast<HANDLE>(m_handle), INFINITE);

    switch (ret) {
        case WAIT_OBJECT_0:
            return;
        case WAIT_FAILED:
            throw error::windows(GetLastError(), "WaitForSingleObject");
        default:
            // Shouldn't happen.
            throw error::custom(ret, "WaitForSingleObject");
    }
}

void Process::terminate(int ec) const {
    if (!::TerminateProcess(static_cast<HANDLE>(m_handle), static_cast<UINT>(ec))) {
        throw error::windows(GetLastError(), "TerminateProcess");
    }
}

void Process::shut_down(int ec) const {
    terminate(ec);
    wait();
}

int Process::get_exit_code() const {
    DWORD ec = 0;

    const auto ret = ::GetExitCodeProcess(static_cast<HANDLE>(m_handle), &ec);

    if (!ret) {
        throw error::windows(GetLastError(), "GetExitCodeProcess");
    }

    if (ec == STILL_ACTIVE) {
        throw std::runtime_error{"Attempted to query the exit code of a running process"};
    }

    return static_cast<int>(ec);
}

HMODULE Process::get_exe_module() {
    const auto module = ::GetModuleHandleW(NULL);
    if (module == NULL) {
        throw error::windows(GetLastError(), "GetModuleHandleW");
    }
    return module;
}

std::string Process::get_exe_path() {
    BOOST_STATIC_CONSTEXPR std::size_t init_buffer_size = MAX_PATH;
    static_assert(init_buffer_size > 0, "init_buffer_size must be positive");

    std::vector<wchar_t> buffer;
    buffer.resize(init_buffer_size);

    while (true) {
        SetLastError(ERROR_SUCCESS);

        const auto nch = ::GetModuleFileNameW(NULL, buffer.data(), buffer.size());

        if (nch == 0) {
            throw error::windows(GetLastError(), "GetModuleFileNameW");
        }

        if (nch == buffer.size() && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            buffer.resize(2 * buffer.size());
            continue;
        }

        buffer.resize(nch);
        return narrow(buffer);
    }
}

std::string Process::get_resource_string(unsigned int id) {
    wchar_t* s = nullptr;

    const auto nch = ::LoadStringW(get_exe_module(), id, reinterpret_cast<wchar_t*>(&s), 0);

    if (nch <= 0) {
        throw error::windows(GetLastError(), "LoadStringW");
    }

    return narrow(s, nch * sizeof(wchar_t));
}

Resource Process::get_resource(unsigned int id) {
    const auto module = get_exe_module();

    const auto src = ::FindResourceA(module, MAKEINTRESOURCEA(id), RT_RCDATA);

    if (src == NULL) {
        throw error::windows(GetLastError(), "FindResourceA");
    }

    const auto resource = ::LoadResource(module, src);

    if (resource == NULL) {
        throw error::windows(GetLastError(), "LoadResource");
    }

    const auto data = ::LockResource(resource);

    if (data == NULL) {
        std::ostringstream oss;
        oss << "Couldn't get data pointer for resource with ID " << id;
        throw std::runtime_error{oss.str()};
    }

    const auto nb = ::SizeofResource(module, src);

    return {data, nb};
}

} // namespace winapi
