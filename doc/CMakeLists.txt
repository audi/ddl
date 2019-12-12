option(ddl_cmake_enable_documentation "If enabled, generate the source code documentation -\
 requires doxygen (default: ON)" ON)
option(ddl_cmake_enable_post_install_doc_generation "If enabled, doxygen will generate the\
 documentation as post-install event. Disable only for development purposes. (default: ON)" ON)
option(DOXYGEN_SKIP_DOT "If true this module will skip trying to find Dot (default: OFF)" OFF)

install(DIRECTORY input DESTINATION doc/
        FILES_MATCHING PATTERN "*.md"
        PATTERN "license" EXCLUDE )
install(FILES input/mapping_configuration.xsd DESTINATION doc/)
install(FILES input/ddl3.xsd DESTINATION doc/)
install(FILES input/ddl4.xsd DESTINATION doc/)
install(FILES changelog.md DESTINATION doc/)

set (AEV_PRODUCT_FULL_NAME "ddl_library")

if(NOT EXCLUDE_PRODUCT_LICENSES)
    set(LICENSE_FILES_EXTENSION "* [mpl.md](../license/mpl.md#)\n\
* [MPL2.0.txt](../license/MPL2.0.txt)\n\
* [used_licenses.md](../license/used_licenses.md#)\n")
endif()

##continue only if enabled
if(ddl_cmake_enable_documentation)
    ## Doxygen is not required. But only built html Doku if doxygen is found
    ## Otherwise just the Markdown docu will be copied.
    find_package(Doxygen)
    if(NOT DOXYGEN_FOUND)
        message(STATUS "No doxygen executable found.")
        return()
    endif(NOT DOXYGEN_FOUND)
    if(NOT DOXYGEN_SKIP_DOT AND NOT DOXYGEN_DOT_FOUND)
        message(FATAL_ERROR "No dot executable found. Either set the "
                            "correct DOXYGEN_DOT_EXECUTABLE or enable DOXYGEN_SKIP_DOT.")
    elseif(NOT DOXYGEN_SKIP_DOT AND DOXYGEN_DOT_FOUND)
        EXECUTE_PROCESS(COMMAND ${DOXYGEN_DOT_EXECUTABLE} -V OUTPUT_VARIABLE dot_version_info
                                                             ERROR_VARIABLE dot_version_info
                                                             OUTPUT_STRIP_TRAILING_WHITESPACE
                                                             ERROR_STRIP_TRAILING_WHITESPACE)
        set(dot_version_info "(found version: \"${dot_version_info}\")")
        message(STATUS "Found dot: ${DOXYGEN_DOT_EXECUTABLE} ${dot_version_info}")
        unset(dot_version_info)
    endif(NOT DOXYGEN_SKIP_DOT AND NOT DOXYGEN_DOT_FOUND)
else()
    return()
endif(ddl_cmake_enable_documentation)

configure_file(run_doxygen.cmake.in run_doxygen.cmake @ONLY)
##create seperate target excluded from all
add_custom_target(ddl_DOC ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/run_doxygen.cmake)
set_target_properties(ddl_DOC PROPERTIES FOLDER process)

##post install script
if(ddl_cmake_enable_post_install_doc_generation)
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/run_doxygen.cmake)
    install(FILES static/ddl.html DESTINATION doc/)
endif(ddl_cmake_enable_post_install_doc_generation)