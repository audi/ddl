##################################################################
#  Common Package Integration File for the LIB - ddl generator
#
#
##################################################################

##################################################################
# Section for required other package to compile
find_package(Clara REQUIRED)    #enable INTERFACE target Clara

include(ddl_generator_lib.sources)
include_directories(${DDL_GENERATOR_LIB_INCLUDE_DIR})

set(SOURCES
    ${DDL_GENERATOR_COMMON}
    ${HEADER_PRESENTATION_H}
    ${HEADER_PRESENTATION_CPP}
)

add_library(ddl_generator STATIC ${SOURCES})

target_include_directories(ddl_generator PUBLIC
    $<BUILD_INTERFACE:${DDL_GENERATOR_LIB_INCLUDE_DIR}>
)

target_link_libraries(ddl_generator 
    ddl Clara)

   
set_target_properties(ddl_generator PROPERTIES FOLDER ddl/utils)