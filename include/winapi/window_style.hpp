// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

namespace winapi {

enum class WindowStyle {
    // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showwindow
    ForceMinimize = 11,
    Hide = 0,
    Maximize = 3,
    Minimize = 6,
    Restore = 9,
    Show = 5,
    ShowDefault = 10,
    ShowMaximized = 3,
    ShowMinimized = 2,
    ShowMinNoActive = 7,
    ShowNA = 8,
    ShowNoActivate = 4,
    ShowNormal = 1,
};

} // namespace winapi
