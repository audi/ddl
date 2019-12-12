## This must always be the first check when running cmake

set(CURRENT_OPTION "ddl_cmake_enable_warn_on_default_cmake_install_prefix")
option(${CURRENT_OPTION} "Warn if 'CMAKE_INSTALL_PREFIX is set to default (default: ON)" ON)

if(${CURRENT_OPTION})
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OR
       ";${util_cmake_CMAKE_DEFAULT_INSTALL_PREFIX};" STREQUAL ";${CMAKE_INSTALL_PREFIX};")
        message(WARNING "'CMAKE_INSTALL_PREFIX' is currently set to the default value. Suppress \
this check by disabling '${CURRENT_OPTION}'")

        ## Enable warning for more than just the first run of cmake
        set(util_cmake_CMAKE_DEFAULT_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} CACHE INTERNAL "")
    else()
        unset(util_cmake_CMAKE_DEFAULT_INSTALL_PREFIX)
    endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OR
          ";${util_cmake_CMAKE_DEFAULT_INSTALL_PREFIX};" STREQUAL ";${CMAKE_INSTALL_PREFIX};")
endif(${CURRENT_OPTION})

unset(CURRENT_OPTION)
