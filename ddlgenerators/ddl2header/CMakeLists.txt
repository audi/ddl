set(PROJECT_NAME ddl2header)

set(SOURCES
    main.cpp
    ddl2header_commandline.h
    ddl2header_commandline.cpp
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
