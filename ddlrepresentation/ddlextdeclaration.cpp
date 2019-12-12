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

#include "ddlextdeclaration.h"

namespace ddl
{
    //define all needed error types and values locally
    _MAKE_RESULT(-5, ERR_INVALID_ARG)

    DDLExtDeclaration::DDLExtDeclaration(const std::string& key,
        const std::string& value) :
    _key(key), _value(value), _init_flag(true)
    {
    }

    a_util::result::Result DDLExtDeclaration::accept(IDDLVisitor *visitor) const
    {
        return visitor->visit(this);
    }
    a_util::result::Result DDLExtDeclaration::accept(IDDLChangeVisitor *visitor)
    {
        return visitor->visit(this);
    }

    const std::string& DDLExtDeclaration::getName() const
    {
        return _key;
    }

    bool DDLExtDeclaration::isInitialized() const
    {
        return _init_flag;
    }

    a_util::result::Result DDLExtDeclaration::create(const std::string& key,
        const std::string& value)
    {
        if (key.empty())
        {
            return ERR_INVALID_ARG;
        }
        _key = key;
        _value = value;
        _init_flag = true;
        return a_util::result::SUCCESS;
    }

    std::string DDLExtDeclaration::getKey() const
    {
        return _key;
    }

    void DDLExtDeclaration::setKey(const std::string& key)
    {
        _key = key;
    }


    std::string DDLExtDeclaration::getValue() const
    {
        return _value;
    }

    void DDLExtDeclaration::setValue(const std::string& value)
    {
        _value = value;
    }

}   // namespace ddl
