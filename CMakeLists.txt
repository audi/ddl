cmake_minimum_required(VERSION 3.8.2) # cxx_std_11
project(ddl_library VERSION 4.4.0)

# Disable extensions here and require the chosen CMAKE_CXX_STANDARD (coming from e.g. Conan)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

# if Conan is used, we have to include the generated file and doing some basic setup
if(CONAN_COMPILER)
    if ( EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
        include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
    elseif ( EXISTS ${CMAKE_CURRENT_BINARY_DIR}/../conanbuildinfo.cmake)
        include(${CMAKE_CURRENT_BINARY_DIR}/../conanbuildinfo.cmake)
    elseif ( EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo_multi.cmake)
        include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo_multi.cmake)
    elseif ( EXISTS ${CMAKE_CURRENT_BINARY_DIR}/../conanbuildinfo_multi.cmake)
        include(${CMAKE_CURRENT_BINARY_DIR}/../conanbuildinfo_multi.cmake)
    else()
        message(FATAL_ERROR "Conan build info can't be found.")
    endif()

    if(CORTEX_WORKSPACE)
        conan_basic_setup(TARGETS)
    else()
        conan_basic_setup(TARGETS NO_OUTPUT_DIRS)
    endif()
endif()

# Options #################################################
option(ddl_cmake_enable_tests
       "Enable tests - requires googletest (default: OFF)"
       OFF)
       
option(ddl_cmake_enable_installation
       "Enable installation of the ddl library to CMAKE_INSTALL_PREFIX (default: ON)"
       ON)
       
option(ddl_cmake_enable_ddl_generator_tools
       "Enable building of the two tools ddl2header and header2ddl (default: ON)"
       ON)
# End Options #############################################

# Check for default install prefix and cmake build type (UNIX)
include(./cmake/check_cmake_install_prefix.cmake)
include(./cmake/check_cmake_build_type.cmake)

# Enable multicore compilation on Windows
include(./cmake/enable_multicore_compilation.cmake)

# Use integrated debug symbols on Windows (avoiding PDBs)
include(./cmake/use_integrated_debug_symbols.cmake)

# Set library name conventions
include(./cmake/set_library_naming_conventions.cmake)

# Enable folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Add a_util dependency
find_package(a_util 5.4 REQUIRED)

# Include sources
include(./ddlrepresentation/pkg_ddlrepresentation.sources)
include(./codec/codec.sources)
include(./serialization/serialization.sources)
include(./mapping/mapping.sources)

add_library(ddl STATIC
    ${DDLREPRESENTATION_CPP}
    ${CODEC_CPP}
    ${SERIALIZATION_CPP}
    ${MAPPING_CPP}

    ${DDLREPRESENTATION_H}
    ${CODEC_H}
    ${SERIALIZATION_H}
    ${MAPPING_CONFIGURATION_H}
    ${MAPPING_ENGINE_H}
    ${MAPPING_H}
    ddl.h
    legacy_error_macros.h
)
set_target_properties(ddl PROPERTIES FOLDER ddl)

target_include_directories(ddl PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# a_util is public since its part of the ddl api
target_link_libraries(ddl PUBLIC a_util)
target_compile_features(ddl INTERFACE cxx_std_11) # C++11 for self and dependants
target_compile_options(ddl PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fPIC>)

# Create version config
include(CMakePackageConfigHelpers)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/cmake/ddl-config-version.cmake
    COMPATIBILITY AnyNewerVersion)
    
configure_file(cmake/ddl-config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/ddl/ddlConfig.cmake"
)

if(ddl_cmake_enable_tests)
    set(ddl_cmake_integrated_tests ON)
    enable_testing()
    add_subdirectory(test)
endif()

add_subdirectory(ddlgenerators)

if(ddl_cmake_enable_installation)
    install(TARGETS ddl ARCHIVE DESTINATION lib)
    
    install(FILES ${DDLREPRESENTATION_INSTALL} DESTINATION include/${DDLREPRESENTATION_DIR})
    install(FILES ${CODEC_INSTALL} DESTINATION include/${CODEC_DIR})
    install(FILES ${SERIALIZATION_INSTALL} DESTINATION include/${SERIALIZATION_DIR})
    install(FILES ${MAPPING_CONFIGURATION_INSTALL} DESTINATION include/${MAPPING_DIR}/configuration)
    install(FILES ${MAPPING_ENGINE_INSTALL} DESTINATION include/${MAPPING_DIR}/engine)
    install(FILES ${MAPPING_INSTALL} DESTINATION include/${MAPPING_DIR})
    install(FILES ddl.h legacy_error_macros.h DESTINATION include)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/ddl-config-version.cmake DESTINATION cmake)
    configure_file(cmake/ddl-config.cmake.in ${CMAKE_INSTALL_PREFIX}/cmake/ddl-config.cmake @ONLY)
    install(FILES cmake/migrate_to_ddl_4_1.cmake cmake/ddl4.1_replacements.txt cmake/migrate_to_ddl_4_1.sh.in DESTINATION cmake)
    install(FILES README.md DESTINATION .)
else()
    # This allows a host product to use find_package from within the build-tree
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cmake/ddl-config.cmake "")
endif()
##this enables a post-install command, so this must be "Last Dir Standing"!!!
add_subdirectory(doc)

# License Information must be delivered anyway!
install(FILES doc/license/MPL2.0.txt DESTINATION ./)
install(FILES doc/license/MPL2.0.txt DESTINATION doc/license)
install(FILES doc/input/used_licenses.md DESTINATION doc/license)
install(FILES doc/input/mpl.md DESTINATION doc/license)

