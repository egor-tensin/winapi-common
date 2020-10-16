// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "cmd_line.hpp"
#include "handle.hpp"
#include "stream.hpp"

#include <boost/config.hpp>

#include <utility>

namespace winapi {

class Process {
public:
    struct IO {
        IO() = default;

        void close();

        process::Stdin std_in;
        process::Stdout std_out;
        process::Stderr std_err;

        // VS 2013 won't generate these automatically.

        IO(IO&& other) BOOST_NOEXCEPT_OR_NOTHROW { swap(other); }

        IO& operator=(IO other) BOOST_NOEXCEPT_OR_NOTHROW {
            swap(other);
            return *this;
        }

        void swap(IO& other) BOOST_NOEXCEPT_OR_NOTHROW {
            using std::swap;
            swap(std_in, other.std_in);
            swap(std_out, other.std_out);
            swap(std_err, other.std_err);
        }

        IO(const IO&) = delete;
    };

    static Process create(const CommandLine&);
    static Process create(const CommandLine&, IO);

    void wait();

private:
    explicit Process(Handle&& handle) : m_handle{std::move(handle)} {}

    Handle m_handle;
};

inline void swap(Process::IO& a, Process::IO& b) BOOST_NOEXCEPT_OR_NOTHROW {
    a.swap(b);
}

} // namespace winapi

namespace std {

template <>
inline void swap(winapi::Process::IO& a, winapi::Process::IO& b) BOOST_NOEXCEPT_OR_NOTHROW {
    a.swap(b);
}

} // namespace std
