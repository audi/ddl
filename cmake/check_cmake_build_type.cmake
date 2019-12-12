## Check build type and terminate cmake if none is given
if(UNIX)
    if(NOT ";${CMAKE_BUILD_TYPE};" STREQUAL ";Debug;" AND
       NOT ";${CMAKE_BUILD_TYPE};" STREQUAL ";Release;" AND
       NOT ";${CMAKE_BUILD_TYPE};" STREQUAL ";RelWithDebInfo;" AND
       NOT ";${CMAKE_BUILD_TYPE};" STREQUAL ";MinSizeRel;")
        message(FATAL_ERROR "'CMAKE_BUILD_TYPE' is either empty or set to an unknown build type")
    endif(NOT ";${CMAKE_BUILD_TYPE};" STREQUAL ";Debug;" AND
          NOT ";${CMAKE_BUILD_TYPE};" STREQUAL ";Release;" AND
          NOT ";${CMAKE_BUILD_TYPE};" STREQUAL ";RelWithDebInfo;" AND
          NOT ";${CMAKE_BUILD_TYPE};" STREQUAL ";MinSizeRel;")
endif(UNIX)
