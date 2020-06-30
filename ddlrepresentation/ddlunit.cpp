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

#include "ddlunit.h"
#include "ddl_type.h"
#include "ddlrefunit.h"
#include "ddldescription.h"

namespace ddl
{
    //define all needed error types and values locally
    _MAKE_RESULT(-5, ERR_INVALID_ARG);
    _MAKE_RESULT(-20, ERR_NOT_FOUND);

    DDLUnit::DDLUnit() :
        _name{},
        _numerator{},
        _denominator{},
        _offset{},
        _ddl_ref_units{},
        _init_flag{},
        _level{1}
    {
    }

    DDLUnit::DDLUnit(DDLUnit&& other)
    {
        swap(*this, other);
    }

    DDLUnit::~DDLUnit()
    {
        std::transform(_ddl_ref_units.begin(), _ddl_ref_units.end(),
            _ddl_ref_units.begin(), DDL::deleteChild<DDLRefUnit>);
            _ddl_ref_units.clear();
    }

    DDLUnit::DDLUnit(const std::string& name,
                     const std::string& strNumerator,
                     const std::string& strDenominator,
                     double const fOffset,
                     DDLRefUnitVec vecDDLRefUnits,
                     int const nCreationLevel) : 
        _name{name},
        _numerator{strNumerator},
        _denominator{strDenominator},
        _offset{fOffset},
        _ddl_ref_units{},
        _init_flag{true},
        _level{nCreationLevel}
    {
        cloneRefUnits(vecDDLRefUnits);
    }

    DDLUnit::DDLUnit(const DDLUnit& oUnit) :
        _name{oUnit.getName()},
        _numerator{oUnit.getNumerator()},
        _denominator{oUnit.getDenominator()},
        _offset{oUnit.getOffset()},
        _ddl_ref_units{},
        _init_flag{true},
        _level{oUnit.getCreationLevel()}
    {
        cloneRefUnits(oUnit.getRefUnits());
    }

    DDLUnit& DDLUnit::operator=(DDLUnit other)
    {
        swap(*this, other);
        return *this;
    }

    a_util::result::Result DDLUnit::accept(IDDLVisitor *poVisitor) const
    {
        return poVisitor->visit(this);
    }
    a_util::result::Result DDLUnit::accept(IDDLChangeVisitor *poVisitor)
    {
        return poVisitor->visit(this);
    }

    bool DDLUnit::isInitialized() const
    {
        return _init_flag;
    }

    bool DDLUnit::isPredefined() const
    {
        return -1 == _level;   // cMediaManager::DL_AlwaysThere
    }

    bool DDLUnit::isOverwriteable() const
    {
        return _level > 0;
    }

    int DDLUnit::getCreationLevel() const
    {
        return _level;
    }

    a_util::result::Result DDLUnit::create(const std::string& name,
                             const std::string& strNumerator,
                             const std::string& strDenominator,
                             double const fOffset,
                             DDLRefUnitVec vecDDLRefUnits,
                             int const nCreationLevel)
    {
        if (name.empty())
        {
            return ERR_INVALID_ARG;
        }
        _name = name;
        _numerator = strNumerator;
        _denominator = strDenominator;
        _offset = fOffset;
        cloneRefUnits(vecDDLRefUnits);
        _init_flag = true;
        _level = nCreationLevel;
        return a_util::result::SUCCESS;
    }

    const std::string& DDLUnit::getName() const
    {
        return _name;
    }

    void DDLUnit::setName(const std::string& name)
    {
        _name = name;
    }


    std::string DDLUnit::getNumerator() const
    {
        return _numerator;
    }

    void DDLUnit::setNumerator(const std::string& strNumerator)
    {
        _numerator = strNumerator;
    }

    std::string DDLUnit::getDenominator() const
    {
        return _denominator;
    }

    void DDLUnit::setDenominator(const std::string& strDenominator)
    {
        _denominator = strDenominator;
    }

    double DDLUnit::getOffset() const
    {
        return _offset;
    }

    void DDLUnit::setOffset(double const fOffset)
    {
        _offset = fOffset;
    }

    DDLRefUnitVec& DDLUnit::getRefUnits()
    {
        return _ddl_ref_units;
    }

    const DDLRefUnitVec& DDLUnit::getRefUnits() const
    {
        return _ddl_ref_units;
    }

    void DDLUnit::addRefUnit(DDLRefUnit* poRefUnit, int nPos)
    {
        const DDLRefUnitVec::size_type szPos = static_cast<DDLRefUnitVec::size_type>(nPos);
        if (0 <= szPos && szPos < _ddl_ref_units.size())
        {
            _ddl_ref_units.insert(_ddl_ref_units.begin() + szPos,
                                    poRefUnit);
        }
        else
        {
            _ddl_ref_units.push_back(poRefUnit);
        }
    }

    a_util::result::Result DDLUnit:: removeRefUnit(const std::string& strRefUnit)
    {
        DDLRefUnitIt oIt = _ddl_ref_units.begin();
        while (oIt != _ddl_ref_units.end())
        {
            if ((*oIt)->getName() == strRefUnit)
            {
                delete (*oIt);
                _ddl_ref_units.erase(oIt);
                return a_util::result::SUCCESS;
            }
            oIt++;
        }
        return ERR_NOT_FOUND;
    }

    void DDLUnit::cloneRefUnits(DDLRefUnitVec vecDDLRefUnits)
    {
        std::transform(_ddl_ref_units.begin(), _ddl_ref_units.end(), _ddl_ref_units.begin(), DDLDescription::deleteChild<DDLRefUnit>);
        _ddl_ref_units.clear();
        _ddl_ref_units = vecDDLRefUnits;
        std::transform(_ddl_ref_units.begin(), _ddl_ref_units.end(), _ddl_ref_units.begin(), DDLDescription::clone<DDLRefUnit>);
    }

    void DDLUnit::setRefUnits(DDLRefUnitVec vecDDLRefUnits)
    {
        std::transform(_ddl_ref_units.begin(), _ddl_ref_units.end(), _ddl_ref_units.begin(), DDLDescription::deleteChild<DDLRefUnit>);
        _ddl_ref_units.clear();
        _ddl_ref_units = vecDDLRefUnits;
    }

    void swap(DDLUnit & lhs, DDLUnit & rhs) noexcept
    {
        using std::swap;
        swap(lhs._ddl_ref_units, rhs._ddl_ref_units);
        swap(lhs._denominator, rhs._denominator);
        swap(lhs._init_flag, rhs._init_flag);
        swap(lhs._level, rhs._level);
        swap(lhs._name, rhs._name);
        swap(lhs._numerator, rhs._numerator);
        swap(lhs._offset, rhs._offset);
    }

}   // namespace ddl
