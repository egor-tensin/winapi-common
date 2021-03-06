// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include <winapi/file.hpp>
#include <winapi/handle.hpp>
#include <winapi/path.hpp>
#include <winapi/process_io.hpp>

#include <string>
#include <utility>

namespace winapi {
namespace process {

Stdin::Stdin() : Stream{Handle::std_in()} {}

Stdout::Stdout() : Stream{Handle::std_out()} {}

Stderr::Stderr() : Stream{Handle::std_err()} {}

Stdin::Stdin(const std::string& path) : Stream{File::open_r(path)} {}

Stdin::Stdin(const CanonicalPath& path) : Stream{File::open_r(path)} {}

Stdout::Stdout(const std::string& path) : Stream{File::open_w(path)} {}

Stdout::Stdout(const CanonicalPath& path) : Stream{File::open_w(path)} {}

Stderr::Stderr(const std::string& path) : Stream{File::open_w(path)} {}

Stderr::Stderr(const CanonicalPath& path) : Stream{File::open_w(path)} {}

Stdin::Stdin(Pipe& pipe) : Stream{std::move(pipe.read_end())} {
    pipe.write_end().dont_inherit();
}

Stdout::Stdout(Pipe& pipe) : Stream{std::move(pipe.write_end())} {
    pipe.read_end().dont_inherit();
}

Stderr::Stderr(Pipe& pipe) : Stream{std::move(pipe.write_end())} {
    pipe.read_end().dont_inherit();
}

void IO::close() {
    std_in.handle.close();
    std_out.handle.close();
    std_err.handle.close();
}

} // namespace process
} // namespace winapi
