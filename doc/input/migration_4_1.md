# Migration to DDL 4.1 {#page_migration_4_1}

DDL version 4.1.0 introduced an updated coding style which led to changes in all APIs.
This guide aims to make the migration as easy as possible.

# Changes in DDL 4.1

The updated coding style brought numerous changes. Relevant for the API are:

* No hungarian notation anymore: Class names are no longer preceded by 'c', i.e. `cDDLDescription` -> `DDLDescription` 
* Class methods and static functions are now lower case, i.e. `GetValue()` -> `getValue()`
* Change of enum values to lower case, i.e. `PLATFORM_LITTLE_ENDIAN_8` -> `plattform_little_endian_8` 

## Version variables changes

In DDL 4.1 all DDL Version variables were changed from a plain floating point value to objects of type `DDLVersion`.
This change made `DDLVersionHelper` obsolete and incorporated most of its functionality in DDLVersion.
The necessary adaptions in DDL user code for these changes can not be done automatically by the migration scripts 
and therefore have to be done manually.


# Migration scripts

To assist in the migration process a new cmake function gets delivered with the DDL package. 
After having called `find_package(ddl)` somewhere in your CMake script, the CMake function `ddl_migrate_4_1()`
is available. This function will create a migration script for the specified target. It should be called after
a target was defined. Example call:

````cmake
ddl_migrate_4_1(TARGET ddl_editor)
````

This will create the migration script in the targets build dir 
(i.e. `build_Debug/src/ddl_editor/gui/ddl_editor_migrate_to_ddl_4_1.sh`)
during the CMake build process.

## The migration script TARGET_migrate_to_ddl_4_1.sh

**Important** The resulting script is a `bash` script that uses POSIX tools like `find` and `sed`. Therefore the migration should either be done using a 
Linux operating system or using a Windows environment that offers a bash runtime and those tools (like MinGW or the Git Bash console)
 
The migration is done in two steps:

* Replacing all DDL class names and enum values with the changed ones. This is a simple text replacement,
without any syntax awareness. Therefore it will replace these symbols even in disabled code etc.
* Running clang-tidy to automatically fix the remaining issues. This will basically compile the code 
and tries to fix the arising problems.

The process will run automatically when executing the migrate script.

**Warning:**  There are several things to note:

* clang-tidy is called several times, since there will be a lot of issues which obscure each other. 
*This can take quite some time.*
* clang-tidy is very strict concerning include directories. While the 'normal' build process might not
have problems discovering header files in sub folder of include directories, clang-tidy will not find
them. You have to correctly specify all include directories for your target in the CMake script
(which is a good idea anyways). This might happen for example if the target is not explicitly linked
against Qt::Gui (but other Qt libs). Qt::Gui has to be added to the linked libraries for clang-tidy
to find those headers.
* If your target uses generated header files (like Qt applications usually do) those have to be 
generated before the migration script can do it's work. If the CMake build fails before those are 
generated you have to fix the dependant targets and run the CMake build process again. Also make sure 
that the destination of those headers is part of the include dirs.
* After the migration is done, clang-tidy might output some remaining warnings. These might not be 
directly related to the migration process and originate from the static code analysis. While fixing 
them is probably a good idea, they are usually not build breaking.
* The migration script changes your code. A lot. There might be things that get broken by it that are 
not even related to the DDL code. You should check the code after the migration is done. And of course
have it under source control so that any changes can easily be reverted.
* If you have serveral targets that depend on each other and use the DDL library, you should migrate 
them one-by-one along the dependency graph beginning at the leafes.

## Remaining issues

Not all issues will be fixed by the migration script. After the script is done try to build the 
application and fix any build errors manually. These are usually minor issues like `GetValue` or `IsEqual` 
instead of `getValue` and `isEqual`.

After the migration is done the calls to `ddl_migrate_4_1()` can be removed from the CMake scripts.

