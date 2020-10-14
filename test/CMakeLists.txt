add_executable(args args.cpp)
set_target_properties(args PROPERTIES OUTPUT_NAME winapi-common-test-args)

install(TARGETS args RUNTIME DESTINATION bin)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    install(FILES "$<TARGET_PDB_FILE:args>" DESTINATION bin OPTIONAL)
endif()

add_subdirectory(unit_tests)
