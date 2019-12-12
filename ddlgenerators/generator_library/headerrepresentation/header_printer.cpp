/**
 * @file
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
#include "header_printer.h"
#include "header_header.h"
#include "header_basic_type.h"

namespace ddl_generator
{
    namespace oo
    {
        //define all needed error types and values locally
        _MAKE_RESULT(0, ERR_NOERROR)
        _MAKE_RESULT(-4, ERR_POINTER)
        _MAKE_RESULT(-5, ERR_INVALID_ARG)
        _MAKE_RESULT(-11, ERR_INVALID_FILE)
        _MAKE_RESULT(-20, ERR_NOT_FOUND)
        _MAKE_RESULT(-24, ERR_PATH_NOT_FOUND)
        _MAKE_RESULT(-27, ERR_OPEN_FAILED)
        _MAKE_RESULT(-38, ERR_FAILED)
    }
}

using namespace ddl_generator::oo;

using namespace ddl;

namespace ddl
{

HeaderPrinter::HeaderPrinter() : _header(NULL)
{ }

HeaderPrinter::~HeaderPrinter()
{ }

a_util::result::Result HeaderPrinter::writeToFile(const a_util::filesystem::Path &filename)
{
    std::string guarded_header_output = addHeaderGuards(filename, _header_output);
    if (a_util::filesystem::writeTextFile(filename, guarded_header_output) != a_util::filesystem::OK)
    {
        return (ERR_OPEN_FAILED);
    }
    return ERR_NOERROR;
}

const std::string& HeaderPrinter::getHeader()
{
    return _header_output;
}

a_util::result::Result HeaderPrinter::visit(const Header* header)
{
    _header = header;
    _header_output = "";
    _header_output.append("// This is a generated file, changes to it may be overwritten in the future.\n\n");

    if (_name_space.length() > 0)
    {
        _header_output.append(a_util::strings::format("namespace %s\n{\n", _name_space.c_str()));
    }

    HeaderTypedefs typedefs = header->getTypedefs();
    for (HeaderTypedefs::iterator iter = typedefs.begin(); iter != typedefs.end(); iter++)
    {
        if (isFailed((appendType(*iter))))
        {
            return ERR_FAILED;
        }
    }

    HeaderEnums enums = header->getEnums();
    for (auto iter = enums.begin(); iter != enums.end(); iter++)
    {
        if (isFailed(((*iter)->accept(this))))
        {
            return ERR_FAILED;
        }
    }

    HeaderStructs structs = header->getStructs();
    for (HeaderStructs::iterator iter = structs.begin(); iter != structs.end(); iter++)
    {
        if (isFailed((appendType(*iter))))
        {
            return ERR_FAILED;
        }
    }
    HeaderConstants constants = header->getConstants();
    for (HeaderConstants::iterator iter = constants.begin(); iter != constants.end(); iter++)
    {
        if (isFailed(((*iter)->accept(this))))
        {
            return ERR_FAILED;
        }
    }

    if (isFailed((printUnknownTypes())))
    {
        return ERR_FAILED;
    }

    if (_name_space.length() > 0)
    {
        _header_output.append(a_util::strings::format("} // namespace %s\n", _name_space.c_str()));
    }

    return ERR_NOERROR;
}

a_util::result::Result HeaderPrinter::visit(const HeaderBasicType* basic_type)
{
    CollectType(basic_type, true);
    _header_output.append("// The following type is assumed to be known:\n");
    _header_output.append("// ");
    _header_output.append(basic_type->getName());
    _header_output.append("\n\n");
    return ERR_NOERROR;
}

a_util::result::Result HeaderPrinter::visit(const HeaderTypedef* type_def)
{
    printDescription(type_def);
    // Set known first, so no infinite loop will occur when looking for base type
    CollectType(type_def, true);
    appendType(type_def->getOriginalType());
    _header_output.append(a_util::strings::format("typedef %s %s;\n\n",
        type_def->getOriginalType()->getName().c_str(), type_def->getName().c_str()));
    return ERR_NOERROR;
}

a_util::result::Result HeaderPrinter::visit(const HeaderConstant* constant)
{
    appendType(constant->getType());
    _header_output.append("const ");
    _header_output.append(constant->getType()->getName());
    _header_output.append(" ");
    _header_output.append(constant->getName());
    _header_output.append(" = ");
    _header_output.append(constant->asString());
    _header_output.append(";\n\n");
    return ERR_NOERROR;
}

a_util::result::Result HeaderPrinter::visit(const HeaderStruct* header_struct)
{
    printDescription(header_struct);
    // Set known first, so no infinite loop will occur when looking for base type
    CollectType(header_struct, true);
    // Make sure all types are already defined
    HeaderStructElementVec elements = header_struct->getElements();
    for (HeaderStructElementVec::iterator iter = elements.begin(); iter != elements.end(); iter++)
    {
        appendType((*iter)->getType());
    }
    _header_output.append(a_util::strings::format("#pragma pack(push,%d)\n", header_struct->getPacking()));
    _header_output.append("typedef struct\n{\n");
    for (HeaderStructElementVec::iterator iter = elements.begin(); iter != elements.end(); iter++)
    {
        if (isFailed(((*iter)->accept(this))))
        {
            return ERR_FAILED;
        }
    }
    _header_output.append(a_util::strings::format("} %s;\n", header_struct->getName().c_str()));
    _header_output.append("#pragma pack(pop)\n\n");
    CollectType(header_struct, true);
    return ERR_NOERROR;
}

a_util::result::Result HeaderPrinter::visit(const HeaderStructElement* header_struct)
{
    if (!header_struct || !header_struct->getType())
    {
        return ERR_INVALID_ARG;
    }
    printDescription(header_struct->getDescription(), header_struct->getComment(), true);
    appendType(header_struct->getType());
    _header_output.append("    ");
    if (header_struct->isStatic())
    {
        _header_output.append("static ");
    }
    if (header_struct->isConst())
    {
        _header_output.append("const ");
    }
    _header_output.append(header_struct->getType()->getName());
    if (header_struct->isPointer())
    {
        _header_output.append("* ");
    }
    _header_output.append(" ");
    _header_output.append(header_struct->getName());
    if (header_struct->getArraySize() > 1)
    {
        _header_output.append(a_util::strings::format("[%d]", header_struct->getArraySize()));
    }
    _header_output.append(";\n");
    return ERR_NOERROR;
}

a_util::result::Result HeaderPrinter::visit(const HeaderEnum* header_enum)
{
    CollectType(header_enum, true);
    printDescription(header_enum);
    _header_output.append("typedef enum {\n");
    for (auto iter = header_enum->getValues().begin(); iter != header_enum->getValues().end(); iter++)
    {
        _header_output.append(a_util::strings::format("    %s=%i,\n", iter->second.c_str(), iter->first));
    }
    if (header_enum->getValues().size() > 0)
    {
        // remove last ',' since some versions of C/C++ don't allow trailing commas in enums
        _header_output.resize(_header_output.length() - 2);
    }
    _header_output.append(a_util::strings::format("\n} %s;\n\n", header_enum->getName().c_str()));
    return ERR_NOERROR;
}

a_util::result::Result HeaderPrinter::appendType(const HeaderType* type)
{
    // See if we already know this type and therefore have printed it already
    for (HeaderConstTypes::iterator iter = _known_types.begin(); iter != _known_types.end(); iter++)
    {
        if (*iter == type)
        {
            return ERR_NOERROR;
        }
    }
    // Search the header for the type.
    if (_header != NULL)
    {
        for (HeaderTypedefs::const_iterator iter = _header->getTypedefs().begin(); iter != _header->getTypedefs().end(); iter++)
        {
            if (*iter == type)
            {
                // Found it, append it
                return ((*iter)->accept(this));
            }
        }
        for (HeaderStructs::const_iterator iter = _header->getStructs().begin(); iter != _header->getStructs().end(); iter++)
        {
            if (*iter == type)
            {
                // Found it, append it
                return ((*iter)->accept(this));
            }
        }
    }
    // Nothing found so far, so type is unknown
    CollectType(type, false);
    return (ERR_NOT_FOUND);
}

a_util::result::Result HeaderPrinter::CollectType(const ddl::HeaderType* type, bool is_known)
{
    if (is_known)
    {
        if (std::find(_known_types.begin(), _known_types.end(), type) ==
            _known_types.end())
        {
            _known_types.push_back(type);
        }
    }
    else
    {
        if (std::find(_unknown_types.begin(), _unknown_types.end(), type) ==
            _unknown_types.end())
        {
            _unknown_types.push_back(type);
        }
    }
    return ERR_NOERROR;
}

a_util::result::Result HeaderPrinter::printUnknownTypes()
{
    // visit all types that are in the unknown types set but not in the knowN types set
    HeaderConstTypes vec = _unknown_types;
    for (HeaderConstTypes::iterator iter = vec.begin(); iter != vec.end(); iter++)
    {
        if (std::find(_known_types.begin(), _known_types.end(), *iter) == _known_types.end())
        {
            (*iter)->accept(this);
        }
    }
    if (vec.size() != _unknown_types.size())
    {
        printUnknownTypes();
    }
    return ERR_NOERROR;
}

a_util::result::Result HeaderPrinter::printDescription(const std::string& description, const std::string& comment, bool indent)
{
    if (description.length() == 0 && comment.length() == 0)
    {
        return ERR_NOERROR;
    }

    std::string indent_string;
    if (indent)
    {
        indent_string = "    ";
    }

    _header_output.append(indent_string);
    _header_output.append("/**\n");
    if (description.length() > 0)
    {
        _header_output.append(a_util::strings::format("%s * %s\n", indent_string.c_str(), description.c_str()));
    }
    if (comment.length() > 0)
    {
        _header_output.append(a_util::strings::format("%s * %s\n", indent_string.c_str(), comment.c_str()));
    }
    _header_output.append(indent_string);
    _header_output.append("*/\n");
    return ERR_NOERROR;
}

a_util::result::Result HeaderPrinter::printDescription(const HeaderType* type)
{
    return printDescription(type->getDescription(), type->getComment());
}

bool invalidHeaderChar(char c)
{
    bool is_num = (c >= '0' && c <= '9');
    bool is_uppercase_alpha = (c >= 'A' && c <= 'Z');
    bool is_lowercase_alpha = (c >= 'a' && c <= 'z');
    bool is_allowed_punctuation = (c == '.' || c == '_' || c == '-');
    return !(is_num || is_uppercase_alpha || is_lowercase_alpha || is_allowed_punctuation);
}

std::string HeaderPrinter::addHeaderGuards(const a_util::filesystem::Path &filename, const std::string &unguarded_header_content)
{
    std::string guard_name = filename.getLastElement().toString();
    guard_name.erase(std::remove_if(guard_name.begin(), guard_name.end(), invalidHeaderChar), guard_name.end());
    std::replace(guard_name.begin(), guard_name.end(), '.', '_');
    std::transform(guard_name.begin(), guard_name.end(), guard_name.begin(), ::toupper);

    std::string output;

    output.append("#ifndef ");
    output.append(guard_name + "\n");
    output.append("#define ");
    output.append(guard_name + "\n\n");

    output.append(unguarded_header_content);

    output.append("\n#endif //");
    output.append(guard_name + "\n");

    return output;
}

void HeaderPrinter::SetNamespace(std::string name_space)
{
    _name_space = name_space;
}

} // namespace ddl
