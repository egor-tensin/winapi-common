// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include <string>

namespace winapi {

class CanonicalPath {
public:
    static std::string canonicalize(const std::string&);

    explicit CanonicalPath(const std::string&);

    std::string get() const { return m_path; }
    std::string path() const { return get(); }

private:
    std::string m_path;
};

} // namespace winapi
