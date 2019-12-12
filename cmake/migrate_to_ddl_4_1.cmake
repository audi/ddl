function(ddl_migrate_4_1)
    set(options "")
    set(oneValueArgs TARGET)
    set(multiValueArgs "")
    cmake_parse_arguments(CLANG_TIDY "${options}" "${oneValueArgs}"
                          "${multiValueArgs}" ${ARGN} )

    get_target_property(inc_dirs ${CLANG_TIDY_TARGET} INCLUDE_DIRECTORIES)
    get_target_property(target_sources ${CLANG_TIDY_TARGET} SOURCES)
    get_target_property(target_folder ${CLANG_TIDY_TARGET} FOLDER)
    get_target_property(link_libs ${CLANG_TIDY_TARGET} LINK_LIBRARIES)
    
    foreach(lib ${link_libs})
        get_target_property(lib_inc_dirs ${lib} INTERFACE_INCLUDE_DIRECTORIES) 
        if(lib_inc_dirs)
            list(APPEND inc_dirs ${lib_inc_dirs})
        endif()
    endforeach(lib)
    
    foreach(inc ${inc_dirs})
        list(APPEND inc_list -isystem${inc})
    endforeach(inc)
          
    # filter headers that are included in sources
    list(FILTER target_sources INCLUDE REGEX ".*\.cpp$")

    set(TIDY_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/run-clang-tidy-fix-${CLANG_TIDY_TARGET}.sh)

    string (REPLACE ";" " " target_sources_spaces "${target_sources}")
    string (REPLACE ";" " " inc_list_spaces "${inc_list}")

    string (CONCAT TIDY_STRING 
        "${CONAN_BIN_DIRS_CLANG}/clang-tidy "
        " ${target_sources_spaces} "
        " -header-filter=.* "
        " -fix "
        " -fix-errors "
        " -- "
        " ${inc_list_spaces} "
        " -std=c++14 "
        " ${CMAKE_CXX_FLAGS} ")

    file(GENERATE OUTPUT ${TIDY_SCRIPT} CONTENT "${TIDY_STRING}")


    set(REPLACEMENT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    set(REPLACEMENTS_FILE ${ddl_INCLUDE_DIRS}/../cmake/ddl4.1_replacements.txt)
    configure_file(${ddl_INCLUDE_DIRS}/../cmake/migrate_to_ddl_4_1.sh.in ${CMAKE_CURRENT_BINARY_DIR}/${CLANG_TIDY_TARGET}_migrate_to_ddl_4_1.sh @ONLY)

endfunction(ddl_migrate_4_1)
