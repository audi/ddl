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
#include "header_header.h"

namespace ddl_generator
{
    namespace oo
    {
        //define all needed error types and values locally
        _MAKE_RESULT(0, ERR_NOERROR)
        _MAKE_RESULT(-5, ERR_INVALID_ARG)
        _MAKE_RESULT(-20, ERR_NOT_FOUND)
    }
}

using namespace ddl_generator::oo;
using namespace ddl;


namespace ddl
{

Header::Header()
{ }

Header::Header(const Header &other)
{
    _name = other._name;
    for (HeaderTypedefs::const_iterator iter = other._typedefs.begin(); iter != other._typedefs.end(); iter++)
    {
        _typedefs.push_back(new HeaderTypedef(*(*iter)));
    }
    for (HeaderConstants::const_iterator iter = other._constants.begin(); iter != other._constants.end(); iter++)
    {
        _constants.push_back(new HeaderConstant(*(*iter)));
    }
    for (HeaderStructs::const_iterator iter = other._structs.begin(); iter != other._structs.end(); iter++)
    {
        _structs.push_back(new HeaderStruct(*(*iter)));
    }
    for (auto iter = other._enums.begin(); iter != other._enums.end(); iter++)
    {
        _enums.push_back(new HeaderEnum(*(*iter)));
    }
}

Header::~Header()
{
    for (HeaderTypedefs::iterator iter = _typedefs.begin(); iter != _typedefs.end(); iter++)
    {
        delete *iter;
    }
    _typedefs.clear();
    for (HeaderConstants::iterator iter = _constants.begin(); iter != _constants.end(); iter++)
    {
        delete *iter;
    }
    _constants.clear();
    for (HeaderStructs::iterator iter = _structs.begin(); iter != _structs.end(); iter++)
    {
        delete *iter;
    }
    _structs.clear();

    for (auto iter = _enums.begin(); iter != _enums.end(); iter++)
    {
        delete *iter;
    }
    _enums.clear();
}


const std::string& Header::getName() const
{
    return _name;
}

a_util::result::Result Header::setName(const std::string& name)
{
    _name = name;
    return ERR_NOERROR;
}

const HeaderTypedefs& Header::getTypedefs() const
{
    return _typedefs;
}

a_util::result::Result Header::addTypedef(HeaderTypedef* header_typedef)
{
    _typedefs.push_back(header_typedef);
    return ERR_NOERROR;
}

a_util::result::Result Header::removeTypedef(const std::string &name)
{
    for (HeaderTypedefs::iterator iter = _typedefs.begin(); iter != _typedefs.end(); iter++)
    {
        if ((*iter)->getName() == name)
        {
            delete *iter;
            _typedefs.erase(iter);
            return ERR_NOERROR;
        }
    }
    return (ERR_NOT_FOUND);
}

const HeaderConstants& Header::getConstants() const
{
    return _constants;
}

a_util::result::Result Header::addConstant(HeaderConstant* constant)
{
    _constants.push_back(constant);
    return ERR_NOERROR;
}

a_util::result::Result Header::removeConstant(const std::string &name)
{
    for (HeaderConstants::iterator iter = _constants.begin(); iter != _constants.end(); iter++)
    {
        if ((*iter)->getName() == name)
        {
            delete *iter;
            _constants.erase(iter);
            return ERR_NOERROR;
        }
    }
    return (ERR_NOT_FOUND);
}

const HeaderStructs& Header::getStructs() const
{
    return _structs;
}

a_util::result::Result Header::addStruct(HeaderStruct* header_struct)
{
    _structs.push_back(header_struct);
    return ERR_NOERROR;
}

a_util::result::Result Header::removeStruct(const std::string &name)
{
    for (HeaderStructs::iterator iter = _structs.begin(); iter != _structs.end(); iter++)
    {
        if ((*iter)->getName() == name)
        {
            delete *iter;
            _structs.erase(iter);
            return ERR_NOERROR;
        }
    }
    return (ERR_NOT_FOUND);
}

const HeaderEnums& Header::getEnums() const
{
    return _enums;
}

a_util::result::Result Header::addEnum(HeaderEnum* header_enum)
{
    _enums.push_back(header_enum);
    return ERR_NOERROR;
}

a_util::result::Result Header::accept(IHeaderVisitor * visitor) const
{
    if (!visitor)
    {
        return ERR_INVALID_ARG;
    }
    return (visitor->visit(this));
}

}   // namespace ddl
