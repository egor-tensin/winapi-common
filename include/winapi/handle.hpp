// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "workarounds.hpp"

#include <boost/config.hpp>

#include <windows.h>

#include <cassert>
#include <memory>
#include <utility>

namespace winapi {

class Handle {
public:
    Handle() = default;

    explicit Handle(HANDLE raw)
        : impl{raw}
    { }

    Handle(Handle&& other) BOOST_NOEXCEPT_OR_NOTHROW {
        swap(other);
    }

    Handle& operator=(Handle other) BOOST_NOEXCEPT_OR_NOTHROW { 
        swap(other);
        return *this;
    }

    void swap(Handle& other) BOOST_NOEXCEPT_OR_NOTHROW {
        using std::swap;
        swap(impl, other.impl);
    }

    explicit operator HANDLE() const { return impl.get(); }

private:
    struct Close {
        void operator()(HANDLE raw) const {
            if (raw == NULL || raw == INVALID_HANDLE_VALUE)
                return;
            const auto ret = ::CloseHandle(raw);
            assert(ret);
            WINAPI_UNUSED_PARAMETER(ret);
        }
    };

    std::unique_ptr<void, Close> impl;

    Handle(const Handle&) = delete;
};

inline void swap(Handle& a, Handle& b) BOOST_NOEXCEPT_OR_NOTHROW {
    a.swap(b);
}

}

namespace std {

template <>
inline void swap(winapi::Handle& a, winapi::Handle& b) BOOST_NOEXCEPT_OR_NOTHROW {
    a.swap(b);
}

} // namespace std
