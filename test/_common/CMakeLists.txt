# Add compat header files as projetc files when using with target_link_libraries(MyProject compat)
add_library(ddl_test_compat INTERFACE)
target_sources(ddl_test_compat INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/adtf_compat.h
                                         ${CMAKE_CURRENT_SOURCE_DIR}/compat.h)