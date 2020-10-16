// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include <cstddef>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace winapi {

class Buffer : public std::vector<unsigned char> {
public:
    typedef std::vector<unsigned char> Parent;

    Buffer() = default;

    explicit Buffer(Parent&& src) : Parent(std::move(src)) {}

    template <typename CharT>
    explicit Buffer(const std::basic_string<CharT>& src) {
        set(src);
    }

    explicit Buffer(const void* src, std::size_t nb) { set(src, nb); }

    template <typename CharT>
    void set(const std::basic_string<CharT>& src) {
        set(src.c_str(), src.length() * sizeof(std::basic_string<CharT>::char_type));
    }

    void set(const void* src, std::size_t nb) {
        resize(nb);
        std::memcpy(data(), src, nb);
    }

    std::string as_utf8() const {
        const auto c_str = reinterpret_cast<const char*>(data());
        const auto nb = size();
        const auto nch = nb;
        return {c_str, nch};
    }

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

    void add(const Buffer& src) {
        const auto nb = size();
        resize(size() + src.size());
        std::memcpy(data() + nb, src.data(), src.size());
    }
};

} // namespace winapi
