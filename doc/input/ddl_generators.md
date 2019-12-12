# DDL Generators (ddl2header, header2ddl) {#page_ddl_generators}

ddl2header and header2ddl are command line utilites to automatically generate 
C/C++ header files from DDL desriptions and vice versa.

# Usage

## ddl2header

````
ddl2header [options]

+++++++++++++++++++++++++++++
+++ Command line option:  +++
+++++++++++++++++++++++++++++

To create a header file from an existing description file. You have to set the
following options:

 -headerfile=<PATH>                  - [Mandatory] The path to the header
                                         file (target).

 -descriptionfile=<PATH>             - [Mandatory] The path to the description
                                         file (source).

 -struct=<StructNameInHeader>        - [Optional] Just create the header
                                         file for the given struct
                                         of the description file.

++++++++++++++++
+++ Example  +++
++++++++++++++++
-headerfile=c:/myHeaderFile.h -descriptionfile=c:/myDescriptionFile.description
or-headerfile=c:/myHeaderFile.h -descriptionfile=c:/myDescriptionFile.description -struct=
tMyStruct
````

## header2ddl

````
header2ddl [options]

+++++++++++++++++++++++++++++
+++ Command line option:  +++
+++++++++++++++++++++++++++++

To create a header file from an existing description file. You have to set the
following options:

 -headerfile=<PATH>                  - [Mandatory] The path to the header
                                         file (source).

 -descriptionfile=<PATH>             - [Mandatory] The path to the description
                                         file (target).

 -struct=<StructNameInHeader>        - [Optional] Just create the description
                                         file for the given struct
                                         of the header file.

 -ddlversion=<VersionOfDDLFile>        - [Optional] Default value is ddl4.
                                         Supported formats are
                                         3.0 and 4.0

++++++++++++++++
+++ Example  +++
++++++++++++++++
-headerfile=c:/myHeaderFile.h -descriptionfile=c:/myDescriptionFile.description
or-headerfile=c:/myHeaderFile.h -descriptionfile=c:/myDescriptionFile.description -struct=
tMyStruct
````

## Notes

- Target files are never just overwritten (independent of conversion direction). 
If the target file exists the generators will try to merge the contents of source and target.  
**Warning:** If the target file contains content that can not be parsed by the ddl library
it might be missing from the result file. **Keep a backup!**
