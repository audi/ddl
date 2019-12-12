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

#include "ddlbaseunit.h"
#include "ddlvisitor_intf.h"

namespace ddl
{
    //define all needed error types and values locally
    _MAKE_RESULT(-5, ERR_INVALID_ARG)
    _MAKE_RESULT(-37, ERR_NOT_INITIALIZED)

    DDLBaseunit::DDLBaseunit() :
        _name{},
        _symbol{},
        _description{},
        _init_flag{},
        _level{1}
    {
    }

    DDLBaseunit::DDLBaseunit(const std::string& name,
                               const std::string& symbol,
                               const std::string& description,
                               int const creation_level) :
        _name(name),
        _symbol(symbol),
        _description(description),
        _init_flag(true),
        _level(creation_level)
    {
    }

    a_util::result::Result DDLBaseunit::create(const std::string& name,
                              const std::string& symbol,
                              const std::string& description,
                              int const creation_level)
    {
        if (name.empty())
        {
            return ERR_INVALID_ARG;
        }
        _name = name;
        _symbol = symbol;
        _description = description;
        _init_flag = true;
        _level = creation_level;
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLBaseunit::accept(IDDLVisitor *visitor) const
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        return visitor->visit(this);
    }

    a_util::result::Result DDLBaseunit::accept(IDDLChangeVisitor *visitor)
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        return visitor->visit(this);
    }

    bool DDLBaseunit::isInitialized() const
    {
        return _init_flag;
    }

    const std::string& DDLBaseunit::getName() const
    {
        return _name;
    }

    void DDLBaseunit::setName(const std::string& name)
    {
        _name = name;
    }

    const std::string& DDLBaseunit::getSymbol() const
    {
        return _symbol;
    }

    void DDLBaseunit::setSymbol(const std::string& symbol)
    {
        _symbol = symbol;
    }


    const std::string& DDLBaseunit::getDescription() const
    {
        return _description;
    }

    void DDLBaseunit::setDescription(const std::string& description)
    {
        _description = description;
    }


    bool DDLBaseunit::isPredefined() const
    {
        return _level == -1;   // cMediaManager::DL_AlwaysThere
    }

    bool DDLBaseunit::isOverwriteable() const
    {
        return _level> 0;
    }

    int DDLBaseunit::getCreationLevel() const
    {
        return _level;
    }
}   // namespace ddl
