# DDL Library {#mainpage}


# Description

The Data Definition Language (DDL) Package provides utilities to handle data descriptions and data defined by these descriptions.

    #include <ddl.h>

* [DDL specification](doc/input/ddl_specification.md) ([link](@ref page_ddl_specification))




_____________________
 
# DDL Library Parts
 
This library contains following internal parts: 
 
## OO DDL
 
A Memory Model for the DDL Language.

* See [OO DDL Documentation](doc/input/oo-ddl.md) ([link](@ref page_oo_ddl))

        ddlrepresentation/pkg_ddlrepresentation.h


## Codec API

Access, decode and encode ddl described memory data.

* See [Codec Documentation](doc/input/codec.md) ([link](@ref page_codec))

        codec/pkg_codec.h

 
## DDL-2-header and header-2-DDL generation

Generating a c-header file from DDL and vice versa.

* See [DDL Generators](doc/input/ddl_generators.md)([link](@ref page_ddl_generators))

        ddlgenerators/generatorlibrary
        ddlgenerators/ddl2header
        ddlgenerators/header2ddl

## Mapping Functionality

Map data from one DDL structure to another
 
* See [Mapping specification](doc/input/mapping_specification.md)([link](@ref page_signal_mapping_spec))

        mapping/pkg_mapping.h
        
## DDL 4.1 migration process

DDL version 4.1.0 introduced an updated coding style which led to changes in all APIs.

* A migration guide is available at [Migration to 4.1](doc/input/migration_4_1.md) ([link](@ref page_migration_4_1))

________________________

# License Information
 
This library is provided under [MPL 2.0](doc/input/mpl.md)([link](@ref page_mpl)).
Have also a look at doc/licence for all license information. 

See also [used licenses](doc/input/used_licenses.md)([link](@ref page_used_licenses)) for licenses of integrated libraries.

________________________

# Change Log

For change history have a look at [Change Log](doc/changelog.md)([link](@ref page_changelog))
________________________
 
# Dependencies

## a_util library
 
The libraries above depend on the *a_util* library
See *a_util* repository at https://github.com/AEV/a_util

## How to build

### Build Environment
 
The libraries are build and tested only under following compilers and operating systems: 

#### Windows 7 64 Bit

* Visual Studio C++ 2015 Update 3.1 (Update 3 and KB3165756)
 
#### Linux Ubuntu 16.04 LTS

* On other distributions make at least sure that libc has version >= 2.23 and libstdc++ >= 6.0.21.
* gcc 5.4 
 
#### Build a_util first
 
The DDL library will only build if a *installation* of a_util library is provided.
Following libraries of the a_utils are necessary:
* a_util_concurrency
* a_util_memory
* a_util_regex
* a_util_strings
* a_util_datetime
* a_util_filesystem
* a_util_xml
 
- Use CMAKE at least in version 3.5.1.  
- Use the release tag \p "v*a_util-version*" 
- Use CMakeLists.txt within the main directory as source directory
- Do not forget to set a CMAKE_INSTALL_PREFIX 
- Build and install for Debug and Release

#### Build ddl library
 
- Use CMAKE at least in version 3.5.1.  
- Use the relase tag \p "v*ddl-version*". 
- Use CMakeLists.txt within the main directory as Source Directory
- Do not forget to set a CMAKE_INSTALL_PREFIX 
- Build and install for Debug and Release

##### Cmake options and dependencies

<table>
<tr>
<td>
a_util_DIR
</td>
<td>
must be set to *a_util_install_DIR*/lib/cmake/a_util 
</td>
<td>
See a_util repository at https://github.com/AEV/a_util
</td>
</tr>
<tr>
<td>
ddl_cmake_enable_documentation ON/OFF 
</td>
<td>
choose wether a documentation is created or not
</td>
<td>
dependency to a valid doxygen executable needed (see http://www.doxygen.nl/)
</td>
</tr>
<tr>
<td>
ddl_cmake_enable_tests ON/OFF 
</td>
<td>
choose wether the tests where build while building the libraries or not
</td>
<td>
dependency to a valid gtest package needed (see https://github.com/google/googletest)
</td>
</tr>
<tr>
<td>
ddl_cmake_enable_generator_tools ON/OFF 
</td>
<td>
choose wether the tools must be build too
</td>
<td>
</td>
</tr>
<tr>
<td>
ddl_cmake_enable_installation ON/OFF 
</td>
<td>
choose wether the libraries are installed or not
</td>
<td>
</td>
</tr>
</table>

 
 

 




 
