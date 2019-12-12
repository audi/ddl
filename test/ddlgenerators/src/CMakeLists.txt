set(TEST_NAME ddl_generators)

add_executable(tester_${TEST_NAME} tester_${TEST_NAME}.cpp)
if (NOT ddl_cmake_integrated_tests)
    target_compile_definitions(tester_${TEST_NAME} PRIVATE TEST_INSTALL_DIR="${ddl_DIR}/..")
else()
    target_compile_definitions(tester_${TEST_NAME}
                               PRIVATE TEST_INSTALL_DIR="${CMAKE_INSTALL_PREFIX}")
endif()

set_target_properties(tester_${TEST_NAME} PROPERTIES FOLDER test/utils)  
set_target_properties(tester_${TEST_NAME}  PROPERTIES TIMEOUT 120)

target_link_libraries(tester_${TEST_NAME} PRIVATE
    ddl_test_compat
    ddl
    a_util
    GTest::Main
    $<$<PLATFORM_ID:Linux>:Threads::Threads>
)
add_test(NAME tester_${TEST_NAME} 
         COMMAND tester_${TEST_NAME}
         WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../")
