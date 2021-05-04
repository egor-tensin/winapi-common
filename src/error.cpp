// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include <winapi/error.hpp>
#include <winapi/utf8.hpp>

#include <windows.h>

#include <cstdint>
#include <sstream>
#include <string>
#include <system_error>

namespace winapi {
namespace error {
namespace {

std::wstring trim_trailing_newline(const std::wstring& s) {
    const auto last_pos = s.find_last_not_of(L"\r\n");
    if (std::wstring::npos == last_pos)
        return {};
    return s.substr(0, last_pos + 1);
}

std::string build_what(DWORD code, const char* function) {
    std::ostringstream what;
    what << "Function " << function << " failed with error code " << code;
    return what.str();
}

std::string format_message(int32_t code) {
    wchar_t* buf;

    const auto len = ::FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<wchar_t*>(&buf),
        0,
        NULL);

    if (0 == len) {
        ::LocalFree(buf);
        return "Couldn't format the error message";
    }

    std::wstring msg{buf, len};
    ::LocalFree(buf);
    return narrow(trim_trailing_newline(msg));
}

} // namespace

std::string CategoryWindows::message(int32_t code) const {
    return format_message(code);
}

std::system_error windows(DWORD code, const char* function) {
    static_assert(sizeof(DWORD) == sizeof(int32_t), "Aren't DWORDs the same size as ints?");
    return std::system_error{
        static_cast<int32_t>(code), category_windows(), build_what(code, function)};
}

} // namespace error
} // namespace winapi
