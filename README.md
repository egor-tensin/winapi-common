winapi-common
=============

[![CI](https://github.com/egor-tensin/winapi-common/actions/workflows/ci.yml/badge.svg)](https://github.com/egor-tensin/winapi-common/actions/workflows/ci.yml)

C++ wrappers for various common WinAPI entities I've had to use in various
projects over the years.

Development
-----------

Build using CMake.
Depends on Boost.Test and Python 3 for running the tests.
The project is Windows-only, so building with either MSVC or MinGW-w64 is
required.

There's a Makefile with useful shortcuts to build the project in the .build/
directory (defaults to building with MinGW-w64):

    make deps
    make build
    make test

Documentation
-------------

Build & display the documentation using

    make docs

License
-------

Distributed under the MIT License.
See [LICENSE.txt] for details.

[LICENSE.txt]: LICENSE.txt
