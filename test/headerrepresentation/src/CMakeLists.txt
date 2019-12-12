# to use generator lib we add sources to test because tester doesn't have this 
# internal lib for linking
include(../../../ddlgenerators/generator_library/ddl_generator_lib.sources)
include_directories(../../../ddlgenerators/generator_library/headerrepresentation)

set(TEST_NAME headerrepresentation)

add_executable(tester_${TEST_NAME} tester_${TEST_NAME}.cpp
                                   ${HEADER_PRESENTATION_H}
                                   ${HEADER_PRESENTATION_CPP}
                                   ddl_definitions.h)

set_target_properties(tester_${TEST_NAME} PROPERTIES FOLDER test/ddl)  
set_target_properties(tester_${TEST_NAME}  PROPERTIES TIMEOUT 120)

target_link_libraries(tester_${TEST_NAME} PRIVATE
    ddl_test_compat
    ddl
    a_util_result
    GTest::Main
    $<$<PLATFORM_ID:Linux>:Threads::Threads>
)
add_test(NAME tester_${TEST_NAME} 
         COMMAND tester_${TEST_NAME}
         WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../")
