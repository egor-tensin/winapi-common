// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "buffer.hpp"

#include <windows.h>

#include <cstddef>
#include <memory>
#include <string>
#include <utility>

namespace winapi {

class Handle {
public:
    Handle() = default;
    explicit Handle(HANDLE);

    HANDLE get() const { return m_impl.get(); }
    HANDLE ptr() const { return get(); }

    explicit operator HANDLE() const { return ptr(); }

    bool is_valid() const;
    static bool is_valid(HANDLE);

    void close();

    bool is_std() const;
    static Handle std_in();
    static Handle std_out();
    static Handle std_err();

    Buffer read() const;

    static constexpr std::size_t max_chunk_size = 16 * 1024;
    bool read_chunk(Buffer& read_chunk) const;

    void write(const void*, std::size_t nb) const;
    void write(const Buffer& buffer) const;

    template <typename CharT>
    void write(const std::basic_string<CharT>& src) const {
        write(src.c_str(), src.size() * sizeof(CharT));
    }

    void inherit(bool yes = true) const;
    void dont_inherit() const { inherit(false); }

private:
    struct Close {
        void operator()(HANDLE) const;
    };

    std::unique_ptr<void, Close> m_impl;
};

} // namespace winapi
