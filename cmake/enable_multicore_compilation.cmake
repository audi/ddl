if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    # enable multicore compilation
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()
