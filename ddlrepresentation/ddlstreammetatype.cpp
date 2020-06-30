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

#include "ddlstreammetatype.h"
#include <ddl.h>

namespace ddl
{
    //define all needed error types and values locally
    _MAKE_RESULT(-37, ERR_NOT_INITIALIZED);

    DDLStreamMetaType::DDLStreamMetaType(const std::string& name,
                                           const std::string& strVersion,
                                           const DDLPropertyVec& vecProperties,
                                           const DDLStreamMetaType* pParent) :
        _name{name},
        _version{strVersion},
        _properties{vecProperties},
        _parent{pParent},
        _init_flag{true}
    {
    }

    DDLStreamMetaType::DDLStreamMetaType(const DDLStreamMetaType &oStreamMetaType) :
        _name{oStreamMetaType._name},
        _version{oStreamMetaType._version},
        _properties{},
        _parent{oStreamMetaType._parent},
        _init_flag{true}
    {
        cloneProperties(oStreamMetaType._properties);
    }

    DDLStreamMetaType& DDLStreamMetaType::operator=(DDLStreamMetaType other)
    {
        swap(*this, other);
        return *this;
    }

    DDLStreamMetaType::DDLStreamMetaType(DDLStreamMetaType&& other)
    {
        swap(*this, other);
    }

    DDLStreamMetaType::~DDLStreamMetaType()
    {
        std::transform(_properties.begin(), _properties.end(), _properties.begin(), deleteChild<DDLProperty>);
    }

    a_util::result::Result DDLStreamMetaType::accept(IDDLVisitor *poVisitor) const
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        return poVisitor->visit(this);
    }

    a_util::result::Result DDLStreamMetaType::accept(IDDLChangeVisitor *poVisitor)
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        return poVisitor->visit(this);
    }

    const std::string& DDLStreamMetaType::getName() const
    {
        return _name;
    }

    void DDLStreamMetaType::setName(const std::string& name)
    {
        _name = name;
    }

    const std::string& DDLStreamMetaType::getVersion() const
    {
        return _version;
    }

    void DDLStreamMetaType::cloneProperties(const DDLPropertyVec& vecProperties)
    {
        std::transform(_properties.begin(), _properties.end(), _properties.begin(), DDLDescription::deleteChild<DDLProperty>);
        _properties = vecProperties;
        std::transform(_properties.begin(), _properties.end(), _properties.begin(), DDLDescription::clone<DDLProperty>);
    }

    DDLPropertyVec& DDLStreamMetaType::getProperties()
    {
        return _properties;
    }

    const DDLPropertyVec& DDLStreamMetaType::getProperties() const
    {
        return _properties;
    }

    const DDLStreamMetaType* DDLStreamMetaType::getParentObject() const
    {
        return _parent;
    }

    const std::string& DDLStreamMetaType::getParent() const
    {
        if (_parent)
        {
            return _parent->getName();
        }

        return a_util::strings::empty_string;
    }

    bool DDLStreamMetaType::isInitialized() const
    {
        return _init_flag;
    }

    int DDLStreamMetaType::getCreationLevel() const
    {
        return 0;
    }

    void swap(DDLStreamMetaType& lhs, DDLStreamMetaType& rhs) noexcept
    {
        using std::swap;
        swap(lhs._name, rhs._name);
        swap(lhs._version, rhs._version);
        swap(lhs._properties, rhs._properties);
        swap(lhs._parent, rhs._parent);
        swap(lhs._init_flag, rhs._init_flag);
    }

}   // namespace ddl
