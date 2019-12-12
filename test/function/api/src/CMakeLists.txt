add_executable(tester_ddl_type_api test_fixture_ddl_types.h tester_ddl_types.cpp)
set_target_properties(tester_ddl_type_api PROPERTIES FOLDER test/ddl/api)
target_link_libraries(tester_ddl_type_api PRIVATE ddl_test_compat
                                                  ddl
                                                  GTest::Main)

add_test(tester_ddl_type_api tester_ddl_type_api)
set_target_properties(tester_ddl_type_api PROPERTIES TIMEOUT 10)