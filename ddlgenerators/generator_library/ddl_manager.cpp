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

#include "ddl_manager.h"
#include <ddl.h>
#include "headerrepresentation/header_header.h"
#include "headerrepresentation/header_importer.h"
#include "headerrepresentation/header_to_ddl_converter.h"
#include "headerrepresentation/header_printer.h"
#include "headerrepresentation/ddl_to_header_converter.h"
#include <algorithm>

namespace ddl_generator
{
namespace oo
{
    //define all needed error types and values locally
    _MAKE_RESULT(0, ERR_NOERROR)
    _MAKE_RESULT(-4, ERR_POINTER)
    _MAKE_RESULT(-11, ERR_INVALID_FILE)
    _MAKE_RESULT(-24, ERR_PATH_NOT_FOUND)
    _MAKE_RESULT(-38, ERR_FAILED)
}
}

using namespace ddl_generator::oo;

DDLManager::DDLManager()
{
    _ddl_description = NULL;
}

DDLManager::~DDLManager()
{
    if (NULL != _ddl_description)
    {
        ddl::DDLImporter::destroyDDL(_ddl_description);
        _ddl_description = NULL;
    }
}

a_util::result::Result DDLManager::setDefaultDDL()
{
    if (_ddl_description == NULL)
    {
        _ddl_description = ddl::DDLDescription::createDefault();
        _ddl_description->getHeader()->setDescription("Generated with AEV Media Description Editor");
    }
    return ERR_NOERROR;
}

a_util::result::Result DDLManager::mergeWithDDLFile(const a_util::filesystem::Path& ddl_path, std::string& error_msg, const std::string& struct_name)
{
    if (!a_util::filesystem::exists(ddl_path))
    {
        error_msg = a_util::strings::format("Description file '%s' not found.", ddl_path.toString().c_str());
         return ERR_PATH_NOT_FOUND;
    }

    ddl::DDLImporter ddl_importer;
    if (isFailed(ddl_importer.setFile(ddl_path)))
    {
        error_msg = a_util::strings::format("Error in File '%s': %s", ddl_path.getLastElement().toString().c_str(), ddl_importer.getErrorDesc().c_str());
        return ERR_FAILED;
    }
    ;
    if (a_util::result::isFailed(ddl_importer.createNew()))
    {
        error_msg = ddl_importer.getErrorDesc();
        return ERR_INVALID_FILE;
    }

    ddl::DDLDescription* imported_description = ddl_importer.getDDL();

    a_util::result::Result res = ERR_NOERROR;

    // merge if dll already exists
    if (struct_name.size() == 0)
    {
        if (_ddl_description == NULL)
        {
            _ddl_description = imported_description;
        }
        else
        {
            res = _ddl_description->merge(*imported_description, 2U);
            // Destroy DDL
            if (NULL != imported_description)
            {
                ddl::DDLImporter::destroyDDL(imported_description);
                imported_description = NULL;
            }
        }
    }
    else
    {
        // get struct from description file
        res = findStructRecursively(imported_description, struct_name);
        if (a_util::result::isFailed(res))
        {
            error_msg = a_util::strings::format("Error: Struct '%s' not found.", struct_name.c_str());
        }
        // Destroy DDL
        if (NULL != imported_description)
        {
            ddl::DDLImporter::destroyDDL(imported_description);
            imported_description = NULL;
        }

    }

    if (NULL == _ddl_description)
    {
        res = ERR_POINTER;
    }

    return res;
}

a_util::result::Result DDLManager::mergeWithHeaderFile(
    const a_util::filesystem::Path& header_path, std::string& error_msg, 
    const ddl::DDLVersion& version /*= 4.0*/, const std::string& struct_name)
{
    if (!a_util::filesystem::exists(header_path))
    {
        error_msg = a_util::strings::format("Headerfile '%s' not found", header_path.toString().c_str());
         return ERR_PATH_NOT_FOUND;
    }

    ddl::HeaderImporter importer;
    ddl::HeaderTypesVec* default_types = ddl::HeaderImporter::getDefaultTypes();
    if (isFailed((importer.setFileName(header_path))))
    {
        return ERR_FAILED;
    }
    if (isFailed((importer.setKnownTypes(default_types))))
    {
        return ERR_FAILED;
    }
    if (isFailed((importer.createNew())))
    {
        error_msg = importer.getLastError();
        return ERR_FAILED;
    }
    if (importer.getLastError().length() > 0)
    {
        LOG_ERROR(importer.getLastError().c_str());
        return ERR_INVALID_FILE;
    }
    ddl::Header* header = importer.getHeader();
    if (NULL == header)
    {
        return ERR_POINTER;
    }

    ddl::DDLDescription* tmp_desc = NULL;
    a_util::result::Result res = ERR_NOERROR;
    if (struct_name.size() == 0)
    {
        // create DDL from header file
        ddl::HeaderToDDLConverter converter;
        if (isFailed((converter.visit(header))))
        {
            return ERR_FAILED;
        }
        if (isFailed((converter.createNew(version))))
        {
            return ERR_FAILED;
        }
        tmp_desc = converter.getDDL();
        if (NULL == tmp_desc)
        {
            res = ERR_POINTER;
        }
    }
    else
    {
        // get struct from header file
        ddl::HeaderStructs header_structs = header->getStructs();
        ddl::HeaderStruct* found_struct = NULL;
        for (ddl::HeaderStructs::const_iterator it_struct = header_structs.begin();
            it_struct != header_structs.end();
            it_struct++)
        {
            if (0 == struct_name.compare((*it_struct)->getName()))
            {
                found_struct = *it_struct;
                break;
            }
        }

        // check if struct is available in header file
        if (NULL == found_struct)
        {
            error_msg = a_util::strings::format("Error: Struct '%s' not found in header file.", struct_name.c_str());
            res = ERR_INVALID_FILE;
        }
        else
        {
            // create DDL from struct of header file
            ddl::HeaderToDDLConverter converter;
            if (isFailed((converter.visit(found_struct))))
            {
                return ERR_FAILED;
            }
            if (isFailed((converter.createNew(version))))
            {
                return ERR_FAILED;
            }
            tmp_desc = converter.getDDL();
            if (NULL == tmp_desc)
            {
                res = ERR_POINTER;
            }
        }
    }


    if (NULL != _ddl_description)
    {
        if (NULL != tmp_desc)
        {
            res = _ddl_description->merge(*tmp_desc);
        }
    }
    else
    {
        _ddl_description = tmp_desc;
    }

    if (NULL != header)
    {
        importer.DestroyHeader();
        header = NULL;
    }
    for (ddl::HeaderTypesVec::iterator it = default_types->begin(); it != default_types->end(); it++)
    {
        delete *it;
        *it = NULL;
    }
    default_types->clear();
    delete default_types;
    default_types = NULL;

    return res;
}

ddl::DDLDescription* DDLManager::getDDL()
{
    return _ddl_description;
}

a_util::result::Result DDLManager::printToDDLFile(const a_util::filesystem::Path& ddl_path, std::string& error_msg)
{
    ddl::DDLPrinter ddl_printer;
    if (a_util::result::isFailed(ddl_printer.visitDDL(_ddl_description)))
    {
        return ERR_POINTER;
    }
    if (a_util::result::isFailed(ddl_printer.toFile(ddl_path)))
    {
        return (ERR_INVALID_FILE);
    }

    return ERR_NOERROR;
}

a_util::result::Result DDLManager::printToHeaderFile(const a_util::filesystem::Path& header_file, std::string& error_msg, 
    const std::string name_space /** = ""*/, const std::string displace /** = ""*/)
{

    // Create header from ddl
    ddl::DDLToHeaderConverter converter;
    converter.setDisplaceableString(displace);
    ddl::HeaderTypesVec* default_types = NULL;
    default_types = ddl::HeaderImporter::getDefaultTypes();
    converter.setKnownTypes(default_types);
    if (a_util::result::isFailed(converter.visitDDL(_ddl_description)))
    {
        error_msg = converter.getError();
        return ERR_FAILED;
    }
    if (isFailed((converter.createNew())))
    {
        error_msg = converter.getError();
        return ERR_FAILED;
    }
    ddl::Header* header = converter.getHeader();
    if (NULL == header)
    {
        return ERR_POINTER;
    }
    // create header file
    ddl::HeaderPrinter printer;
    printer.SetNamespace(name_space);
    if (a_util::result::isFailed(printer.visit(header)))
    {
        return (ERR_INVALID_FILE);
    }
    if (a_util::result::isFailed(printer.writeToFile(header_file)))
    {
        return (ERR_FAILED);
    }

    return ERR_NOERROR;
}

a_util::result::Result DDLManager::searchForStructs()
{
    ddl::DDLComplexVec ddl_structs = _ddl_description->getStructs();
    if (0 == ddl_structs.size())
    {
        return (ERR_INVALID_FILE);
    }
    return ERR_NOERROR;
}

ddl::ImporterMsgList DDLManager::checkValidity()
{
    ddl::ImporterMsgList msg_list = ddl::DDLInspector::checkValidyOfNestedStructs(_ddl_description);
    if (!(msg_list.size() > 0))
    {
        ddl::DDLInspector ddl_inspector(false);
        ddl_inspector.visitDDL(_ddl_description);
        msg_list = ddl_inspector.getSuggestions();
    }
    return msg_list;
}


a_util::result::Result DDLManager::findStructRecursively(ddl::DDLDescription* description, const std::string &struct_name)
{
    if (NULL != _ddl_description && NULL != _ddl_description->getStructByName(struct_name))
    {
        // struct was already added, skip
        return ERR_NOERROR;
    }
    a_util::result::Result res = ERR_NOERROR;

    ddl::DDLComplexVec ddl_structs = description->getStructs();
    ddl::DDLComplex* found_struct = NULL;
    for (ddl::DDLComplexVec::const_iterator it_struct = ddl_structs.begin();
        it_struct != ddl_structs.end(); it_struct++)
    {
        if (0 == a_util::strings::compare(struct_name.c_str(), (*it_struct)->getName().c_str()))
        {
            found_struct = new ddl::DDLComplex(**it_struct);
            break;
        }
    }
    // check if struct is available in description file
    if (NULL == found_struct)
    {
        return ERR_INVALID_FILE;
    }

    if (NULL == _ddl_description)
    {
        _ddl_description = ddl::DDLDescription::createDefault();    
    }
    _ddl_description->addStruct(found_struct);

    // add child-structs
    for (auto it_element = found_struct->getElements().begin(); it_element != found_struct->getElements().end(); it_element++)
    {
        ddl::DDLElement* element = *it_element;
        const std::string element_name = element->getTypeObject()->getName();
        if (element->getTypeObject()->isComplex())
        {
            if (isFailed(findStructRecursively(description, element_name)))
            {
                return ERR_INVALID_FILE;
            }
            element->setType(_ddl_description->getStructByName(element_name));
        }
    }

    return ERR_NOERROR;
}
