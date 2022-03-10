// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace winapi {

/**
 * @brief Binary data container.
 *
 * This class wraps a blob of binary data.
 */
class Buffer : public std::vector<unsigned char> {
public:
    typedef std::vector<unsigned char> Parent;

    Buffer() = default;

    /** Construct a buffer from an explicit list of byte values. */
    Buffer(std::initializer_list<unsigned char> lst) : Parent{lst} {}

    /** Construct a buffer from an instance of `std::vector<unsigned char>`. */
    explicit Buffer(Parent&& src) : Parent{std::move(src)} {}

    /** Construct a buffer from an instance of `std::basic_string`. */
    template <typename CharT>
    explicit Buffer(const std::basic_string<CharT>& src) {
        set(src);
    }

    /** Construct a buffer from a memory region. */
    Buffer(const void* src, std::size_t nb) { set(src, nb); }

    /** Replace the buffer's contents with the data from `std::basic_string`. */
    template <typename CharT>
    void set(const std::basic_string<CharT>& src) {
        set(src.c_str(), src.length() * sizeof(std::basic_string<CharT>::char_type));
    }

    /** Replace the buffer's contents with the data from a memory region. */
    void set(const void* src, std::size_t nb) {
        resize(nb);
        std::memcpy(data(), src, nb);
    }

    /** Interpret the buffer's contents as a `std::string`. */
    std::string as_utf8() const {
        const auto c_str = reinterpret_cast<const char*>(data());
        const auto nb = size();
        const auto nch = nb;
        return {c_str, nch};
    }

    /** Interpret the buffer's contents as a `std::wstring`. */
    std::wstring as_utf16() const {
        const auto c_str = reinterpret_cast<const wchar_t*>(data());
        const auto nb = size();
        if (nb % 2 != 0) {
            std::ostringstream oss;
            oss << "Buffer size invalid at " << nb << " bytes";
            throw std::runtime_error{oss.str()};
        }
        const auto nch = nb / 2;
        return {c_str, nch};
    }

    /** Append another buffer to the end of this one. */
    void add(const Buffer& src) {
        const auto nb = size();
        resize(nb + src.size());
        std::memcpy(data() + nb, src.data(), src.size());
    }
};

} // namespace winapi
