/**
 * @file
 * Implementation of core
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

#ifndef DDL_GENERATOR_H
#define DDL_GENERATOR_H

#include <ddl.h>

#include "ddl_manager.h"

#define EXTENSION_DESCRIPTIONFILE   "DESCRIPTION"
#define EXTENSION_HEADERFILE        "H"

class DDLUtilsCore
{

public:
    /**
    * CTOR
    * @param [in] 
    */
    DDLUtilsCore();

    /**
    * DTOR
    */
    ~DDLUtilsCore();

    /**
    * Create new ddl file from header file
    * @param[in] header_path         - path to the header file
    * @param[in] description_path    - path to the ddl file
    * @param[in] struct_name             - merge only this struct to the existing ddl
    * @retval ERR_NOERROR Everything went as expected.
    */
    a_util::result::Result generateDescriptionFile(const a_util::filesystem::Path& header_path, 
        const a_util::filesystem::Path& description_path, const ddl::DDLVersion& version = ddl::DDLVersion::ddl_version_current,
        const std::string struct_name = "");
    /**
    * Create new header file from ddl file
    * @param[in] description_path    - path to the ddl file
    * @param[in] header_path         - path to the header file
    * @param[in] struct_name             - merge only this struct to the existing ddl
    * @retval ERR_NOERROR Everything went as expected.
    */
    a_util::result::Result generateHeaderFile(const a_util::filesystem::Path& description_path, 
        const a_util::filesystem::Path& header_path, const std::string struct_name = "", const std::string name_space = "", const std::string displace = "");

private:
    /**
    * Add existing ddl file
    * @param[in] description_path  - path to the ddl file
    * @param[out] error_msg - error message, gives more information if it failed
    * @param[in] struct_name    - merge only this struct to the existing ddl
    * @retval ERR_NOERROR Everything went as expected.
    */
    a_util::result::Result setDescription(const a_util::filesystem::Path& description_path, 
        std::string& error_msg, const std::string struct_name = "");
    /**
    * Create new ddl file from header file
    * @param[in] header_path  - path to the header file
    * @param[in] description_path    - path to the new ddl file
    * @param[in] struct_name    - merge only this struct to the existing ddl
    * @retval ERR_NOERROR Everything went as expected.
    */
    a_util::result::Result setDescriptionFromHeader(const a_util::filesystem::Path& header_path, 
        const a_util::filesystem::Path& description_path, std::string& error_msg, 
        const ddl::DDLVersion& version = ddl::DDLVersion::ddl_version_current, const std::string struct_name = "");

private:
    /// Mamanger map
    DDLManager* _ddl_manager;
};

#endif //DDL_GENERATOR_H
