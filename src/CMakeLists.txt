find_package(Boost CONFIG REQUIRED)

file(GLOB_RECURSE winapi_common_include CONFIGURE_DEPENDS "../include/*.hpp")
file(GLOB winapi_common_src CONFIGURE_DEPENDS "*.cpp")
add_library(winapi_common ${winapi_common_include} ${winapi_common_src})
target_include_directories(winapi_common PUBLIC ../include)
target_link_libraries(winapi_common PRIVATE winapi_utf8)
target_link_libraries(winapi_common PUBLIC
    Boost::disable_autolinking
    Boost::boost
)
install(TARGETS winapi_common ARCHIVE DESTINATION lib)
install(DIRECTORY ../include/winapi DESTINATION include)

if(MINGW)
    # FILE_ID_INFO and friends require at least 0x0602:
    target_compile_definitions(winapi_common PUBLIC
        NTDDI_VERSION=NTDDI_WIN8
        _WIN32_WINNT=_WIN32_WINNT_WIN8)
endif()
