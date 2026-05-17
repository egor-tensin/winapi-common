# oldstable as of May '26
cmake_minimum_required(VERSION 3.25)

project(winapi_common CXX)
enable_testing()

if (MSVC)
    add_compile_options(/MP /W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX)

find_package(Boost REQUIRED)
add_subdirectory(3rdparty/winapi/utf8 EXCLUDE_FROM_ALL)

file(GLOB_RECURSE winapi_common_include CONFIGURE_DEPENDS "include/*.hpp")
file(GLOB winapi_common_src CONFIGURE_DEPENDS "src/*.cpp")
add_library(winapi_common ${winapi_common_include} ${winapi_common_src})
target_include_directories(winapi_common PUBLIC include)
target_link_libraries(winapi_common PRIVATE winapi_utf8)
target_link_libraries(winapi_common PUBLIC Boost::disable_autolinking Boost::boost)
install(TARGETS winapi_common ARCHIVE DESTINATION lib)
install(DIRECTORY include/winapi DESTINATION include)

if(MINGW)
    # FILE_ID_INFO and friends require at least 0x0602:
    target_compile_definitions(winapi_common PUBLIC
        NTDDI_VERSION=NTDDI_WIN8
        _WIN32_WINNT=_WIN32_WINNT_WIN8)
endif()

if(WINAPI_COMMON_TESTS)
    add_subdirectory(test)
endif()

install(FILES LICENSE.txt DESTINATION share)

find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_INCLUDE_GRAPH NO)
    set(DOXYGEN_INCLUDED_BY_GRAPH NO)
    set(DOXYGEN_SHOW_INCLUDE_FILES NO)
    set(DOXYGEN_SORT_MEMBER_DOCS NO)
    set(DOXYGEN_SOURCE_BROWSER YES)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)
    doxygen_add_docs(winapi_common_docs
        README.md
        ${winapi_common_include}
        ${winapi_common_src})
endif()
