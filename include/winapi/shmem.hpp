// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "handle.hpp"

#include <boost/config.hpp>

#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

namespace winapi {

class SharedMemory {
public:
    static SharedMemory create(const std::string& name, std::size_t nb);
    static SharedMemory open(const std::string& name);

    // VS 2013 won't generate these automatically:
    SharedMemory(SharedMemory&&) BOOST_NOEXCEPT_OR_NOTHROW;
    SharedMemory& operator=(SharedMemory) BOOST_NOEXCEPT_OR_NOTHROW;
    void swap(SharedMemory&) BOOST_NOEXCEPT_OR_NOTHROW;
    SharedMemory(const SharedMemory&) = delete;

    void* get() const { return m_addr.get(); }
    void* ptr() const { return get(); }

private:
    struct Unmap {
        void operator()(void*) const;
    };

    SharedMemory() = default;

    SharedMemory(Handle&& handle, void* addr) : m_handle{std::move(handle)}, m_addr{addr} {}

    Handle m_handle;
    std::unique_ptr<void, Unmap> m_addr;
};

inline void swap(SharedMemory& a, SharedMemory& b) BOOST_NOEXCEPT_OR_NOTHROW {
    a.swap(b);
}

template <typename T>
class SharedObject {
public:
    typedef typename std::aligned_storage<sizeof(T), __alignof(T)>::type AlignedType;

    template <typename... Args>
    static SharedObject create(const std::string& name, Args&&... args) {
        SharedObject obj{SharedMemory::create(name, sizeof(AlignedType))};
        new (obj.ptr()) T(std::forward<Args>(args)...);
        obj.m_destruct = true;
        return obj;
    }

    static SharedObject open(const std::string& name) {
        SharedObject obj{SharedMemory::open(name)};
        return obj;
    }

    SharedObject(SharedObject&& other) BOOST_NOEXCEPT_OR_NOTHROW
        : m_shmem{std::move(other.m_shmem)},
          m_destruct{other.m_destruct} {}

    SharedObject& operator=(SharedObject other) BOOST_NOEXCEPT_OR_NOTHROW {
        swap(other);
        return *this;
    }

    ~SharedObject() {
        if (m_destruct && ptr()) {
            ptr()->~T();
        }
    }

    void swap(SharedObject& other) BOOST_NOEXCEPT_OR_NOTHROW {
        using std::swap;
        swap(m_shmem, other.m_shmem);
        swap(m_destruct, other.m_destruct);
    }

    T* ptr() const { return reinterpret_cast<T*>(m_shmem.ptr()); }
    T& get() const { return *ptr(); }

    T* operator->() const { return ptr(); }
    T& operator*() const { return get(); }

private:
    explicit SharedObject(SharedMemory&& shmem) : m_shmem{std::move(shmem)} {}

    SharedMemory m_shmem;
    // Destruct only once, no matter the number of mappings.
    bool m_destruct = false;

    SharedObject(const SharedObject&) = delete;
};

template <typename T>
inline void swap(SharedObject<T>& a, SharedObject<T>& b) BOOST_NOEXCEPT_OR_NOTHROW {
    a.swap(b);
}

} // namespace winapi

namespace std {

template <>
inline void swap(winapi::SharedMemory& a, winapi::SharedMemory& b) BOOST_NOEXCEPT_OR_NOTHROW {
    a.swap(b);
}

template <typename T>
inline void swap(winapi::SharedObject<T>& a, winapi::SharedObject<T>& b) BOOST_NOEXCEPT_OR_NOTHROW {
    a.swap(b);
}

} // namespace std
