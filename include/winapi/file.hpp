// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "handle.hpp"
#include "path.hpp"

#include <boost/functional/hash.hpp>

#include <windows.h>

#include <cstddef>
#include <functional>
#include <string>
#include <utility>

namespace winapi {

bool operator==(const FILE_ID_128& a, const FILE_ID_128& b);

class File : public Handle {
public:
    struct ID {
        const FILE_ID_INFO impl;

        bool operator==(const ID& other) const {
            return impl.VolumeSerialNumber == other.impl.VolumeSerialNumber &&
                   impl.FileId == other.impl.FileId;
        }
    };

    static File open_r(const std::string&);
    static File open_r(const CanonicalPath&);
    static File open_read_attributes(const std::string&);
    static File open_read_attributes(const CanonicalPath&);
    static File open_w(const std::string&);
    static File open_w(const CanonicalPath&);

    static void remove(const std::string&);
    static void remove(const CanonicalPath&);

    explicit File(Handle&& handle) : Handle(std::move(handle)) {}

    std::size_t get_size() const;

    ID query_id() const;
};

} // namespace winapi

namespace std {

template <>
struct hash<winapi::File::ID> {
    std::size_t operator()(const winapi::File::ID& id) const {
        std::size_t seed = 0;
        boost::hash_combine(seed, id.impl.VolumeSerialNumber);
        boost::hash_combine(seed, id.impl.FileId.Identifier);
        return seed;
    }
};

} // namespace std
