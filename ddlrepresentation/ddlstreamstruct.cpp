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

#include "ddlstreamstruct.h"
#include <ddl.h>

namespace ddl
{
    //define all needed error types and values locally
    _MAKE_RESULT(-4, ERR_POINTER)
    _MAKE_RESULT(-37, ERR_NOT_INITIALIZED)

    DDLStreamStruct::DDLStreamStruct(DDLComplex* poType,
                                       unsigned int const uiBytepos,
                                       const std::string& name) :

        _type{poType},
        _bytes_pos{uiBytepos},
        _name{name},
        _init_flag{NULL != poType}
    {
    }

    a_util::result::Result DDLStreamStruct::accept(IDDLVisitor *poVisitor) const
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        return poVisitor->visit(this);
    }
    a_util::result::Result DDLStreamStruct::accept(IDDLChangeVisitor *poVisitor)
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        return poVisitor->visit(this);
    }

    a_util::result::Result DDLStreamStruct::create(DDLComplex* poType,
                                     unsigned int const uiBytepos,
                                     const std::string& name)
    {
        if (!poType) { return ERR_POINTER; }
        _type = poType;
        _bytes_pos = uiBytepos;
        _name = name;
        _init_flag = true;
        return a_util::result::SUCCESS;
    }

    const std::string& DDLStreamStruct::getType() const
    {
        if (!_init_flag)
        {
            return a_util::strings::empty_string;
        }
        return _type->getName();
    }

    void DDLStreamStruct::setType(DDLComplex* const pType)
    {
        _type = pType;
        _init_flag = (_type != NULL);
    }

    DDLComplex * DDLStreamStruct::getTypeObject() const
    {
        if (!_init_flag)
        {
            return NULL;
        }
        return _type;
    }

    void DDLStreamStruct::setName(const std::string& name)
    {
        _name = name;
    }

    const std::string& DDLStreamStruct::getName() const
    {
        if (_name.empty())
        {
            return getType();
        }
        return _name;
    }

    unsigned int DDLStreamStruct::getBytepos() const
    {
        return _bytes_pos;
    }

    void DDLStreamStruct::setBytepos(unsigned int const uiBytepos)
    {
        _bytes_pos = uiBytepos;
    }

    bool DDLStreamStruct::isInitialized() const
    {
        return _init_flag;
    }
}   // namespace ddl
