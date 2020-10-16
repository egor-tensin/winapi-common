// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include <winapi/error.hpp>
#include <winapi/file.hpp>
#include <winapi/handle.hpp>
#include <winapi/path.hpp>
#include <winapi/utf8.hpp>

#include <cstring>
#include <string>

namespace winapi {
namespace {

std::wstring to_system_path(const std::string& path) {
    return widen(path);
}

std::wstring to_system_path(const CanonicalPath& path) {
    return widen(R"(\\?\)" + path.get());
}

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

Handle open_file(const std::wstring& path, const CreateFileParams& params) {
    SECURITY_ATTRIBUTES attributes;
    std::memset(&attributes, 0, sizeof(attributes));
    attributes.nLength = sizeof(attributes);
    attributes.bInheritHandle = TRUE;

    const auto handle = ::CreateFileW(path.c_str(),
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

void remove_file(const std::wstring& path) {
    const auto ret = ::DeleteFileW(path.c_str());

    if (!ret) {
        throw error::windows(GetLastError(), "DeleteFileW");
    }
}

} // namespace

Handle File::open_r(const std::string& path) {
    return open_file(to_system_path(path), CreateFileParams::read());
}

Handle File::open_r(const CanonicalPath& path) {
    return open_file(to_system_path(path), CreateFileParams::read());
}

Handle File::open_w(const std::string& path) {
    return open_file(to_system_path(path), CreateFileParams::write());
}

Handle File::open_w(const CanonicalPath& path) {
    return open_file(to_system_path(path), CreateFileParams::write());
}

void File::remove(const std::string& path) {
    remove_file(to_system_path(path));
}

void File::remove(const CanonicalPath& path) {
    remove_file(to_system_path(path));
}

} // namespace winapi
