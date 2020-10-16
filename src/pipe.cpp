// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include <winapi/error.hpp>
#include <winapi/handle.hpp>
#include <winapi/pipe.hpp>

#include <boost/config.hpp>

#include <windows.h>

#include <cstring>

namespace winapi {
namespace {

void create_pipe(Handle& read_end, Handle& write_end) {
    HANDLE h_read_end = INVALID_HANDLE_VALUE;
    HANDLE h_write_end = INVALID_HANDLE_VALUE;

    SECURITY_ATTRIBUTES attributes;
    std::memset(&attributes, 0, sizeof(attributes));
    attributes.nLength = sizeof(attributes);
    attributes.bInheritHandle = TRUE;

    BOOST_STATIC_CONSTEXPR DWORD buffer_size = 16 * 1024;

    const auto ret = ::CreatePipe(&h_read_end, &h_write_end, &attributes, buffer_size);

    if (!ret) {
        throw error::windows(GetLastError(), "CreatePipe");
    }

    read_end = Handle{h_read_end};
    write_end = Handle{h_write_end};
}

} // namespace

Pipe::Pipe() {
    create_pipe(m_read_end, m_write_end);
}

} // namespace winapi
