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

#include "map_configuration.h"

#include <algorithm>
#include "a_util/result/error_def.h"
#include "legacy_error_macros.h"

#include "ddlrepresentation/ddlelement.h"
#include "ddlrepresentation/ddlcomplex.h"
#include "ddlrepresentation/ddldatatype.h"

#include "mapping/ddl_helper.h"


namespace mapping
{
namespace oo
{
    //define all needed error types and values locally
    _MAKE_RESULT(-5, ERR_INVALID_ARG)
    _MAKE_RESULT(-9, ERR_INVALID_FLAGS)
    _MAKE_RESULT(-11, ERR_INVALID_FILE)
    _MAKE_RESULT(-19, ERR_NOT_SUPPORTED)
    _MAKE_RESULT(-20, ERR_NOT_FOUND)
    _MAKE_RESULT(-38, ERR_FAILED)
    _MAKE_RESULT(-40, ERR_INVALID_STATE)
    _MAKE_RESULT(-42, ERR_INVALID_TYPE)
}
}

using namespace mapping::oo;

MapConfiguration::MapConfiguration()
    : _ddl_ref(NULL)
    , _checked_for_consistency(false)
    , _is_consistent(false)
{
}

MapConfiguration::MapConfiguration(const ddl::DDLDescription* pDDL)
    : _ddl_ref(pDDL)
    , _checked_for_consistency(false)
    , _is_consistent(false)
{
}

MapConfiguration::~MapConfiguration()
{
    reset();
}

MapConfiguration::MapConfiguration(const MapConfiguration& oOther)
{
    _ddl_ref = oOther._ddl_ref;
    _header = oOther._header;
    _sources = oOther._sources;
    _targets = oOther._targets;
    _checked_for_consistency = oOther._checked_for_consistency;
    _is_consistent = oOther._is_consistent;

    for (MapTransformationList::const_iterator it = oOther._transforms.begin();
        it != oOther._transforms.end(); ++it)
    {
        _transforms.push_back((*it)->clone());
    }
    
    repairConfigReferences(*this);
}

MapConfiguration& MapConfiguration::operator=(const MapConfiguration& oOther)
{
    MapConfiguration oCopy(oOther);
    oCopy.swap(*this);

    return *this;
}

void MapConfiguration::reset()
{
    _errors.clear();
    _header.reset();
    _sources.clear();
    _targets.clear();
    _checked_for_consistency = false;
    _is_consistent = false;
    
    for (MapTransformationList::iterator it = _transforms.begin();
        it != _transforms.end(); ++it)
    {
        delete *it;
    }
    
    _transforms.clear();
}

a_util::result::Result MapConfiguration::loadFromFile(const std::string& strFilePath,
    uint32_t ui32Flags)
{
    a_util::xml::DOM oDom;
    if(!oDom.load(strFilePath))
    {
        appendError(oDom.getLastError());
        return ERR_INVALID_FILE;
    }
    return loadFromDOM(oDom, ui32Flags);
}

a_util::result::Result MapConfiguration::loadFromDOM(a_util::xml::DOM& oDom, uint32_t ui32Flags)
{

    bool bLoad = (ui32Flags & mc_load_mapping) == mc_load_mapping;
    bool bMerge = (ui32Flags & mc_merge_mapping) == mc_merge_mapping;
    // only one of them supported
    if (!(bLoad | bMerge))
    {
        return ERR_NOT_SUPPORTED;
    }

    // Clear list and verify ddl description exists
    resetErrors();

    // we parse into a temporary config to be able to check for validity before
    // overwriting/resetting the current configuration
    MapConfiguration oTmp(_ddl_ref);
    
    if (isFailed(MapConfiguration::loadMappingFromDOM(oDom, oTmp)))
    {
        // preserve errors
        std::swap(_errors, oTmp._errors);
        return ERR_FAILED;
    }

    if (bLoad)
    {
        // check for consistency
        if (isFailed(oTmp.checkMappingConsistency()))
        {
            // preserve errors
            std::swap(_errors, oTmp._errors);
            return ERR_FAILED;
        }   
        if (isFailed(oTmp.checkDDLConsistency()))
        {
            // preserve errors
            std::swap(_errors, oTmp._errors);
            return ERR_INVALID_TYPE;
        }   
        // the config should replace the current one -> swap
        swap(oTmp);
    }
    else if (bMerge)
    {
        // since we allow partial mapping configurations,
        // we need to merge before checking consistency
        MapConfiguration oCopy(*this);
        a_util::result::Result nResult = oCopy.merge(oTmp);
        if (isOk(nResult))
        {
            nResult = oCopy.checkMappingConsistency();
        }
        if (isFailed(nResult))
        {
            // preserve errors
            std::swap(_errors, oCopy._errors);
            return ERR_FAILED;
        }
        if (isFailed(oCopy.checkDDLConsistency()))
        {
            // preserve errors
            std::swap(_errors, oCopy._errors);
            return ERR_INVALID_TYPE;
        }
        // all ok? now we can swap
        swap(oCopy);
    }

    return a_util::result::SUCCESS;
}

a_util::result::Result MapConfiguration::loadFromDOMWithoutDDLConsistency(a_util::xml::DOM& oDom,
    uint32_t ui32Flags)
{
    bool bLoad = (ui32Flags & mc_load_mapping) == mc_load_mapping;
    bool bMerge = (ui32Flags & mc_merge_mapping) == mc_merge_mapping;
    // only one of them supported
    if (!(bLoad | bMerge))
    {
        return ERR_NOT_SUPPORTED;
    }

    // Clear list
    _errors.clear();

    // we parse into a temporary config to be able to check for validity before
    // overwriting/resetting the current configuration
    MapConfiguration oTmp(_ddl_ref);

    if (isFailed(MapConfiguration::loadMappingFromDOM(oDom, oTmp)))
    {
        // preserve errors
        std::swap(_errors, oTmp._errors);
        return ERR_FAILED;
    }

    if (bLoad)
    {
        // check for consistency
        if (isFailed(oTmp.checkMappingConsistency()))
        {
            // preserve errors
            std::swap(_errors, oTmp._errors);
            return ERR_FAILED;
        }
        // the config should replace the current one -> swap
        swap(oTmp);
    }
    else if (bMerge)
    {
        // since we allow partial mapping configurations,
        // we need to merge before checking consistency
        MapConfiguration oCopy(*this);
        a_util::result::Result nResult = oCopy.merge(oTmp);
        if (isOk(nResult))
        {
            nResult = oCopy.checkMappingConsistency();
        }
        if (isFailed(nResult))
        {
            // preserve errors
            std::swap(_errors, oCopy._errors);
            return ERR_FAILED;
        }
        // all ok? now we can swap
        swap(oCopy);
    }

    return a_util::result::SUCCESS;
}

a_util::result::Result MapConfiguration::loadPartiallyFromFile(const std::string& strFilePath,
    uint32_t ui32Flags)
{
    a_util::xml::DOM oDom;
    if(!oDom.load(strFilePath))
    {
        appendError(oDom.getLastError());
        return ERR_INVALID_FILE;
    }
    
    return loadPartiallyFromDOM(oDom, ui32Flags);
}

a_util::result::Result MapConfiguration::loadPartiallyFromDOM(a_util::xml::DOM& oDom,
    uint32_t ui32Flags)
{
    _errors.clear();

    bool bLoad = (ui32Flags & mc_load_mapping) == mc_load_mapping;
    bool bMerge = (ui32Flags & mc_merge_mapping) == mc_merge_mapping;
    // only one of them supported
    if (!(bLoad | bMerge))
    {
        return ERR_INVALID_FLAGS;
    }

    // we parse into a temporary config to be able to check for validity before
    // overwriting/resetting the current configuration
    MapConfiguration oTmp(_ddl_ref);
    // IF is failed, map-file is corrupted
    if (isFailed(MapConfiguration::loadMappingFromDOM(oDom, oTmp)))
    {
        // preserve errors
        std::swap(_errors, oTmp._errors);
        return ERR_FAILED;
    }

    a_util::result::Result nResult = a_util::result::SUCCESS;
    if (bLoad)
    {
        // check for consistency
        if (isFailed(oTmp.checkMappingConsistency()))
        {
            std::swap(_errors, oTmp._errors);
            return ERR_FAILED;
        }
        //Check DDL consistency only to set the validity booleans
        nResult = oTmp.checkDDLConsistency();
        // the config should replace the current one -> swap
        swap(oTmp);

    }
    else if (bMerge)
    {
        // since we allow partial mapping configurations,
        // we need to merge before checking consistency
        MapConfiguration oCopy(*this);
        nResult = oCopy.merge(oTmp);
        if (isOk(nResult))
        {
            nResult = oCopy.checkMappingConsistency();
        }
        if (isFailed(nResult))
        {
            // preserve errors
            std::swap(_errors, oCopy._errors);
            return ERR_FAILED;
        }
        //Check DDL consistency only to set the validity booleans
        nResult = oCopy.checkDDLConsistency();
        // all ok? now we can swap
        swap(oCopy);
    }

    return nResult;
}

a_util::result::Result MapConfiguration::writeToFile(const std::string& strFilePath)
{
    a_util::xml::DOM oDom;
    oDom.fromString("<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>"); 

    a_util::result::Result nRes = writeToDOM(oDom);
    if(isOk(nRes))
    {
        if(!oDom.save(strFilePath))
        {
            appendError(oDom.getLastError());
            nRes = ERR_INVALID_FILE;
        }        
    }    
    return nRes;
}

a_util::result::Result MapConfiguration::writeToDOM(a_util::xml::DOM& oDom)
{
    resetErrors();

    // Verify consistency
    
    RETURN_IF_FAILED(checkMappingConsistency());
    RETURN_IF_FAILED(checkDDLConsistency());

    oDom.reset();
    a_util::xml::DOMElement oRoot = oDom.getRoot();

    // set <header>
    a_util::xml::DOMElement oHeader = oRoot.createChild("header");
    RETURN_IF_FAILED(_header.writeToDOM(oHeader));

    // set <sources>
    a_util::xml::DOMElement oSourcesElement = oRoot.createChild("sources");
    for(MapSourceList::const_iterator itSrc = _sources.begin(); itSrc != _sources.end(); itSrc++)
    {
        a_util::xml::DOMElement oSourceElement = oSourcesElement.createChild("source");
        RETURN_IF_FAILED(itSrc->writeToDOM(oSourceElement));
    }
    
    // set <targets>
    a_util::xml::DOMElement oTargetsElement = oRoot.createChild("targets");
    for(MapTargetList::const_iterator itTrg = _targets.begin(); itTrg != _targets.end(); itTrg++)
    {
        a_util::xml::DOMElement oTargetElement = oTargetsElement.createChild("target");
        RETURN_IF_FAILED(itTrg->writeToDOM(oTargetElement));
    }

    // set <transformations>
    a_util::xml::DOMElement oTransformationsElement = oRoot.createChild("transformations");
    for(MapTransformationList::const_iterator itTransform = _transforms.begin(); itTransform != _transforms.end(); itTransform++)
    {
        a_util::xml::DOMElement oTransformationElement = oTransformationsElement.createChild("");
        RETURN_IF_FAILED((*itTransform)->writeToDOM(oTransformationElement));
    }    

    // the root <mapping> element
    oRoot.setName("mapping");
    return a_util::result::SUCCESS;
}

const MapHeader& MapConfiguration::getHeader() const
{
    return _header;
}

a_util::result::Result MapConfiguration::setHeaderDescription(const std::string& desc)
{
    return _header.setDescription(desc);
}

a_util::result::Result MapConfiguration::setHeaderDdlPaths(const std::string& strDdl)
{
    return _header.setDdlPaths(strDdl);
}

const std::string& MapConfiguration::getHeaderDdlPaths() const
{
    return _header.getDllPaths();
}

const MapTransformationList& MapConfiguration::getTransformationList() const
{
    return _transforms;
}

const MapTransformationBase* MapConfiguration::
    getTransformation(const std::string& strTransformationName) const
{
    for (MapTransformationList::const_iterator it = _transforms.begin();
        it != _transforms.end(); ++it)
    {
        if ((*it)->getName() == strTransformationName)
        {
            return *it;
        }
    }

    return NULL;
}

MapTransformationBase* MapConfiguration::getTransformation(const std::string& strTransformationName)
{
    for (MapTransformationList::iterator it = _transforms.begin();
        it != _transforms.end(); ++it)
    {
        if ((*it)->getName() == strTransformationName)
        {
            return *it;
        }
    }
    return NULL;
}


a_util::result::Result MapConfiguration::addTransformation(const std::string& strTransformationName, const std::string& strTransformationType)
{
    if(getTransformation(strTransformationName))
    {
        appendError("A transformation with this name already exists");
        return ERR_INVALID_ARG;
    }

    MapTransformationBase* pTrans = NULL;
    RETURN_IF_FAILED(MapTransformationBase::create(this, strTransformationName, strTransformationType, pTrans));
    _transforms.push_back(pTrans);
    return a_util::result::SUCCESS;
}

a_util::result::Result MapConfiguration::removeTransformation(const std::string& strTransformationName)
{
    _errors.clear();
    if(strTransformationName.empty())
    {
        return a_util::result::SUCCESS;
    }
    for(MapTransformationList::iterator itTrf = _transforms.begin(); itTrf != _transforms.end(); itTrf++)
    {
        if((*itTrf)->getName() == strTransformationName)
        {
            // Remove all assignments that use this transformation
            for (MapTargetList::iterator it = _targets.begin();
                it != _targets.end(); ++it)
            {
                // Copy from list to remove assignments form original list 
                // while iterating through the copy
                MapAssignmentList oAssignList = it->getAssignmentList();
                for (MapAssignmentList::iterator itA = oAssignList.begin();
                    itA != oAssignList.end(); ++itA)
                {
                    if(itA->getTransformation() == strTransformationName)
                    {
                        RETURN_IF_FAILED(it->removeAssignmentWithoutClear(itA->getTo()));
                    }
                }
            }
            delete *itTrf;
            _transforms.erase(itTrf);
            return a_util::result::SUCCESS;
        }
    }
    return ERR_NOT_FOUND;
}


const MapSource* MapConfiguration::getSource(const std::string& strSourceName) const
{
    for (MapSourceList::const_iterator it = _sources.begin();
        it != _sources.end(); ++it)
    {
        if (it->getName() == strSourceName)
        {
            return &(*it);
        }
    }
    
    return NULL;
}

MapSource* MapConfiguration::getSource(const std::string& strSourceName)
{
    for (MapSourceList::iterator it = _sources.begin();
        it != _sources.end(); ++it)
    {
        if (it->getName() == strSourceName)
        {
            return &(*it);
        }
    }

    return NULL;
}

a_util::result::Result MapConfiguration::addSource(const std::string& name, const std::string& strType)
{
    RETURN_IF_FAILED(resetErrors());    
    RETURN_IF_FAILED(checkSignalType(strType));
    MapSource oSource(this, name, strType);    
    return addSource(oSource);
}

a_util::result::Result MapConfiguration::removeSource(const std::string& name)
{
    _errors.clear();
    for (MapTargetList::iterator it = _targets.begin();
        it != _targets.end(); ++it)
    {
        // Copy from list to remove assignments form original list 
        // while iterating through the copy
        MapAssignmentList oAssignList = it->getAssignmentList();
        for (MapAssignmentList::iterator itA = oAssignList.begin();
            itA != oAssignList.end(); ++itA)
        {
            if(itA->getSource() == name)
            {
                RETURN_IF_FAILED(it->removeAssignmentWithoutClear(itA->getTo()));
            }
        }
        // Copy from list to remove triggers form original list 
        // while iterating through the copy
        MapTriggerList oTriggerList = it->getTriggerList();
        for (MapTriggerList::iterator itTrigger = oTriggerList.begin();
            itTrigger != oTriggerList.end(); ++itTrigger)
        {
            if((*itTrigger)->getSourceDependency() == name)
            {
                RETURN_IF_FAILED(it->removeTrigger(*itTrigger));
            }
        }
    }
    for (MapSourceList::iterator it = _sources.begin();
        it != _sources.end(); ++it)
    {
        if (it->getName() == name)
        {
            _sources.erase(it);
            return a_util::result::SUCCESS;
        }
    }
    return ERR_NOT_FOUND;
}

const MapSourceList& MapConfiguration::getSourceList() const
{
    return _sources;
}

const MapTarget* MapConfiguration::getTarget(const std::string& strTargetName) const
{
    for (MapTargetList::const_iterator it = _targets.begin();
        it != _targets.end(); ++it)
    {
        if (it->getName() == strTargetName)
        {
            return &(*it);
        }
    }
    
    return NULL;
}

MapTarget* MapConfiguration::getTarget(const std::string& strTargetName)
{
    for (MapTargetList::iterator it = _targets.begin();
        it != _targets.end(); ++it)
    {
        if (it->getName() == strTargetName)
        {
            return &(*it);
        }
    }

    return NULL;
}

a_util::result::Result MapConfiguration::addTarget(const std::string& name, const std::string& strType)
{
    RETURN_IF_FAILED(resetErrors());   
    RETURN_IF_FAILED(checkSignalType(strType));
    MapTarget oTarget(this, name, strType);    
    return addTarget(oTarget);
}

a_util::result::Result MapConfiguration::removeTarget(const std::string& _name)
{
    _errors.clear();
    for (MapTargetList::iterator it = _targets.begin();
        it != _targets.end(); ++it)
    {
        if (it->getName() == _name)
        {
            _targets.erase(it);
            return a_util::result::SUCCESS;
        }
    }
    return ERR_NOT_FOUND;
}

const MapTargetList& MapConfiguration::getTargetList() const
{
    return _targets;
}

a_util::result::Result MapConfiguration::setDescription(const ddl::DDLDescription* pDDL)
{
    _ddl_ref = pDDL;  
    MapConfiguration oCopy = *this;
    oCopy._ddl_ref = pDDL;
    // reset mapping configuration if we got a ddl that doesn't fit to the current mapping config
    if (isFailed(oCopy.checkDDLConsistency()))
    {
        reset();
    }
    return a_util::result::SUCCESS;
}

a_util::result::Result MapConfiguration::setDescriptionWithoutConsistency(const ddl::DDLDescription* pDDL)
{
    _ddl_ref = pDDL;
    _checked_for_consistency = false;
    _is_consistent = false;
    return a_util::result::SUCCESS;
}

a_util::result::Result MapConfiguration::modifyDescription(const ddl::DDLDescription* pDDL)
{
    _ddl_ref = pDDL;
    _errors.clear();
    return checkDDLConsistency();
}

const ddl::DDLDescription* MapConfiguration::getDescription() const
{
    return _ddl_ref;
}

const MapErrorList& MapConfiguration::getErrorList() const
{
    return _errors;
}

a_util::result::Result MapConfiguration::resetErrors()
{
    _errors.clear();
    if (!_ddl_ref) 
    { 
        appendError("No Description found");
        return ERR_INVALID_STATE; 
    }    
    return a_util::result::SUCCESS;
}

void MapConfiguration::swap(MapConfiguration& oOther)
{
    using std::swap;
    swap(_ddl_ref, oOther._ddl_ref);
    swap(_header, oOther._header);
    swap(_sources, oOther._sources);   
    swap(_targets, oOther._targets);    
    swap(_transforms, oOther._transforms);    
    swap(_errors, oOther._errors);
    repairConfigReferences(*this);
    repairConfigReferences(oOther);
}

void MapConfiguration::repairConfigReferences(MapConfiguration& oConfig)
{
    // Sources
    for(MapSourceList::iterator itSrc = oConfig._sources.begin(); itSrc != oConfig._sources.end(); itSrc++)
    {
        itSrc->_config = &oConfig;
    }
    
    // Targets
    for(MapTargetList::iterator itTrg = oConfig._targets.begin(); itTrg != oConfig._targets.end(); itTrg++)
    {
        itTrg->_config = &oConfig;
        for(MapTriggerList::iterator itTrigger = itTrg->_triggers.begin(); itTrigger != itTrg->_triggers.end(); itTrigger++)
        {
            (*itTrigger)->_config = &oConfig;
        }
    }    
    // Transformations
    for(MapTransformationList::iterator itTrf = oConfig._transforms.begin(); itTrf != oConfig._transforms.end(); itTrf++)
    {
        (*itTrf)->_config = &oConfig;
    }    
}

a_util::result::Result MapConfiguration::addTarget(const MapTarget& oTarget)
{
    RETURN_IF_FAILED(checkSignalName(oTarget.getName()));
    _targets.push_back(oTarget);
    return a_util::result::SUCCESS;
}

a_util::result::Result MapConfiguration::addSource(const MapSource& oSource)
{
    RETURN_IF_FAILED(checkSignalName(oSource.getName()));
    _sources.push_back(oSource);
    return a_util::result::SUCCESS;
}

a_util::result::Result MapConfiguration::merge(const MapConfiguration& oOther)
{
    MapConfiguration oCopy(*this);

    const MapSourceList& lstSources = oOther.getSourceList();
    for (MapSourceList::const_iterator it = lstSources.begin();
        it != lstSources.end(); ++it)
    {
        const MapSource* pSrc = getSource(it->getName());
        if (pSrc)
        {
            if (*pSrc != *it)
            {
                appendError(a_util::strings::format(
                    "Trying to merge conflicting source definition for '%s'",
                    it->getName().c_str()));
                return ERR_INVALID_ARG;
            }
        }
        else
        {            
            RETURN_IF_FAILED(oCopy.addSource(*it));
        }
    }

    const MapTargetList& lstTargets = oOther.getTargetList();
    for (MapTargetList::const_iterator it = lstTargets.begin();
        it != lstTargets.end(); ++it)
    {
        const MapTarget* pTarget = getTarget(it->getName());
        if (pTarget)
        {
            if (*pTarget != *it)
            {
                appendError(a_util::strings::format(
                    "Trying to merge conflicting target definition for '%s'",
                    it->getName().c_str()));
                return ERR_INVALID_ARG;
            }
        }
        else
        {
            RETURN_IF_FAILED(oCopy.addTarget(*it));
        }
    }

    const MapTransformationList& lstTrafos = oOther.getTransformationList();
    for (MapTransformationList::const_iterator it = lstTrafos.begin();
        it != lstTrafos.end(); ++it)
    {
        const MapTransformationBase* pSrc = getTransformation((*it)->getName());
        if (pSrc)
        {
            if (!pSrc->isEqual(*(*it)))
            {
                appendError(a_util::strings::format(
                    "Trying to merge conflicting transformation definition for '%s'",
                    (*it)->getName().c_str()));
                return ERR_INVALID_ARG;
            }
        }
        else
        {
            oCopy._transforms.push_back((*it)->clone());
        }
    }

    swap(oCopy);
    return a_util::result::SUCCESS;
}

a_util::result::Result MapConfiguration::loadMappingFromDOM(a_util::xml::DOM& oDom, MapConfiguration& oTmpConfig)
{
    a_util::result::Result nResult = a_util::result::SUCCESS;

    // the root <mapping> element
    a_util::xml::DOMElement oMapping = oDom.getRoot();
    if (oMapping.getName() != "mapping")
    {
        oTmpConfig.appendError("Missing <mapping> root element");
        return ERR_INVALID_ARG; // no need to continue
    }

    // parse <header>
    a_util::xml::DOMElement oElement = oMapping.getChild("header");
    if (!oElement.isNull())
    {
        nResult = oTmpConfig._header.loadFromDOM(oElement, oTmpConfig._errors);
    }
    else
    {
        oTmpConfig.appendError("Missing <header> element");
        nResult = ERR_INVALID_ARG;
    }

    // parse <sources>
    oElement = oMapping.getChild("sources");
    if (!oElement.isNull())
    {
        const a_util::xml::DOMElementList lstSources = oElement.getChildren();
        for (a_util::xml::DOMElementList::const_iterator it = lstSources.begin();
            it != lstSources.end(); ++it)
        {
            if (it->getName() == "source")
            {
                MapSource oSrc(&oTmpConfig);
                a_util::result::Result nRes = oSrc.loadFromDOM(*it);
                if (isOk(nRes))
                {
                    nRes = oTmpConfig.addSource(oSrc);
                }
                if (isFailed(nRes))
                {
                    nResult = nRes;
                }
            }
        }
    }
    else
    {
        // optional
    }

    // parse <targets>
    oElement = oMapping.getChild("targets");
    if (!oElement.isNull())
    {
        const a_util::xml::DOMElementList lstTargets = oElement.getChildren();
        for (a_util::xml::DOMElementList::const_iterator it = lstTargets.begin();
            it != lstTargets.end(); ++it)
        {
            MapTarget oTarget(&oTmpConfig);
            a_util::result::Result nRes = oTarget.loadFromDOM(*it);
            if (isOk(nRes))
            {
                nRes = oTmpConfig.addTarget(oTarget);
            }
            if (isFailed(nRes))
            {
                nResult = nRes;
            }
        } 
    }
    else
    {
        // optional
    }

    // parse <transformations>
    oElement = oMapping.getChild("transformations");
    if (!oElement.isNull())
    {
        const a_util::xml::DOMElementList lstTrans = oElement.getChildren();
        for (a_util::xml::DOMElementList::const_iterator it = lstTrans.begin();
            it != lstTrans.end(); ++it)
        {
            MapTransformationBase* pTrans = NULL;
            a_util::result::Result nRes = MapTransformationBase::createFromDOM(&oTmpConfig,
                *it, pTrans);
            if(isOk(nRes))
            {
                oTmpConfig._transforms.push_back(pTrans);
            }
            else
            {
                nResult = nRes;
            }
        }
    }
    else
    {
        // optional
    }
    return nResult;
}

a_util::result::Result MapConfiguration::checkMappingConsistency()
{
    a_util::result::Result nResult =a_util::result::SUCCESS;
    // If import was succesful, check if the mapping is consistent
    // All referenced Sources and Transformations must exist in Mapping
    if(isOk(nResult))
    {
        for(MapTargetList::const_iterator itTrg = _targets.begin(); 
            itTrg != _targets.end(); itTrg++)
        {
            for(MapTriggerList::const_iterator itTrigger = itTrg->getTriggerList().begin(); 
                itTrigger != itTrg->getTriggerList().end(); itTrigger++)
            {
                if(isFailed((*itTrigger)->checkTriggerReferences()))
                {
                    nResult = ERR_INVALID_ARG;
                }
            }
            for(MapAssignmentList::const_iterator itAssign = itTrg->getAssignmentList().begin(); 
                itAssign != itTrg->getAssignmentList().end(); itAssign++)
            {
                if(isFailed(checkAssignmentReferences(*itAssign)))
                {
                    nResult = ERR_INVALID_ARG;
                }            
            }
        }
    }
    return nResult;
}

a_util::result::Result MapConfiguration::checkDDLConsistency()
{
    a_util::result::Result nRes = a_util::result::SUCCESS;
    // Sources
    for (MapSourceList::iterator itSource = _sources.begin();
        itSource != _sources.end(); ++itSource)
    {
        if(isOk(checkSignalType(itSource->getType())))
        {
            itSource->_is_valid = true;
        }
        else
        {
            itSource->_is_valid = false;
            nRes = ERR_INVALID_TYPE;
        }
    }

    // Targets
    for (MapTargetList::iterator itTarget = _targets.begin();
        itTarget != _targets.end(); ++itTarget)
    {
        if(isOk(checkSignalType(itTarget->getType())))
        {
            itTarget->_is_valid = true;

            // Check Assignments type  
            const ddl::DDLComplex* pTargetStruct =
                _ddl_ref->getStructByName(itTarget->getType());
            for (MapAssignmentList::iterator itAssign = itTarget->_assignments.begin();
                itAssign != itTarget->_assignments.end(); ++itAssign)
            {            
                // Signal type was checked before, so pTargetStruct can be derefenrenced
                if(isOk(checkAssignmentType(itTarget->getName(), *pTargetStruct, *itAssign)))
                {
                    itAssign->_is_valid = true;
                }
                else
                {
                    itAssign->_is_valid = false;
                    nRes = ERR_INVALID_TYPE;
                }
            }

            // Set overlapping assignments to invalid
            a_util::result::Result nResDouble = itTarget->checkDoubleAssignments();
            if(isFailed(nResDouble))
            {                
                nRes = nResDouble;
            }

            // Triggers
            MapTriggerList& lstTrg = itTarget->_triggers;
            for (MapTriggerList::iterator itTriggers = lstTrg.begin();
                itTriggers != lstTrg.end(); ++itTriggers)
            {
                if(isOk(checkTriggerType(*itTriggers)))
                {
                    (*itTriggers)->_is_valid = true;
                }
                else
                {
                    (*itTriggers)->_is_valid = false;
                    nRes = ERR_INVALID_TYPE;
                }
            }
        }
        else
        {
            itTarget->_is_valid = false;
            nRes = ERR_INVALID_TYPE;
        }
    }

    // Transformations
    for(MapTransformationList::iterator itTrf = _transforms.begin(); itTrf != _transforms.end(); itTrf++)
    {
        a_util::result::Result nResTrf = (*itTrf)->setTypeFromDDL();
        if(isOk(nResTrf))
        {
            (*itTrf)->_is_valid = true;
        }
        else
        {
            (*itTrf)->_is_valid = false;
            nRes = nResTrf;
        }
    }
    if (isOk(nRes))
    {
        _is_consistent = true;
    }
    _checked_for_consistency = true;
    return nRes;
}

a_util::result::Result MapConfiguration::findTypeObject(const std::string& strStruct,
    const std::string& strPath, const ddl::IDDLDataType*& pType,
    int& nArraySize) const
{
    const ddl::DDLComplex* pStruct = _ddl_ref->getStructByName(strStruct);
    if (pStruct)
    {
        return findTypeObject(*pStruct, strPath, pType, nArraySize);
    }

    return ERR_NOT_FOUND;
}

a_util::result::Result MapConfiguration::findTypeObject(const ddl::DDLComplex& oStruct,
    const std::string& strPath, const ddl::IDDLDataType*& pType,
    int& nArraySize) const
{
    const ddl::DDLElement* pElem = NULL;
    bool bIsArrayElement = false;
    if (isFailed(DDLHelper::LookupElement(oStruct, strPath, pElem, bIsArrayElement)))
    {
        return ERR_NOT_FOUND;
    }
    if (pElem->isDynamic())
    {
        return ERR_INVALID_TYPE;
    }

    pType = pElem->getTypeObject();
    nArraySize = pElem->getArraysize();
    if (bIsArrayElement)
    {
        nArraySize = 1;
    }

    return a_util::result::SUCCESS;
}

bool MapConfiguration::checkTypeCompatibility(const ddl::IDDLDataType& oSource,
    int nSourceArrSize, const ddl::IDDLDataType& oTarget, int nTargetArrSize) const
{
    const ddl::DDLComplex* pSourceStruct = dynamic_cast<const ddl::DDLComplex*>(&oSource);
    const ddl::DDLComplex* pTargetStruct = dynamic_cast<const ddl::DDLComplex*>(&oTarget);

    if (pSourceStruct)
    {
        if (!pTargetStruct || pSourceStruct->getName() != pTargetStruct->getName())
        {
            return false;
        }
    }

    if (pTargetStruct && !pSourceStruct)
    {
        return false;
    }

    if (nSourceArrSize != nTargetArrSize)
    {
        return false;
    }

    return true;
}

a_util::result::Result MapConfiguration::checkSignalName(const std::string& name) const
{
    if(name.empty())
    {
        return ERR_INVALID_ARG;
    }
    std::set<std::string> lstSignalName; 
    for(MapSourceList::const_iterator it = _sources.begin(); it != _sources.end(); it++)
    {
        lstSignalName.insert(it->getName());
    }
    for(MapTargetList::const_iterator it = _targets.begin(); it != _targets.end(); it++)
    {
        lstSignalName.insert(it->getName());
    }
    if(lstSignalName.find(name) != lstSignalName.end())
    {
        appendError(a_util::strings::format("A signal with the name '%s' already exists",
            name.c_str()));
        return ERR_INVALID_ARG;
    }  
    return a_util::result::SUCCESS;
}

a_util::result::Result MapConfiguration::checkSignalType(const std::string& strType) const
{
    a_util::result::Result nRes= a_util::result::SUCCESS;    
    // Is the type defined in DDL
    if (!_ddl_ref||!_ddl_ref->getStructByName(strType))
    {
        appendError(
            a_util::strings::format("Unknown type '%s'",
            strType.c_str()));
        nRes = ERR_INVALID_TYPE;
    }
    return nRes;
}

a_util::result::Result MapConfiguration::checkAssignmentReferences(const MapAssignment& oAssignment) const
{    
    a_util::result::Result nRes = a_util::result::SUCCESS;
    // Is the transformation known?
    if (!oAssignment.getTransformation().empty() &&
        !getTransformation(oAssignment.getTransformation()))

    {
        appendError(a_util::strings::format(
            "Unknown <transformation> '%s' in <assignment> '%s'",
            oAssignment.getTransformation().c_str(), oAssignment.getTo().c_str()));
        nRes = ERR_INVALID_ARG;
    }
    // Is the source known?
    if (!oAssignment.getSource().empty()&&!getSource(oAssignment.getSource()))
    {
        appendError(
            a_util::strings::format("Unknown <source> '%s' in <assignment> '%s'",
            oAssignment.getSource().c_str(), oAssignment.getTo().c_str()));
        nRes = ERR_INVALID_ARG;
    }
    return nRes;
}

a_util::result::Result MapConfiguration::checkAssignmentType(const std::string& strSignalName, const ddl::DDLComplex& oSignalStruct, const MapAssignment& oAssignment) const
{
    a_util::result::Result nRes= a_util::result::SUCCESS;
    const std::string& strTo = oAssignment.getTo();
    
    // Are From&To known in DDL and not dynamic?    
    int nArrSizeTo = 1;
    const ddl::IDDLDataType* pTypeTo = NULL;
    a_util::result::Result res = findTypeObject(oSignalStruct, strTo, pTypeTo, nArrSizeTo);
    if (res == ERR_NOT_FOUND)
    {
        appendError(
            a_util::strings::format("Target element '%s' unknown or invalid in <target> '%s'",
            strTo.c_str(), strSignalName.c_str()));
        nRes = ERR_INVALID_TYPE;
    }
    if (res == ERR_INVALID_TYPE)
    {
        appendError(
            a_util::strings::format("Dynamic target elements are not allowed: '%s' in <target> '%s'",
            strTo.c_str(), strSignalName.c_str()));
        nRes = ERR_INVALID_TYPE;
    }

    // find Type for assigned value
    const ddl::IDDLDataType* pTypeFrom = NULL;
    int nArrSizeFrom = 1;
    // If it is a received function, type must be bool
    if(oAssignment.getFunction() == "received")
    {
        pTypeFrom = _ddl_ref->getDataTypeByName("tBool");
    }
    else if (!oAssignment.getSource().empty())
    {
        const MapSource* pSrcInstance = getSource(oAssignment.getSource());
        const std::string& strFrom = oAssignment.getFrom();
        if (!strFrom.empty()) // its a regular field assignment
        {
            if (isFailed(findTypeObject(pSrcInstance->getType(), strFrom,
                pTypeFrom, nArrSizeFrom)))
            {
                appendError(a_util::strings::format(
                    "Source element '%s' unknown or invalid for <assignment> to '%s' in <target> '%s'",
                    strFrom.c_str(), strTo.c_str(), strSignalName.c_str()));
                nRes = ERR_INVALID_TYPE;
            }
        }
        else // its a whole signal assignment
        {
            // use source type for whole signal assignment
            pTypeFrom = _ddl_ref->getStructByName(pSrcInstance->getType());
        }
    }
    else // its a constant, a simulation_time or a trigger_counter
    {
        pTypeFrom = _ddl_ref->getDataTypeByName("tUInt64");
    }

    // Are From/Constant -> To type-compatible?
    if (!checkTypeCompatibility(*pTypeFrom, nArrSizeFrom, *pTypeTo, nArrSizeTo))
    {
        appendError(a_util::strings::format("Incompatible types for <assignment> to '%s' in <target> '%s'",
            strTo.c_str(), strSignalName.c_str()));
        nRes = ERR_INVALID_TYPE;
    }    

    return nRes;
}


a_util::result::Result MapConfiguration::checkTriggerType(const MapTriggerBase* pTrigger) const
{
    const oo::MapDataTrigger* pMapDTrigger =
        dynamic_cast<const oo::MapDataTrigger*>(pTrigger);
    if(pMapDTrigger)
    {
        const MapSource* pSrcInstance = getSource(pMapDTrigger->getSource());
        const ddl::IDDLDataType* pTypeVar = NULL;
        int nArrSizeVar = 1;
        if (isFailed(findTypeObject(pSrcInstance->getType(), pMapDTrigger->getVariable(),
            pTypeVar, nArrSizeVar)))
        {
            appendError(a_util::strings::format(
                "Source element '%s' type unknown or invalid for data <trigger> source '%s'",
                pMapDTrigger->getVariable().c_str(), pMapDTrigger->getSource().c_str()));
            return ERR_INVALID_TYPE;
        }

        const ddl::IDDLDataType* pTypeFrom = _ddl_ref->getDataTypeByName("tFloat64");
        // Are Value -> Variable type-compatible?
        if (!checkTypeCompatibility(*pTypeFrom, 1, *pTypeVar, nArrSizeVar))
        {
            appendError(a_util::strings::format("Data <trigger> variable '%s' in <source> '%s' should be comparable to a float value",
                pMapDTrigger->getVariable().c_str(), pMapDTrigger->getSource().c_str()));
            return ERR_INVALID_TYPE;
        }
    }
    return a_util::result::SUCCESS;
}

void MapConfiguration::appendError(const std::string& m_strError) const
{
    _errors.push_back(m_strError);
}

bool MapConfiguration::isConsistencyChecked()
{
    return _checked_for_consistency;
}

bool MapConfiguration::isConsistent()
{
    return _is_consistent;
}
