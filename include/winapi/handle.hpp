// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "buffer.hpp"

#include <windows.h>

#include <cstddef>
#include <memory>
#include <string_view>
#include <utility>

namespace winapi {

/**
 * @brief HANDLE wrapper.
 *
 * This class wraps HANDLE, allowing for painless reads and writes from a
 * random handle.
 */
class Handle {
public:
    Handle() = default;
    explicit Handle(HANDLE);

    HANDLE get() const {
        return m_impl.get();
    }

    HANDLE ptr() const {
        return get();
    }

    explicit operator HANDLE() const {
        return ptr();
    }

    bool is_valid() const;
    static bool is_valid(HANDLE);

    /** Close this handle. */
    void close();

    /** Check if this is a standard console handle. */
    bool is_std() const;
    /** Check if this is the stdin handle. */
    static Handle std_in();
    /** Check if this is the stdout handle. */
    static Handle std_out();
    /** Check if this is the stderr handle. */
    static Handle std_err();

    /** Read everything from this handle. */
    Buffer read() const;

    static constexpr std::size_t max_chunk_size = 16 * 1024;
    /**
     * Read a chunk from this handle.
     * @param read_chunk Receives the data read.
     * @return `true` if there's more data, `false` otherwise.
     */
    bool read_chunk(Buffer& read_chunk) const;

    /**
     * Write data to this handle.
     * @param data Pointer to binary data.
     * @param nb   Data size.
     */
    void write(const void* data, std::size_t nb) const;
    /**
     * Write data to this handle.
     * @param buffer Binary data to write.
     */
    void write(const Buffer& buffer) const;
    /**
     * Write data to this handle.
     * @param src Binary data to write.
     */
    template <typename CharT>
    void write(std::basic_string_view<CharT> src) const {
        write(src.data(), src.length() * sizeof(CharT));
    }
    /**
     * Write data to this handle.
     * @param src Binary data to write.
     */
    template <typename CharT,
              typename Traits = std::char_traits<CharT>,
              typename Allocator = std::allocator<CharT>>
    void write(const std::basic_string<CharT, Traits, Allocator>& src) const {
        write(std::basic_string_view<CharT>{src});
    }

    void inherit(bool yes = true) const;
    void dont_inherit() const {
        inherit(false);
    }

private:
    struct Close {
        void operator()(HANDLE) const;
    };

    std::unique_ptr<void, Close> m_impl;
};

} // namespace winapi
