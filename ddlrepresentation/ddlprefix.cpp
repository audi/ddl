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

#include "ddlprefix.h"
#include <ddl.h>

namespace ddl
{
    DDLPrefix::DDLPrefix() :
        _name{},
        _symbol{},
        _power{},
        _init_flag{},
        _level{1}
    {
    }

    DDLPrefix::DDLPrefix(const std::string& name, const std::string& symbol,
        int const power, int const creation_level) :
    _name(name), _symbol(symbol), _power(power),
        _init_flag(true), _level(creation_level)
    {
    }

    a_util::result::Result DDLPrefix::accept(IDDLVisitor *visitor) const
    {
        return visitor->visit(this);
    }
    a_util::result::Result DDLPrefix::accept(IDDLChangeVisitor *visitor)
    {
        return visitor->visit(this);
    }

    const std::string& DDLPrefix::getName() const
    {
        return _name;
    }

    void DDLPrefix::setName(const std::string& name)
    {
        _name = name;
    }

    bool DDLPrefix::isInitialized() const
    {
        return _init_flag;
    }

    a_util::result::Result DDLPrefix::create(const std::string& name, const std::string& symbol,
        int const power, int const creation_level)
    {
        _name = name;
        _symbol = symbol;
        _power = power;
        _init_flag = true;
        _level = creation_level;
        return a_util::result::SUCCESS;
    }

    std::string DDLPrefix::getSymbol() const
    {
        return _symbol;
    }

    void DDLPrefix::setSymbol(const std::string& symbol)
    {
        _symbol = symbol;
    }


    int DDLPrefix::getPower() const
    {
        return _power;
    }

    void DDLPrefix::setPower(int const power)
    {
        _power = power;
    }

    bool DDLPrefix::isPredefined() const
    {
        return -1 == _level;   // cMediaManager::DL_AlwaysThere
    }

    int DDLPrefix::getCreationLevel() const
    {
        return _level;
    }

}   // namespace ddl
