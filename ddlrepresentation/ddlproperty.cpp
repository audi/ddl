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

#include "ddlproperty.h"
#include <ddl.h>

namespace ddl
{
    //define all needed error types and values locally
    _MAKE_RESULT(-37, ERR_NOT_INITIALIZED);

    DDLProperty::DDLProperty(const std::string& name, const std::string& type) :
        _name(name),
        _type(type),
        _init_flag{true}
    {
    }

    a_util::result::Result DDLProperty::accept(IDDLVisitor *visitor) const
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        return visitor->visit(this);
    }

    a_util::result::Result DDLProperty::accept(IDDLChangeVisitor *visitor)
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        return visitor->visit(this);
    }

    const std::string& DDLProperty::getName() const
    {
        return _name;
    }    

    const std::string& DDLProperty::getType() const
    {
        return _type;
    }

    bool DDLProperty::isInitialized() const
    {
        return _init_flag;
    }

    int DDLProperty::getCreationLevel() const
    {
        return 0;
    }

}   // namespace ddl
