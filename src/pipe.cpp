// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include <winapi/error.hpp>
#include <winapi/handle.hpp>
#include <winapi/pipe.hpp>

#include <windows.h>

#include <cstring>
#include <utility>

namespace winapi {
namespace {

void create_pipe(Handle& read_end, Handle& write_end) {
    HANDLE read_end_impl = INVALID_HANDLE_VALUE;
    HANDLE write_end_impl = INVALID_HANDLE_VALUE;

    SECURITY_ATTRIBUTES attributes;
    std::memset(&attributes, 0, sizeof(attributes));
    attributes.nLength = sizeof(attributes);
    attributes.bInheritHandle = TRUE;

    static constexpr DWORD buffer_size = 16 * 1024;

    const auto ret = ::CreatePipe(&read_end_impl, &write_end_impl, &attributes, buffer_size);

    if (!ret) {
        throw error::windows(GetLastError(), "CreatePipe");
    }

    read_end = Handle{read_end_impl};
    write_end = Handle{write_end_impl};
}

} // namespace

Pipe::Pipe() {
    create_pipe(m_read_end, m_write_end);
}

} // namespace winapi
