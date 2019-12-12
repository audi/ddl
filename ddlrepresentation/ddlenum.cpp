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

#include "ddlenum.h"
#include "ddlvisitor_intf.h"

namespace ddl
{
    //define all needed error types and values locally
    _MAKE_RESULT(-4, ERR_POINTER)
    _MAKE_RESULT(-20, ERR_NOT_FOUND)

    struct tEnumName
    {
        tEnumName(std::string const& strEnumName) : m_strEnumName(strEnumName) { }

        bool operator () (std::pair<std::string, std::string> const& oPair)
        {
            return (oPair.first == m_strEnumName);
        }

        std::string m_strEnumName;
    };

    DDLEnum::DDLEnum() :
        _name{},
        _name_values{},
        _unit{},
        _type{},
        _init_flag{},
        _level{1}
    {
    }

    DDLEnum::DDLEnum(IDDLDataType* poType,
                       const std::string& name,
                       const EnumNameValueVec& vecNameValues,
                       IDDLUnit* poUnit /*= NULL*/,
                       int nCreationLevel /*= 1 */ ) :
        _name(name),
        _name_values(vecNameValues),
        _unit(poUnit),
        _type(poType),
        _init_flag(true),
        _level(nCreationLevel)
    {
    }

    a_util::result::Result DDLEnum::accept(IDDLVisitor *poVisitor) const
    {
        return poVisitor->visit(this);
    }
    a_util::result::Result DDLEnum::accept(IDDLChangeVisitor *poVisitor)
    {
        return poVisitor->visit(this);
    }

    const std::string& DDLEnum::getName() const
    {
        return _name;
    }

    void DDLEnum::setName(const std::string& name)
    {
        _name = name;
    }

    const EnumNameValueVec& DDLEnum::getValues() const
    {
        return _name_values;
    }

    void DDLEnum::setValues(const EnumNameValueVec& vecNameValues)
    {
        _name_values = vecNameValues;
    }

    const std::string& DDLEnum::getType() const
    {
        if (_type != NULL)
        {
            return _type->getName();
        }

        return a_util::strings::empty_string;
    }

    void DDLEnum::setType( IDDLDataType* const pType )
    {
        _type = pType;
    }

    IDDLDataType * DDLEnum::getTypeObject() const
    {
        return _type;
    }

    bool DDLEnum::isInitialized() const
    {
        return _init_flag;
    }

    int DDLEnum::getCreationLevel() const
    {
        return _level;
    }


    a_util::result::Result DDLEnum::create(IDDLDataType* poType,
                             const std::string& name,
                             const EnumNameValueVec& vecNameValues,
                             IDDLUnit* poUnit /*= NULL*/,
                             int nCreationLevel /*= 1*/ )
    {
        if (!poUnit || !poType) { return ERR_POINTER; }
        _name = name;
        _unit = poUnit;
        _init_flag = true;
        _level = nCreationLevel;
        _name_values = vecNameValues;
        _type = poType;
        return a_util::result::SUCCESS;
    }

    void DDLEnum::setUnit(IDDLUnit* poUnit)
    {
        _unit = poUnit;
    }

    std::string DDLEnum::getUnit() const
    {
        if (NULL == _unit)
        {
            return a_util::strings::empty_string;
        }
        return _unit->getName();
    }

    IDDLUnit * DDLEnum::getUnitObject() const
    {
        if (!_init_flag)
        {
            return NULL;
        }
        return _unit;
    }

    bool DDLEnum::isPredefined() const
    {
        return _level == -1;   // cMediaManager::DL_AlwaysThere
    }

    bool DDLEnum::isOverwriteable() const
    {
        return _level > 0;
    }

    a_util::result::Result DDLEnum::removeElement(const std::string& strElement)
    {
        EnumNameValueVec::iterator itNameVal =
            find_if(_name_values.begin(), _name_values.end(), tEnumName(strElement));
        if (itNameVal != _name_values.end())
        {
            _name_values.erase(itNameVal);
            return a_util::result::SUCCESS;
        }

        return ERR_NOT_FOUND;
    }

    a_util::result::Result DDLEnum::getValue(const std::string& strElement, std::string& strValue) const
    {
        EnumNameValueVec::const_iterator itNameVal =
            find_if(_name_values.begin(), _name_values.end(), tEnumName(strElement));

        if (itNameVal != _name_values.end())
        {
            strValue = itNameVal->second;
            return a_util::result::SUCCESS;
        }

        return ERR_NOT_FOUND;
    }

    a_util::result::Result DDLEnum::setValue(const std::string& strElement, const std::string& strValue)
    {
        EnumNameValueVec::iterator itNameVal =
            find_if(_name_values.begin(), _name_values.end(), tEnumName(strElement));

        if (itNameVal != _name_values.end())
        {
            itNameVal->second = strValue;
            return a_util::result::SUCCESS;
        }

        return ERR_NOT_FOUND;
    }

    a_util::result::Result DDLEnum::getNameForValue(const std::string& strValue, std::string& strElement)
    {
        for (EnumNameValueVec::iterator itNameVal = _name_values.begin();
             itNameVal != _name_values.end();
             ++itNameVal)
        {
            if (itNameVal->second == strValue)
            {
                strElement = itNameVal->first;
                return a_util::result::SUCCESS;
            }
        }
        return ERR_NOT_FOUND;
    }

}   // namespace ddl
