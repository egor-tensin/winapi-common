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

Handle create_process(EscapedCommandLine cmd_line, process::IO& io) {
    BOOST_STATIC_CONSTEXPR DWORD flags = /*CREATE_NO_WINDOW | */ CREATE_UNICODE_ENVIRONMENT;

    STARTUPINFOW startup_info;
    std::memset(&startup_info, 0, sizeof(startup_info));
    startup_info.cb = sizeof(startup_info);
    startup_info.dwFlags = STARTF_USESTDHANDLES;
    startup_info.hStdInput = static_cast<HANDLE>(io.std_in.handle);
    startup_info.hStdOutput = static_cast<HANDLE>(io.std_out.handle);
    startup_info.hStdError = static_cast<HANDLE>(io.std_err.handle);

    PROCESS_INFORMATION child_info;
    std::memset(&child_info, 0, sizeof(child_info));

    const auto ret = ::CreateProcessW(
        NULL, cmd_line.data(), NULL, NULL, TRUE, flags, NULL, NULL, &startup_info, &child_info);

    if (!ret) {
        throw error::windows(GetLastError(), "CreateProcessW");
    }

    io.close();

    Handle process{child_info.hProcess};
    Handle thread{child_info.hThread};

    return process;
}

EscapedCommandLine escape_command_line(const CommandLine& cmd_line) {
    const auto unicode_cmd_line = widen(cmd_line.to_string());
    EscapedCommandLine buffer;
    buffer.reserve(unicode_cmd_line.size() + 1);
    buffer.assign(unicode_cmd_line.cbegin(), unicode_cmd_line.cend());
    buffer.emplace_back(L'\0');
    return buffer;
}

Handle create_process(const CommandLine& cmd_line, process::IO& io) {
    return create_process(escape_command_line(cmd_line), io);
}

Handle shell_execute(const CommandLine& cmd_line) {
    BOOST_STATIC_CONSTEXPR unsigned long flags =
        SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI | SEE_MASK_NO_CONSOLE;

    const auto exe_path = widen(cmd_line.get_argv0());
    const auto args = widen(cmd_line.args_to_string());

    SHELLEXECUTEINFOW info;
    std::memset(&info, 0, sizeof(info));
    info.cbSize = sizeof(info);
    info.fMask = flags;
    info.lpVerb = L"runas";
    info.lpFile = exe_path.c_str();
    if (!args.empty())
        info.lpParameters = args.c_str();
    info.nShow = SW_SHOWDEFAULT;

    if (!::ShellExecuteExW(&info)) {
        throw error::windows(GetLastError(), "ShellExecuteExW");
    }

    return Handle{info.hProcess};
}

} // namespace

Process Process::create(const CommandLine& cmd_line) {
    return create(cmd_line, {});
}

Process Process::create(const CommandLine& cmd_line, process::IO io) {
    return Process{create_process(cmd_line, io)};
}

Process Process::runas(const CommandLine& cmd_line) {
    return Process{shell_execute(cmd_line)};
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
