// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include <winapi/error.hpp>
#include <winapi/path.hpp>
#include <winapi/utf8.hpp>

#include <windows.h>

#include <string>
#include <vector>

namespace winapi {
namespace {

std::wstring do_canonicalize(const std::wstring& path) {
    BOOST_STATIC_CONSTEXPR std::size_t init_buffer_size = MAX_PATH;
    static_assert(init_buffer_size > 0, "init_buffer_size must be positive");

    std::vector<wchar_t> buffer;
    buffer.resize(init_buffer_size);

    while (true) {
        const auto nch = ::GetFullPathNameW(path.c_str(), buffer.size(), buffer.data(), NULL);

        if (nch == 0) {
            throw error::windows(GetLastError(), "GetFullPathNameW");
        }

        if (nch < buffer.size()) {
            return {buffer.data(), nch};
        }

        if (nch > buffer.size()) {
            buffer.resize(2 * buffer.size());
        }
    }
}

} // namespace

CanonicalPath::CanonicalPath(const std::string& path) : m_path(canonicalize(path)) {}

std::string CanonicalPath::canonicalize(const std::string& path) {
    return narrow(do_canonicalize(widen(path)));
}

} // namespace winapi
