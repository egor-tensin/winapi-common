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
#include <utility>
#include <vector>

namespace winapi {
namespace {

typedef std::vector<wchar_t> EscapedCommandLine;

Handle create_process(EscapedCommandLine cmd_line) {
    BOOST_STATIC_CONSTEXPR DWORD flags = CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT;

    STARTUPINFOW startup_info;
    std::memset(&startup_info, 0, sizeof(startup_info));
    startup_info.cb = sizeof(startup_info);

    PROCESS_INFORMATION child_info;
    std::memset(&child_info, 0, sizeof(child_info));

    const auto ret = ::CreateProcessW(
        NULL, cmd_line.data(), NULL, NULL, FALSE, flags, NULL, NULL, &startup_info, &child_info);

    if (!ret) {
        throw error::windows(GetLastError(), "CreateProcessW");
    }

    Handle h_process{child_info.hProcess};
    Handle h_thread{child_info.hThread};

    return std::move(h_process);
}

EscapedCommandLine escape_command_line(const CommandLine& cmd_line) {
    const auto whole = widen(cmd_line.join());
    return {whole.cbegin(), whole.cend()};
}

Handle create_process(const CommandLine& cmd_line) {
    return create_process(escape_command_line(cmd_line));
}

} // namespace

Process Process::create(const CommandLine& cmd_line) {
    return Process{create_process(cmd_line)};
}

void Process::wait() {
    const auto ret = ::WaitForSingleObject(static_cast<HANDLE>(m_handle), INFINITE);

    switch (ret) {
        case WAIT_OBJECT_0:
            m_handle = Handle{};
            return;
        case WAIT_FAILED:
            throw error::windows(GetLastError(), "WaitForSingleObject");
        default:
            // Shouldn't happen.
            throw error::custom(ret, "WaitForSingleObject");
    }
}

} // namespace winapi
