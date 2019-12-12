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

#include "ddlstream.h"
#include <ddl.h>

namespace ddl
{
    //define all needed error types and values locally
    _MAKE_RESULT(-4, ERR_POINTER)
    _MAKE_RESULT(-20, ERR_NOT_FOUND)
    _MAKE_RESULT(-37, ERR_NOT_INITIALIZED)

    DDLStream::DDLStream() :
        _name{},
        _type{},
        _description{},
        _structs{},
        _init_flag{false},
        _level{4},
        _language_version(DDLVersion::ddl_version_invalid)
    {
    }

    DDLStream::DDLStream(DDLComplex* poType,
                           const std::string& name,
                           const std::string& strDescription,
                           DDLStreamStructVec vecStructs,
                           int const nCreationLevel) :
        _name(name),
        _type{poType},
        _description(strDescription),
        _structs{},
        _init_flag{NULL != poType},
        _level{nCreationLevel},
        _language_version(DDLVersion::ddl_version_invalid)
    {
        cloneStructs(vecStructs);
    }

    DDLStream::DDLStream(const DDLStream &oStream) :
        _name(oStream.getName()),
        _type{oStream.getTypeObject()},
        _description(oStream.getDescription()),
        _structs{},
        _init_flag{NULL != oStream.getTypeObject()},
        _level{oStream.getCreationLevel()},
        _language_version(oStream.getDDLVersion())
    {
        cloneStructs(oStream.getStructs());
    }

    DDLStream& DDLStream::operator=(DDLStream other)
    {
        swap(*this, other);
        return *this;
    }

    DDLStream::DDLStream(DDLStream&& other)
    {
        swap(*this, other);
    }

    DDLStream::~DDLStream()
    {
        std::transform(_structs.begin(), _structs.end(), _structs.begin(), deleteChild<DDLStreamStruct>);
        // The type object needs not to be deleted as this pointer only is a
        // reference to an object in the struct vector of the central
        // description object!
        _structs.clear();
        _type = NULL;
    }

    a_util::result::Result DDLStream::accept(IDDLVisitor *poVisitor) const
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        return poVisitor->visit(this);
    }
    a_util::result::Result DDLStream::accept(IDDLChangeVisitor *poVisitor)
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        return poVisitor->visit(this);
    }

    a_util::result::Result DDLStream::create(DDLComplex* poType,
                               const std::string& name,
                               const std::string& strDescription,
                               DDLStreamStructVec vecStructs,
                               int const nCreationLevel)
    {
        if (!poType) { return ERR_POINTER; }
        _type = poType;
        _name = name;
        _description = strDescription;
        cloneStructs(vecStructs);
        _init_flag = true;
        _level = nCreationLevel;
        return a_util::result::SUCCESS;
    }

    std::string DDLStream::getType() const
    {
        if (!_init_flag)
        {
            return a_util::strings::empty_string;
        }
        return _type->getName();
    }

    void DDLStream::setType(DDLComplex* const pType)
    {
        _type = pType;
        _init_flag = (_type != NULL);
    }

    DDLComplex * DDLStream::getTypeObject() const
    {
        if (!_init_flag)
        {
            return NULL;
        }
        return _type;
    }

    DDLVersion DDLStream::getDDLVersion() const
    {
        return _language_version;
    }

    a_util::result::Result DDLStream::setDDLVersion(DDLVersion& language_version)
    {
        _language_version = language_version;
        return a_util::result::SUCCESS;
    }

    void DDLStream::setName(const std::string& name)
    {
        _name = name;
    }

    const std::string& DDLStream::getName() const
    {
        return _name;
    }

    void DDLStream::setDescription(const std::string& strDescription)
    {
        _description = strDescription;
    }

    std::string DDLStream::getDescription() const
    {
        return _description;
    }

    void DDLStream::cloneStructs(DDLStreamStructVec vecStructs)
    {
        std::transform(_structs.begin(), _structs.end(), _structs.begin(), DDLDescription::deleteChild<DDLStreamStruct>);
        _structs.clear();

        _structs = vecStructs;
        std::transform(_structs.begin(), _structs.end(), _structs.begin(), DDLDescription::clone<DDLStreamStruct>);
    }

    void DDLStream::refStructs(DDLStreamStructVec vecStructs)
    {
        std::transform(_structs.begin(), _structs.end(), _structs.begin(), DDLDescription::deleteChild<DDLStreamStruct>);
        _structs.clear();

        _structs = vecStructs;
    }


    void DDLStream::addStruct(DDLStreamStruct* poStruct)
    {
        if (NULL != poStruct)
        {
            _structs.push_back(poStruct);
        }
    }
    a_util::result::Result DDLStream::removeStruct(const std::string& strStructName)
    {
        DDLStreamStructIt oIt = _structs.begin();
        while (oIt != _structs.end())
        {
            if ((*oIt)->getName() == strStructName)
            {
                DDLDescription::deleteChild(*oIt);
                _structs.erase(oIt);
                return a_util::result::SUCCESS;
            }
            oIt++;
        }
        return ERR_NOT_FOUND;
    }


    DDLStreamStructVec& DDLStream::getStructs()
    {
        return _structs;
    }

    const DDLStreamStructVec& DDLStream::getStructs() const
    {
        return _structs;
    }

    bool DDLStream::isInitialized() const
    {
        return _init_flag;
    }

    int DDLStream::getCreationLevel() const
    {
        return _level;
    }

    void swap(DDLStream& lhs, DDLStream& rhs) noexcept
    {
        using std::swap;
        swap(lhs._description, rhs._description);
        swap(lhs._init_flag, rhs._init_flag);
        swap(lhs._language_version, rhs._language_version);
        swap(lhs._level, rhs._level);
        swap(lhs._name, rhs._name);
        swap(lhs._structs, rhs._structs);
        swap(lhs._type, rhs._type);
    }
}   // namespace ddl
