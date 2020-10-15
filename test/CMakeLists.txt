add_executable(echo echo.cpp)
set_target_properties(echo PROPERTIES OUTPUT_NAME winapi-common-test-echo)

install(TARGETS echo RUNTIME DESTINATION bin)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    install(FILES "$<TARGET_PDB_FILE:echo>" DESTINATION bin OPTIONAL)
endif()

add_subdirectory(unit_tests)
