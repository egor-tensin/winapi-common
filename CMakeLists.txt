cmake_minimum_required(VERSION 3.5) # for Boost::* imported targets

project(winapi_common CXX)

include(cmake/common.cmake)

file(GLOB_RECURSE winapi_common_include "include/*.hpp")
file(GLOB winapi_common_src "src/*.cpp")
add_library(winapi_common ${winapi_common_include} ${winapi_common_src})
target_include_directories(winapi_common PUBLIC include/)

add_subdirectory(3rdparty/winapi/utf8 EXCLUDE_FROM_ALL)
target_link_libraries(winapi_common PUBLIC winapi_utf8)

find_package(Boost REQUIRED)
target_link_libraries(winapi_common PUBLIC Boost::boost)

install(TARGETS winapi_common ARCHIVE DESTINATION lib)
install(DIRECTORY include/winapi DESTINATION include)

if(WINAPI_COMMON_TESTS)
    add_subdirectory(test)
endif()

install(FILES LICENSE.txt DESTINATION share)
