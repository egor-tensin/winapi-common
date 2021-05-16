// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "handle.hpp"

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

    SharedObject(SharedObject&& other) noexcept = default;
    SharedObject& operator=(const SharedObject& other) noexcept = default;
    SharedObject(const SharedObject&) = delete;

    ~SharedObject() {
        if (m_destruct && ptr()) {
            ptr()->~T();
        }
    }

    T* ptr() const { return reinterpret_cast<T*>(m_shmem.ptr()); }
    T& get() const { return *ptr(); }

    T* operator->() const { return ptr(); }
    T& operator*() const { return get(); }

private:
    explicit SharedObject(SharedMemory&& shmem) : m_shmem(std::move(shmem)) {}

    SharedMemory m_shmem;
    // Destruct only once, no matter the number of mappings.
    bool m_destruct = false;
};

} // namespace winapi
