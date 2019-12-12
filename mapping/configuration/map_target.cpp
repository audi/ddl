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

#include "map_target.h"

#include <algorithm>
#include "a_util/result/error_def.h"
#include "legacy_error_macros.h"

#include "map_configuration.h"

namespace mapping
{
namespace oo
{
    //define all needed error types and values locally
    _MAKE_RESULT(-5, ERR_INVALID_ARG)
    _MAKE_RESULT(-19, ERR_NOT_SUPPORTED)
    _MAKE_RESULT(-20, ERR_NOT_FOUND)
    _MAKE_RESULT(-40, ERR_INVALID_STATE)
}
}

using namespace mapping::oo;

MapTarget::MapTarget(MapConfiguration* pConfig) : _config(pConfig), _is_valid(true)
{
}

MapTarget::MapTarget(MapConfiguration* pConfig, std::string name, std::string strType) : 
 _config(pConfig), _is_valid(true)
{
    _name = name;
    _type = strType;
}

MapTarget::MapTarget(const MapTarget& oOther)
{
    _name = oOther._name;
    _type = oOther._type;
    _assignments = oOther._assignments;
    _sources = oOther._sources;
    _is_valid = oOther._is_valid;
    _config = oOther._config;
    for (MapTriggerList::const_iterator it = oOther._triggers.begin();
        it != oOther._triggers.end(); ++it)
    {
        _triggers.push_back((*it)->clone());
    }
}

MapTarget& MapTarget::operator=(const MapTarget& oOther)
{
    MapTarget oCopy(oOther);
    oCopy.swap(*this);
    return *this;
}

MapTarget::~MapTarget()
{
    for (MapTriggerList::iterator it = _triggers.begin();
        it != _triggers.end(); ++it)
    {
        delete *it;
    }
    _triggers.clear();
}

const std::string& MapTarget::getName() const
{
    return _name;
}

const std::string& MapTarget::getType() const
{
    return _type;
}

bool MapTarget::isValid() const
{
    return _is_valid;
}

const MapAssignmentList& MapTarget::getAssignmentList() const
{
    return _assignments;
}


a_util::result::Result MapTarget::addAssignment(const MapAssignment& oAssignment)
{
    if(!_config)
    {
        return ERR_INVALID_STATE;
    }
    RETURN_IF_FAILED(_config->resetErrors());  

    if(!_is_valid)
    {
        _config->appendError(a_util::strings::format("Target Signal %s is not valid, cannot add any assignment", _name.c_str()));
        return ERR_INVALID_STATE;
    }

    RETURN_IF_FAILED(_config->checkAssignmentReferences(oAssignment));
    RETURN_IF_FAILED(addAssignmentNoTypeCheck(oAssignment));

    // Check new assignment with ddl
    const ddl::DDLComplex* pTargetStruct = _config->_ddl_ref->getStructByName(_type);
    // Target is valid, so the type is defined and can be dereferenced
    a_util::result::Result nRes = _config->checkAssignmentType(_name, *pTargetStruct, oAssignment);
    if(isOk(nRes))
    {
        nRes = checkDoubleAssignments();
    }
    // If assignment not consistent with DDL, remove assignment
    if(isFailed(nRes))
    {
        removeAssignmentWithoutClear(oAssignment.getTo());
    }

    return nRes;
}

a_util::result::Result MapTarget::removeAssignment(const std::string& strElementName)
{
    if(!_config)
    {
        return ERR_INVALID_STATE;
    }
    _config->_errors.clear(); 
    return removeAssignmentWithoutClear(strElementName);
}

a_util::result::Result MapTarget::addTrigger(MapTriggerBase* pTrigger)
{
    if(!_config)
    {
        return ERR_INVALID_STATE;
    }
    RETURN_IF_FAILED(_config->resetErrors());

    if(!_is_valid)
    {
        _config->appendError(a_util::strings::format("Target Signal %s is not valid, cannot add any assignment", _name.c_str()));
        return ERR_INVALID_STATE;
    }

    if(pTrigger->_config != _config)
    {
        _config->appendError(a_util::strings::format("Trigger refers to another configuration"));
        return ERR_INVALID_STATE;
    }

    RETURN_IF_FAILED(pTrigger->checkTriggerReferences());
    RETURN_IF_FAILED(_config->checkTriggerType(pTrigger));
    return addTriggerNoTypeCheck(pTrigger);
}

a_util::result::Result MapTarget::removeTrigger(MapTriggerBase* pTrigger)
{
    if(!_config)
    {
        return ERR_INVALID_STATE;
    }
    _config->_errors.clear();
    bool bIsRemoved = false;
    std::string strSource;
    for(MapTriggerList::iterator it = _triggers.begin(); it != _triggers.end(); it++)
    {
        if((*it)->isEqual(*pTrigger))
        {
            strSource = (*it)->getSourceDependency();
            _triggers.erase(it);
            bIsRemoved = true;
            break;
        }
    }
    if(!strSource.empty())
    {
        bool bIsSourceUsed = false;

        for(MapAssignmentList::const_iterator it = _assignments.begin(); it != _assignments.end(); it++)
        {
            if(it->getSource() == strSource)
            {
                bIsSourceUsed = true;
            }
        }
        for(MapTriggerList::const_iterator it = _triggers.begin(); it != _triggers.end(); it++)
        {
            if((*it)->getSourceDependency() == strSource)
            {
                bIsSourceUsed = true;
            }
        }
        if(!bIsSourceUsed)
        {
            for (MapSourceNameList::iterator itSource = _sources.begin();
                itSource != _sources.end(); ++itSource)
            {
                if (*itSource == strSource)
                {
                    _sources.erase(itSource);
                    break;
                }
            }
        }
    }
    if(bIsRemoved)
    {
        return a_util::result::SUCCESS;
    }
    return ERR_NOT_FOUND;
}

a_util::result::Result MapTarget::removeAssignmentWithoutClear(const std::string& strElementName)
{
    bool bIsRemoved = false;
    std::string strSource;
    for(MapAssignmentList::iterator it = _assignments.begin(); it != _assignments.end(); it++)
    {
        if(it->getTo() == strElementName)
        {
            strSource = it->getSource();
            _assignments.erase(it);
            bIsRemoved = true;
            break;
        }
    }
    if(!strSource.empty())
    {
        bool isSourceUsed = false;

        for(MapAssignmentList::const_iterator it = _assignments.begin(); it != _assignments.end(); it++)
        {
            if(it->getSource() == strSource)
            {
                isSourceUsed = true;
            }
        }
        for(MapTriggerList::const_iterator it = _triggers.begin(); it != _triggers.end(); it++)
        {
            if((*it)->getSourceDependency() == strSource)
            {
                isSourceUsed = true;
            }
        }
        if(!isSourceUsed)
        {
            for (MapSourceNameList::iterator itSource = _sources.begin();
                itSource != _sources.end(); ++itSource)
            {
                if (*itSource == strSource)
                {
                    _sources.erase(itSource);
                    break;
                }
            }
        }
    }
    if(bIsRemoved)
    {
        return a_util::result::SUCCESS;
    }
    return ERR_NOT_FOUND;
}

a_util::result::Result MapTarget::addAssignmentNoTypeCheck(const MapAssignment& oAssignment)
{
    // Assignment is attributed to an element
    const std::string& strTo = oAssignment.getTo();
    if(strTo.empty())
    {
        _config->appendError(a_util::strings::format("Target element name is empty in <target> '%s'", 
            _name.c_str()));
        return ERR_INVALID_ARG;
    }

    for(MapAssignmentList::iterator itAssign = _assignments.begin(); itAssign != _assignments.end(); itAssign++)
    {
        // Is To already assigned
        if (itAssign->getTo() == strTo)
        {
            _config->appendError(
                a_util::strings::format("Target element '%s' is already assigned in <target> '%s'",
                strTo.c_str(), getName().c_str()));
            return ERR_INVALID_ARG;
        }
    }

    // Assignment is a constant, a connection or a function
    if (oAssignment.getSource().empty() && oAssignment.getConstant().empty() && oAssignment.getFunction().empty())
    {
        _config->appendError(a_util::strings::format("Missing <from>, <constant> or <function> attribute for <assignment> to '%s' in <target> '%s'", 
            strTo.c_str(), _name.c_str()));
        return ERR_INVALID_ARG;
    }

    // The constant is numeric
    if (!oAssignment.getConstant().empty() &&
        !a_util::strings::isDouble(oAssignment.getConstant()))
    {
        _config->appendError(
            a_util::strings::format("Empty or non-numeric constant for <assignment> to '%s' in <target> '%s'",
            strTo.c_str(), _name.c_str()));
        return ERR_INVALID_ARG;
    }

    // The trigger_counter function modulo is numeric
    if (!oAssignment.getModulo().empty())
    {
        if (!a_util::strings::isInt64(oAssignment.getModulo()) || 
            oAssignment.getModulo().find('-') != std::string::npos ||
            oAssignment.getModulo().find('+') != std::string::npos ||
            a_util::strings::toUInt64(oAssignment.getModulo()) == 0)
        {
            _config->appendError(
                a_util::strings::format("Non-integer or zero modulo in transmission_counter for <assignment> to '%s' in <target> '%s'",
                strTo.c_str(), _name.c_str()));
            return ERR_INVALID_ARG;
        }
    }

    _assignments.push_back(oAssignment);
    if (!oAssignment.getSource().empty())
    {
        for (MapSourceList::const_iterator itSource = _config->getSourceList().begin();
            itSource != _config->getSourceList().end(); ++itSource)
        {
            if (itSource->getName() == oAssignment.getSource())
            {
                _sources.insert(itSource->getName());
                break;
            }
        }
    }
    return a_util::result::SUCCESS;
}

a_util::result::Result MapTarget::addTriggerNoTypeCheck(MapTriggerBase* pTrigger)
{
    _triggers.push_back(pTrigger);

    // append dependency to references sources
    std::string strSourceDep = pTrigger->getSourceDependency();
    if (!strSourceDep.empty())
    {
        for (MapSourceList::const_iterator itSource = _config->getSourceList().begin();
            itSource != _config->getSourceList().end(); ++itSource)
        {
            if (itSource->getName() == strSourceDep)
            {
                _sources.insert(itSource->getName());
                break;
            }
        }
    }
    return a_util::result::SUCCESS;
}

a_util::result::Result MapTarget::modifySourceName(const std::string& name, const std::string& strNewName)
{
    _sources.erase(name);
    _sources.insert(strNewName);
    for(MapAssignmentList::iterator itAssign = _assignments.begin(); 
        itAssign != _assignments.end(); itAssign++)
    {
        if(itAssign->_source == name)
        {
            itAssign->_source = strNewName;
        }
    }
    for(MapTriggerList::iterator itTrigger = _triggers.begin(); 
        itTrigger != _triggers.end(); itTrigger++)
    {
        if((*itTrigger)->getSourceDependency() == name)
        {
            (*itTrigger)->setSourceDependency(strNewName);
        }
    }
    return a_util::result::SUCCESS;
}

const MapTriggerList& MapTarget::getTriggerList() const
{
    return _triggers;
}

a_util::result::Result MapTarget::loadFromDOM(const a_util::xml::DOMElement& oTarget)
{
    _sources.clear();
    _assignments.clear();
    _triggers.clear();

    // parse attributes
    const a_util::xml::DOMAttributes mapAttrs = oTarget.getAttributes();
    a_util::xml::DOMAttributes::const_iterator itName = mapAttrs.find("name");
    if (itName == mapAttrs.end() || itName->second.empty())
    {
        _config->appendError("Missing name attribute for a <target>");
        return ERR_INVALID_ARG;
    }
    _name = itName->second;
    a_util::strings::trim(_name);

    a_util::xml::DOMAttributes::const_iterator itType = mapAttrs.find("type");
    if (itType == mapAttrs.end() || itType->second.empty())
    {
        _config->appendError(a_util::strings::format("Missing type attribute for <target> '%s'", 
            itName->second.c_str()));
        return ERR_INVALID_ARG;
    }
    _type = itType->second;
    a_util::strings::trim(_type);

    // parse assignments and triggers
    const a_util::xml::DOMElementList lstElements = oTarget.getChildren();
    a_util::result::Result nResult = a_util::result::SUCCESS;
    for (a_util::xml::DOMElementList::const_iterator it = lstElements.begin();
        it != lstElements.end(); ++it)
    {
        const a_util::xml::DOMElement& oElem = *it;
        const std::string& name = oElem.getName();

        if (name == "assignment")
        {
            MapAssignment oAssign;
            a_util::result::Result nRes = oAssign.loadFromDOM(oElem, _config->_errors);
            if(isOk(nRes))
            {
                nRes = addAssignmentNoTypeCheck(oAssign);
            }
            if (isFailed(nRes))
            {
                nResult = nRes;
            }                           
        }
        else if (name == "trigger")
        {
            MapTriggerBase* pTrigger = NULL;
            a_util::result::Result nRes = MapTriggerBase::createFromDOM(_config,
                oElem, pTrigger);
            if(isOk(nRes))
            {
                nRes = addTriggerNoTypeCheck(pTrigger);
            }
            if (isFailed(nRes))
            {
                nResult = nRes;
            }                     
        }
        else
        {
            _config->appendError(a_util::strings::format("Invalid element type in <target> '%s': '%s'", 
                itName->second.c_str(), name.c_str()));
            nResult = ERR_INVALID_ARG;
        }
    }

    return nResult;
}

a_util::result::Result MapTarget::writeToDOM(a_util::xml::DOMElement& oDOMElement) const
{
    oDOMElement.setName("target");
    oDOMElement.setAttribute("name", _name);
    oDOMElement.setAttribute("type", _type);

    for(MapAssignmentList::const_iterator itAssign = _assignments.begin();
        itAssign != _assignments.end(); itAssign++)
    {
        a_util::xml::DOMElement oDOMAssign = oDOMElement.createChild("assignment");
        itAssign->writeToDOM(oDOMAssign);
    }

    for(MapTriggerList::const_iterator itTrigger = _triggers.begin();
        itTrigger != _triggers.end(); itTrigger++)
    {
        a_util::xml::DOMElement oDOMTrigger = oDOMElement.createChild("trigger");
        (*itTrigger)->writeToDOM(oDOMTrigger);
    }

    return a_util::result::SUCCESS;
}

a_util::result::Result MapTarget::checkDoubleAssignments()
{
    a_util::result::Result nRes = a_util::result::SUCCESS;    
    std::set<std::string> oAssignedElList;
    for(MapAssignmentList::iterator itAssign = _assignments.begin(); itAssign != _assignments.end(); itAssign++)
    {
        oAssignedElList.insert(itAssign->getTo());
    }
    for(MapAssignmentList::iterator itAssign = _assignments.begin(); itAssign != _assignments.end(); itAssign++)
    {
        for(std::set<std::string>::iterator itParent = oAssignedElList.begin(); itParent != oAssignedElList.end(); itParent++)
        {
            std::string strEl = itAssign->getTo();
            if(strEl.find(*itParent) == 0)
            {
                size_t pos = 0;
                while ((pos = strEl.find(*itParent, pos)) != std::string::npos)
                {
                    strEl.replace(pos, itParent->length(), std::string());
                    pos += 1;
                }

                if(strEl.find('.') == 0 ||strEl.find('[') == 0)
                {
                    _config->appendError(a_util::strings::format(
                        "<assignment> to '%s' not possible because of <assignment> to %s in <target> '%s'",
                        strEl.c_str(), itParent->c_str(), getName().c_str()));
                    itAssign->_is_valid = false;
                    nRes = ERR_NOT_SUPPORTED;
                }
            }
        }
    }
    return nRes;
}

void MapTarget::swap(MapTarget& oOther)
{
    using std::swap;
    swap(_config, oOther._config);
    swap(_name, oOther._name);
    swap(_type, oOther._type);
    swap(_assignments, oOther._assignments);
    swap(_sources, oOther._sources);
    swap(_triggers, oOther._triggers);
}

const MapSourceNameList& MapTarget::getReferencedSources() const
{
    return _sources;
}

bool mapping::oo::operator==(const MapTarget& a, const MapTarget& b)
{
    if (a.getName() != b.getName() ||
        a.getType() != b.getType())
    {
        return false;
    }

    const MapAssignmentList& lstAssignA = a.getAssignmentList();
    const MapAssignmentList& lstAssignB = b.getAssignmentList();
    
    if (lstAssignA.size() != lstAssignB.size() ||
        !std::equal(lstAssignA.begin(), lstAssignA.end(), lstAssignB.begin()))
    {
        return false;
    }

    const MapTriggerList& lstTriggerA = a.getTriggerList();
    const MapTriggerList& lstTriggerB = b.getTriggerList();
    if (lstTriggerA.size() != lstTriggerB.size())
    {
        return false;
    }

    MapTriggerList::const_iterator itA = lstTriggerA.begin();
    MapTriggerList::const_iterator itB = lstTriggerB.begin();
    for (; itA != lstTriggerA.end() && itB != lstTriggerB.end();
        ++itA, ++itB)
    {
            if (!(*itA)->isEqual(*(*itB)))
            {
                return false;
            }
    }

    return true;
}


a_util::result::Result MapTarget::setName(const std::string& strNewName)
{
    if(!_config)
    {
        return ERR_INVALID_STATE;
    }
    RETURN_IF_FAILED(_config->resetErrors());
    RETURN_IF_FAILED(_config->checkSignalName(strNewName));
    _name = strNewName;

    return _config->checkMappingConsistency();
}

a_util::result::Result MapTarget::setType(const std::string& strType)
{
    if(!_config)
    {
        return ERR_INVALID_STATE;
    }
    RETURN_IF_FAILED(_config->resetErrors());
    RETURN_IF_FAILED(_config->checkSignalType(strType));
    _type = strType;
    return _config->checkDDLConsistency();
}
