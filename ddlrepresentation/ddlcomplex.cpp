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

#include <utility>  //std::swap

#include "ddlcomplex.h"
#include "ddlalignment.h"
#include "ddldescription.h"
#include "ddlelement.h"

namespace ddl
{
    //define all needed error types and values locally
    _MAKE_RESULT(-5, ERR_INVALID_ARG);
    _MAKE_RESULT(-20, ERR_NOT_FOUND);

    DDLComplex::DDLComplex() :
    _language_version(DDLVersion::getDefaultVersion()), _init_flag(false), _level(1), _dynamic_elements(false)
    {
    }

    DDLComplex::DDLComplex(const std::string& name,
                             unsigned int const version,
                             const std::string& comment,
                             DDLAlignment::AlignmentType const alignment,
                             DDLElementVec ddl_elements,
                             int const creation_level,
                             DDLVersion language_version) :
        _language_version(language_version),
        _name(name),
        _version(version),
        _comment(comment),
        _alignment(alignment),
        _ddl_elements(),
        _init_flag(true),
        _level(creation_level), 
        _dynamic_elements(false)
    {
        cloneElements(ddl_elements);
    }

    DDLComplex::DDLComplex(const DDLComplex &other) :
        _language_version(other.getDDLVersion()),
        _name(other.getName()),
        _version(other.getVersion()),
        _comment(other.getComment()),
        _alignment(other.getAlignment()),
        _init_flag(true),
        _level(other.getCreationLevel())
    {
        cloneElements(other.getElements());
    }

    DDLComplex& DDLComplex::operator=(DDLComplex other)
    {
        swap(*this, other);
        return *this;
    }

    DDLComplex::DDLComplex(DDLComplex&& other) : DDLComplex()
    {
        swap(*this, other);
    }

    DDLComplex::~DDLComplex()
    {
        std::transform(_ddl_elements.begin(), _ddl_elements.end(),
            _ddl_elements.begin(), DDL::deleteChild<DDLElement>);
        _ddl_elements.clear();
    }

    a_util::result::Result DDLComplex::create(const std::string& name,
                                unsigned int const version,
                                const std::string& comment,
                                DDLAlignment::AlignmentType const alignment,
                                DDLElementVec ddl_elements,
                                int const creation_level)
    {
        if (name.empty())
        {
            return ERR_INVALID_ARG;
        }
        _name = name;
        _version = version;
        _comment = comment;
        _alignment = alignment;
        cloneElements(ddl_elements);
        _init_flag = true;
        _level = creation_level;
        _language_version = DDLVersion::getDefaultVersion();
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLComplex::accept(IDDLVisitor *visitor) const
    {
        return visitor->visit(this);
    }
    a_util::result::Result DDLComplex::accept(IDDLChangeVisitor *visitor)
    {
        return visitor->visit(this);
    }

    const std::string& DDLComplex::getName() const
    {
        return _name;
    }

    void DDLComplex::setName(const std::string& name )
    {
        _name = name;
    }

    bool DDLComplex::isInitialized() const
    {
        return _init_flag;
    }

    bool DDLComplex::isPredefined() const
    {
        return _level == -1;   // cMediaManager::DL_Internal
    }

    bool DDLComplex::isOverwriteable() const
    {
        return _level > 0;   // cMediaManager::DL_AlwaysThere
    }

    int DDLComplex::getCreationLevel() const
    {
        return _level;
    }

    unsigned int DDLComplex::getVersion() const
    {
        return _version;
    }

    void DDLComplex::setVersion(unsigned int const version)
    {
        _version = version;
    }

    DDLVersion DDLComplex::getDDLVersion() const
    {
        return _language_version;
    }

    a_util::result::Result DDLComplex::setDDLVersion(const DDLVersion& language_version)
    {
        _language_version = language_version;
        return a_util::result::SUCCESS;
    }

    void DDLComplex::setComment(const std::string& comment)
    {
        _comment = comment;
    }

    std::string DDLComplex::getComment() const
    {
        return _comment;
    }

    void DDLComplex::setAlignment(DDLAlignment::AlignmentType const alignment)
    {
        _alignment = alignment;
    }

    DDLAlignment::AlignmentType DDLComplex::getAlignment() const
    {
        return _alignment;
    }

    void DDLComplex::cloneElements(DDLElementVec ddl_elements)
    {
        std::transform(_ddl_elements.begin(), _ddl_elements.end(), _ddl_elements.begin(), DDLDescription::deleteChild<DDLElement>);
        _ddl_elements.clear();

        _ddl_elements = ddl_elements;
        std::transform(_ddl_elements.begin(), _ddl_elements.end(), _ddl_elements.begin(), DDLDescription::clone<DDLElement>);
        _dynamic_elements = false;
        for (DDLElementIt itEl = _ddl_elements.begin(); itEl != _ddl_elements.end(); itEl++)
        {
            if ((*itEl)->isDynamic())
            {
                _dynamic_elements = true;
                break;
            }
        }
    }

    void DDLComplex::setElements(DDLElementVec ddl_elements)
    {
        std::transform(_ddl_elements.begin(), _ddl_elements.end(), _ddl_elements.begin(), DDLDescription::deleteChild<DDLElement>);
        _ddl_elements.clear();

        _ddl_elements = ddl_elements;
        _dynamic_elements = false;
        for (DDLElementIt itEl = _ddl_elements.begin(); itEl != _ddl_elements.end(); itEl++)
        {
            if ((*itEl)->isDynamic())
            {
                _dynamic_elements = true;
                break;
            }
        }
    }

    void DDLComplex::addElement(DDLElement* element, int pos)
    {
        if (NULL != element)
        {
            const DDLElementVec::size_type size_pos = static_cast<DDLElementVec::size_type>(pos);
            if (0 <= size_pos && size_pos < _ddl_elements.size())
            {
                _ddl_elements.insert(_ddl_elements.begin() + size_pos, element);
            }
            else
            {
                _ddl_elements.push_back(element);
            }
            if (!_dynamic_elements)
            {
                _dynamic_elements = element->isDynamic();
            }
        }
    }

    a_util::result::Result DDLComplex::removeElement(const std::string& element_name)
    {
        DDLElementIt it = _ddl_elements.begin();
        while (_ddl_elements.end() != it)
        {
            if ((*it)->getName() == element_name)
            {
                bool itHasDynArr = (*it)->isDynamic();
                DDLDescription::deleteChild(*it);
                _ddl_elements.erase(it);
                if (_dynamic_elements && itHasDynArr)
                {
                    // We have to verify if the structure was dynamic only because of this element
                    _dynamic_elements = false;
                    for (DDLElementIt itEl = _ddl_elements.begin(); itEl != _ddl_elements.end(); itEl++)
                    {
                        if ((*itEl)->isDynamic())
                        {
                            _dynamic_elements = true;
                            break;
                        }
                    }
                }
                return a_util::result::SUCCESS;
            }
            it++;
        }
        return ERR_NOT_FOUND;
    }

    const DDLElementVec& DDLComplex::getElements() const
    {
        return _ddl_elements;
    }

    DDLElementVec& DDLComplex::getElements()
    {
        return _ddl_elements;
    }

    bool DDLComplex::hasDynamicElements()
    {
        return _dynamic_elements;
    }

    void swap(DDLComplex& lhs, DDLComplex& rhs) noexcept
    {
        using std::swap; // ADL
        swap(lhs._language_version, rhs._language_version);
        swap(lhs._name, rhs._name);
        swap(lhs._version, rhs._version);
        swap(lhs._comment, rhs._comment);
        swap(lhs._alignment, rhs._alignment);
        swap(lhs._ddl_elements, rhs._ddl_elements);
        swap(lhs._init_flag, rhs._init_flag);
        swap(lhs._level, rhs._level);
        swap(lhs._dynamic_elements, rhs._dynamic_elements);
    }
}   // namespace ddl
