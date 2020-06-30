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

#include "ddl_generator_core.h"
#include <ddl.h>

namespace ddl_generator
{
namespace oo
{
    //define all needed error types and values locally
    _MAKE_RESULT(0, ERR_NOERROR);
    _MAKE_RESULT(-11, ERR_INVALID_FILE);
    _MAKE_RESULT(-20, ERR_NOT_FOUND);
    _MAKE_RESULT(-24, ERR_PATH_NOT_FOUND);
    _MAKE_RESULT(-38, ERR_FAILED);
}
}
using namespace ddl_generator::oo;

/*Helper function*/
a_util::result::Result checkIfFileExists(const a_util::filesystem::Path& path)
{
    if (!a_util::filesystem::exists(path))
    {
        return ERR_PATH_NOT_FOUND;
    }

    return ERR_NOERROR;
}


/*DDLUtilsCore*/
DDLUtilsCore::DDLUtilsCore()
{
    _ddl_manager = NULL;
}

DDLUtilsCore::~DDLUtilsCore()
{
    delete _ddl_manager;
    _ddl_manager = NULL;
}

a_util::result::Result DDLUtilsCore::generateDescriptionFile(const a_util::filesystem::Path& header_path, 
    const a_util::filesystem::Path& description_path, 
    const ddl::DDLVersion& version /* 4.0 */, const std::string struct_name /*= ""*/)
{
    // check if description file still exists and struct has to be added
    if (a_util::result::isOk(checkIfFileExists(description_path)))
    {
        std::string error_msg;        
        if (a_util::result::isFailed(setDescription(description_path, error_msg)))
        {
            LOG_INFO(error_msg.c_str());
            return (ERR_INVALID_FILE);
        }
        ddl::DDLVersion existing_version = _ddl_manager->getDDL()->getHeader()->getLanguageVersion();
        if (existing_version > version)
        {
            LOG_ERROR(a_util::strings::format(
                "Exising description file has a newer Version (%s) as the requested Version (%s). Version downgrade is not possible!", 
                existing_version.toString().c_str(), version.toString().c_str()).c_str());
            return ERR_FAILED;
        }
    }

    std::string error_msg;
    if (a_util::result::isFailed(setDescriptionFromHeader(header_path, description_path, 
        error_msg, version, struct_name)))
    {
        LOG_INFO(error_msg.c_str());
        return ERR_FAILED;
    }
    if (a_util::result::isFailed(_ddl_manager->searchForStructs()))
    {
        LOG_INFO("Info: No structs found in description file.");
        return ERR_FAILED;
    }
            
    // create description file
    if (a_util::result::isFailed(_ddl_manager->printToDDLFile(description_path, error_msg)))
    {
        LOG_INFO("Error: Could not create file. %s", error_msg.c_str());
        return ERR_FAILED;
    }

    LOG_INFO("Success: Description file created.");
    return ERR_NOERROR;
}

a_util::result::Result DDLUtilsCore::generateHeaderFile(const a_util::filesystem::Path& description_path, 
    const a_util::filesystem::Path& header_path, const std::string struct_name /*= ""*/,
    const std::string name_space /*= ""*/, const std::string displace /*= ""*/)
{
    // check if description file still exists and struct has to be added
    if (a_util::result::isOk(checkIfFileExists(header_path)))
    {
        std::string error_msg;
        if (a_util::result::isFailed(setDescriptionFromHeader(header_path, description_path, error_msg)))
        {
            LOG_INFO(error_msg.c_str());
            return ERR_FAILED;
        }
    }

    std::string error_msg;
    if (a_util::result::isFailed(setDescription(description_path, error_msg, struct_name)))
    {
        LOG_INFO(error_msg.c_str());
        return (ERR_INVALID_FILE);
    }

    if (a_util::result::isFailed(_ddl_manager->printToHeaderFile(header_path, error_msg, name_space, displace)))
    {
        LOG_INFO("Error: Could not create header file. %s", error_msg.c_str());
        return (ERR_FAILED);
    }

    LOG_INFO("Success: Header file created.");
    return ERR_NOERROR;
}

a_util::result::Result DDLUtilsCore::setDescription(const a_util::filesystem::Path& description_path,
    std::string& error_msg, const std::string struct_name)
{
    if (!_ddl_manager)
    {
        _ddl_manager = new DDLManager();
    }
    return _ddl_manager->mergeWithDDLFile(description_path, error_msg, struct_name);
}

a_util::result::Result DDLUtilsCore::setDescriptionFromHeader(const a_util::filesystem::Path& header_path,
    const a_util::filesystem::Path& description_path, std::string& error_msg,  
    const ddl::DDLVersion& version /*= 4.0*/, const std::string struct_name /*= ""*/)
{
    if (!_ddl_manager)
    {
        _ddl_manager = new DDLManager();
    }

    a_util::result::Result result = _ddl_manager->mergeWithHeaderFile(header_path, error_msg, version, struct_name);
    if (a_util::result::isFailed(result))
    {
        LOG_INFO("Error: Could not read header file '%s'.", header_path.toString().c_str());
    }
    return result;
}
