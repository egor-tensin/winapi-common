// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include <winapi/cmd_line.hpp>
#include <winapi/error.hpp>
#include <winapi/process.hpp>
#include <winapi/utf8.hpp>

#include <boost/config.hpp>

#include <windows.h>

#include <cstring>
#include <stdexcept>
#include <utility>
#include <vector>

namespace winapi {
namespace {

typedef std::vector<wchar_t> EscapedCommandLine;

Handle create_process(EscapedCommandLine cmd_line, Process::IO& io) {
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

    return std::move(process);
}

EscapedCommandLine escape_command_line(const CommandLine& cmd_line) {
    const auto unicode_cmd_line = widen(cmd_line.to_string());
    EscapedCommandLine buffer;
    buffer.reserve(unicode_cmd_line.size() + 1);
    buffer.assign(unicode_cmd_line.cbegin(), unicode_cmd_line.cend());
    buffer.emplace_back(L'\0');
    return buffer;
}

Handle create_process(const CommandLine& cmd_line, Process::IO& io) {
    return create_process(escape_command_line(cmd_line), io);
}

} // namespace

void Process::IO::close() {
    std_in.handle.close();
    std_out.handle.close();
    std_err.handle.close();
}

Process Process::create(const CommandLine& cmd_line) {
    return create(cmd_line, {});
}

Process Process::create(const CommandLine& cmd_line, IO io) {
    return Process{create_process(cmd_line, io)};
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

} // namespace winapi
