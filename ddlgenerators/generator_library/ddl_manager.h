/**
 * @file
 * Implementation of FEP MDE core
 *
 * @copyright
 * @verbatim
   Copyright @ 2017 Audi Electronics Venture GmbH. All rights reserved.

       This Source Code Form is subject to the terms of the Mozilla
       Public License, v. 2.0. If a copy of the MPL was not distributed
       with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

   If it is not possible or desirable to put the notice in a particular file, then
   You may include the notice in a location (such as a LICENSE file in a
   relevant directory) where a recipient would be likely to look for such a notice.

   You may add additional accurate notices of copyright ownership.
   @endverbatim
*/

#include <ddl.h>

#ifndef DDL_MANAGER_H
#define DDL_MANAGER_H


class DDLManager
{

public:
    /**
    * CTOR
    * @param [in] 
    */
    DDLManager();

    /**
    * DTOR
    */
    ~DDLManager();
     
    /// Set default ddl
    a_util::result::Result setDefaultDDL();
    /**
    * Nerge with ddl file, or with default ddl if empty
    * @param[in] ddl_path  - path to the ddl file
    * @param[out] error_msg - error message, gives more information if it failed
    * @param[in] struct_name   - merge only this struct to the existing ddl
    * @retval ERR_NOERROR Everything went as expected.
    */
    a_util::result::Result mergeWithDDLFile(const a_util::filesystem::Path& ddl_path, std::string& error_msg, const std::string& struct_name = "");
    /**
    * Nerge with ddl file, or with default ddl if empty
    * @param[in] header_path  - path to the header file
    * @param[in] struct_name   - merge only this struct to the existing ddl
    * @retval ERR_NOERROR Everything went as expected.
    */
    a_util::result::Result mergeWithHeaderFile(const a_util::filesystem::Path& header_path, std::string& error_msg,
        const ddl::DDLVersion& version = ddl::DDLVersion::ddl_version_current, const std::string& struct_name = "");
    /// Getter for ddl representation
    ddl::DDLDescription* getDDL();
    /// Print to ddl file
    a_util::result::Result printToDDLFile(const a_util::filesystem::Path& ddl_path, std::string& error_msg);
    /// Print to header file
    a_util::result::Result printToHeaderFile(const a_util::filesystem::Path& header_file, std::string& error_msg, 
        const std::string name_space = "", const std::string displace = "");
    /// Search if the file contains structs
    a_util::result::Result searchForStructs();
    /**
    * Verify if the file is valid and write some errors and warnings 
    * @retval message list
    */
    ddl::ImporterMsgList checkValidity();

private:

    /** find struct struct_name in description.
    *   Recursively add all child-structs of struct_name as well
    *   @param[in] description description to search in
    *   @param[in] struct_name name of struct so search for
    *   @retval result
    */
    a_util::result::Result findStructRecursively(ddl::DDLDescription* description, const std::string &struct_name);

private:
    /// File name
    a_util::filesystem::Path _filename;
    /// ddl representation
    ddl::DDLDescription* _ddl_description;

};

#endif //DDL_MANAGER_H
