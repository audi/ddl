set(PROJECT_NAME header2ddl)

set(SOURCES
    main.cpp
    header2ddl_commandline.h
    header2ddl_commandline.cpp
    ${DDL_GENERATOR_COMMON}
    ${HEADER_PRESENTATION_H}
    ${HEADER_PRESENTATION_CPP}
)

if (WIN32)
    #list(APPEND SOURCES native_resource.rc)
endif (WIN32)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

target_link_libraries(${PROJECT_NAME}
   ddl
   ddl_generator
)

if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:console")
endif(MSVC)

install(TARGETS ${PROJECT_NAME} DESTINATION ./bin/debug CONFIGURATIONS Debug)
install(TARGETS ${PROJECT_NAME} DESTINATION ./bin CONFIGURATIONS Release RelWithDebInfo)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ddl/utils)
