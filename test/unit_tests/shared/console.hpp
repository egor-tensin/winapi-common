// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include <winapi/error.hpp>
#include <winapi/handle.hpp>
#include <winapi/utf8.hpp>

#include <boost/algorithm/string.hpp>

#include <windows.h>

#include <cstddef>
#include <cstring>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace console {

class Buffer {
public:
    typedef CONSOLE_SCREEN_BUFFER_INFO Info;

    Buffer() : m_handle{winapi::Handle::std_out()}, m_info{get_info(m_handle)} {}

    Buffer(winapi::Handle&& handle) : m_handle{std::move(handle)}, m_info{get_info(m_handle)} {}

    std::size_t get_columns() const { return m_info.dwSize.X; }

    std::size_t get_lines() const { return m_info.dwSize.Y; }

    std::size_t get_cursor_column() const { return m_info.dwCursorPosition.X; }

    std::size_t get_cursor_line() const { return m_info.dwCursorPosition.Y; }

    void update() { m_info = get_info(m_handle); }

    /*
     * This is a stupid little function to read the console screen buffer.
     * It's fragile and will break whenever anything happens (like, if the
     * console screen is resized).
     *
     * The screen buffer:
     * 1) doesn't preserve line breaks,
     * 2) pads text lines with spaces (ASCII 0x20) for storage.
     *
     * Hence, the "lines" read are one-dimensional arrays, right-trimmed, and
     * there's no way to learn whether the whole line printed by the user was
     * read back in its entirety.
     *
     * For example, let the console window be 80 columns wide, and the user
     * prints 85 consecutive characters 'a' using
     *
     *     std::cout << std::string(85, 'a') << '\n';
     *
     * The following holds:
     * 1) read_lines(-1, -1) == {"aaaaa"},
     * 2) read_lines(-2, -2) == {std::string(80, 'a'), "aaaaa"}.
     *
     * I also don't know how it interacts with tab characters '\t', encodings,
     * etc. It sucks, don't use it.
     */
    std::vector<std::string> read_lines(int top, int bottom) const {
        if (top < 0) {
            top = get_cursor_line() + top;
        }
        if (bottom < 0) {
            bottom = get_cursor_line() + bottom;
        }
        if (top > bottom) {
            std::swap(top, bottom);
        }
        int numof_lines = bottom - top + 1;

        COORD buffer_size;
        buffer_size.X = get_columns();
        buffer_size.Y = numof_lines;

        COORD buffer_coord;
        buffer_coord.X = 0;
        buffer_coord.Y = 0;

        std::vector<CHAR_INFO> buffer;
        buffer.resize(buffer_size.X * buffer_size.Y);

        SMALL_RECT read_region;
        read_region.Top = top;
        read_region.Left = 0;
        read_region.Bottom = bottom;
        read_region.Right = buffer_size.X - 1;

        if (!::ReadConsoleOutputW(
                m_handle.get(), buffer.data(), buffer_size, buffer_coord, &read_region)) {
            throw winapi::error::windows(GetLastError(), "ReadConsoleOutputW");
        }

        std::vector<std::string> result;
        for (std::size_t i = 0; i < numof_lines; ++i) {
            std::wostringstream oss;
            for (std::size_t c = 0; c < buffer_size.X; ++c) {
                oss << buffer[i * buffer_size.X + c].Char.UnicodeChar;
            }
            result.emplace_back(boost::trim_right_copy(winapi::narrow(oss.str())));
        }

        return result;
    }

    std::vector<std::string> read_last_lines(int numof_lines = 1) const {
        return read_lines(-numof_lines, -1);
    }

    std::string read_last_line() const { return read_lines(-1, -1)[0]; }

    std::string read_line(int n) const { return read_lines(n, n)[0]; }

private:
    static Info get_info(const winapi::Handle& handle) {
        Info dest;
        std::memset(&dest, 0, sizeof(dest));

        if (!::GetConsoleScreenBufferInfo(static_cast<HANDLE>(handle), &dest)) {
            throw winapi::error::windows(GetLastError(), "GetConsoleScreenBufferInfo");
        }

        return dest;
    }

    winapi::Handle m_handle;
    Info m_info;
};

} // namespace console
