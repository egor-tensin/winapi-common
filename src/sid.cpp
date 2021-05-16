// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include <winapi/buffer.hpp>
#include <winapi/error.hpp>
#include <winapi/sid.hpp>
#include <winapi/utf8.hpp>
#include <winapi/utils.hpp>

// clang-format off
#include <windows.h>
#include <sddl.h>
// clang-format on

#include <memory>
#include <string>

namespace winapi {

Sid Sid::well_known(WELL_KNOWN_SID_TYPE type) {
    Buffer buffer;
    buffer.resize(MAX_SID_SIZE);

    auto cb = static_cast<DWORD>(buffer.size());

    if (!::CreateWellKnownSid(type, NULL, buffer.data(), &cb))
        throw error::windows(GetLastError(), "CreateWellKnownSid");

    buffer.resize(cb);
    return Sid{buffer};
}

Sid Sid::builtin_administrators() {
    return well_known(WinBuiltinAdministratorsSid);
}

std::string Sid::to_string() const {
    wchar_t* s = nullptr;

    if (!::ConvertSidToStringSidW(const_cast<Impl*>(&get_impl()), &s))
        throw error::windows(GetLastError(), "ConvertSidToStringSidW");

    return narrow(std::unique_ptr<wchar_t, LocalDelete>{s}.get());
}

} // namespace winapi
