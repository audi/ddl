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

#include "ddlrefunit.h"
#include <ddl.h>

namespace ddl
{
    //define all needed error types and values locally
    _MAKE_RESULT(-4, ERR_POINTER);
    _MAKE_RESULT(-37, ERR_NOT_INITIALIZED);

    DDLRefUnit::DDLRefUnit(IDDLUnit* poUnit, int const nPower, DDLPrefix* poPrefix) :
        _unit{poUnit},
        _power{nPower},
        _prefix{poPrefix},
        _init_flag{(NULL != poUnit) && (NULL != poPrefix)}
    {
    }

    a_util::result::Result DDLRefUnit::accept(IDDLVisitor *poVisitor) const
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        return poVisitor->visit(this);
    }

    a_util::result::Result DDLRefUnit::accept(IDDLChangeVisitor *poVisitor)
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        return poVisitor->visit(this);
    }

    const std::string& DDLRefUnit::getName() const
    {
        if (NULL == _unit)
        {
            return a_util::strings::empty_string;
        }
        return _unit->getName();
    }

    IDDLUnit * DDLRefUnit::getUnitObject() const
    {
        if (!_init_flag)
        {
            return NULL;
        }
        return _unit;
    }

    void DDLRefUnit::setUnitObject(IDDLUnit* pUnit)
    {
        _unit = pUnit;
        _init_flag = (NULL != _unit) &&
            (NULL != _prefix);
    }

    int DDLRefUnit::getPower() const
    {
        return _power;
    }

    void DDLRefUnit::setPower(int const nPower)
    {
        _power = nPower;
    }

    std::string DDLRefUnit::getPrefix() const
    {
        if (!_init_flag)
        {
            return a_util::strings::empty_string;
        }
        return _prefix->getName();
    }

    DDLPrefix * DDLRefUnit::getPrefixObject() const
    {
        if (!_init_flag)
        {
            return NULL;
        }
        return _prefix;
    }

    void DDLRefUnit::setPrefixObject(DDLPrefix* pPrefix)
    {
        _prefix = pPrefix;
        _init_flag = (NULL != _unit) &&
            (NULL != _prefix);
    }


    bool DDLRefUnit::isInitialized() const
    {
        return _init_flag;
    }

    a_util::result::Result DDLRefUnit::create(IDDLUnit* poUnit, int const nPower, DDLPrefix* poPrefix)
    {
        if (!poUnit || !poPrefix) { return ERR_POINTER; }
        _unit = poUnit;
        _power = nPower;
        _prefix = poPrefix;
        _init_flag = true;
        return a_util::result::SUCCESS;
    }
}   // namespace ddl
