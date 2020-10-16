// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include <winapi/error.hpp>
#include <winapi/file.hpp>
#include <winapi/handle.hpp>
#include <winapi/utf8.hpp>

#include <cstring>
#include <string>

namespace winapi {
namespace {

struct CreateFileParams {
    static CreateFileParams read() {
        CreateFileParams params;
        params.dwDesiredAccess = GENERIC_READ;
        params.dwShareMode = FILE_SHARE_READ;
        params.dwCreationDisposition = OPEN_EXISTING;
        return params;
    }

    static CreateFileParams write() {
        CreateFileParams params;
        params.dwDesiredAccess = GENERIC_WRITE;
        params.dwShareMode = FILE_SHARE_READ;
        params.dwCreationDisposition = OPEN_ALWAYS;
        return params;
    }

    DWORD dwDesiredAccess = 0;
    DWORD dwShareMode = 0;
    DWORD dwCreationDisposition = 0;

private:
    CreateFileParams() = default;
};

Handle open_file(const std::string& path, const CreateFileParams& params) {
    const auto unicode_path = LR"(\\?\)" + widen(path);

    SECURITY_ATTRIBUTES attributes;
    std::memset(&attributes, 0, sizeof(attributes));
    attributes.nLength = sizeof(attributes);
    attributes.bInheritHandle = TRUE;

    const auto handle = ::CreateFileW(unicode_path.c_str(),
                                      params.dwDesiredAccess,
                                      params.dwShareMode,
                                      &attributes,
                                      params.dwCreationDisposition,
                                      FILE_ATTRIBUTE_NORMAL,
                                      NULL);

    if (handle == INVALID_HANDLE_VALUE) {
        throw error::windows(GetLastError(), "CreateFileW");
    }

    return Handle{handle};
}

} // namespace

Handle File::open_for_reading(const std::string& path) {
    return open_file(path, CreateFileParams::read());
}

Handle File::open_for_writing(const std::string& path) {
    return open_file(path, CreateFileParams::write());
}

} // namespace winapi
