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

#include "map_trigger.h"

#include <cmath>
#include "a_util/result/error_def.h"
#include "legacy_error_macros.h"

#include "map_configuration.h"

namespace mapping
{
namespace oo
{
    //define all needed error types and values locally
    _MAKE_RESULT(-5, ERR_INVALID_ARG);
    _MAKE_RESULT(-40, ERR_INVALID_STATE);
}
}

using namespace mapping::oo;

MapTriggerBase::MapTriggerBase(MapConfiguration* pConfig) : _config(pConfig), _is_valid(true)
{
}

MapTriggerBase::~MapTriggerBase()
{
}

bool MapTriggerBase::isValid() const
{
    return _is_valid;
}

bool MapTriggerBase::checkValidity()
{
    if(isOk(_config->checkTriggerType(this)))
    {
        _is_valid = true;
    }
    else
    {
        _is_valid = false;
    }
    return _is_valid;
}

std::string MapTriggerBase::getSourceDependency() const
{
    return std::string();
}

a_util::result::Result MapTriggerBase::setSourceDependency(const std::string& strNewName)
{
    MapSignalTrigger* pMapSTrigger =
        dynamic_cast<MapSignalTrigger*>(this);
    if(pMapSTrigger)
    {
           return pMapSTrigger->setVariableNoTypeCheck(strNewName);
    }
    oo::MapDataTrigger* pMapDTrigger =
        dynamic_cast<oo::MapDataTrigger*>(this);
    if(pMapDTrigger)
    {
       return pMapDTrigger->setSourceNoTypeCheck(strNewName);
    }
    return a_util::result::SUCCESS;
}

a_util::result::Result MapTriggerBase::createFromDOM(MapConfiguration* pConfig, const a_util::xml::DOMElement& oTrigger,
    MapTriggerBase*& pDestination)
{
    // parse attributes
    const a_util::xml::DOMAttributes mapAttrs = oTrigger.getAttributes();
    a_util::xml::DOMAttributes::const_iterator it = mapAttrs.find("type");
    if (it == mapAttrs.end() || it->second.empty())
    {
        pConfig->appendError("Missing <trigger> type attribute");
        return ERR_INVALID_ARG;
    }

    if (it->second == "periodic")
    {
        MapPeriodicTrigger* pTrig = new MapPeriodicTrigger(pConfig);
        if (isFailed(pTrig->loadFromDom(oTrigger)))
        {
            delete pTrig;
            return ERR_INVALID_ARG;
        }

        pDestination = pTrig;
        return a_util::result::SUCCESS;
    }

    if (it->second == "signal")
    {
        MapSignalTrigger* pTrig = new MapSignalTrigger(pConfig);
        if (isFailed(pTrig->loadFromDom(oTrigger)))
        {
            delete pTrig;
            return ERR_INVALID_ARG;
        }

        pDestination = pTrig;
        return a_util::result::SUCCESS;
    }

    if (it->second == "data")
    {
        MapDataTrigger* pTrig = new MapDataTrigger(pConfig);
        if (isFailed(pTrig->loadFromDom(oTrigger)))
        {
            delete pTrig;
            return ERR_INVALID_ARG;
        }

        pDestination = pTrig;
        return a_util::result::SUCCESS;
    }

    pConfig->appendError(a_util::strings::format("Unknown <trigger> type '%s'",
        it->second.c_str()));

    return ERR_INVALID_ARG;
}

a_util::result::Result MapTriggerBase::writeToDOM(a_util::xml::DOMElement& oDOMElement) const
{
    oDOMElement.setName("trigger");

    const MapPeriodicTrigger* pMapPTrigger =
        dynamic_cast<const MapPeriodicTrigger*>(this);
    if(pMapPTrigger)
    {
        return pMapPTrigger->writeToDOM(oDOMElement);
    }

    const MapSignalTrigger* pMapSTrigger =
        dynamic_cast<const MapSignalTrigger*>(this);
    if(pMapSTrigger)
    {
       return pMapSTrigger->writeToDOM(oDOMElement);
    }

    const oo::MapDataTrigger* pMapDTrigger =
        dynamic_cast<const oo::MapDataTrigger*>(this);
    if(pMapDTrigger)
    {   
        return pMapDTrigger->writeToDOM(oDOMElement);
    }
    return a_util::result::SUCCESS;
}

a_util::result::Result MapTriggerBase::checkTriggerReferences() const
{
    const MapSignalTrigger* pMapSTrigger =
        dynamic_cast<const MapSignalTrigger*>(this);
    if(pMapSTrigger)
    {
        const MapSource* pSrcInstance = _config->getSource(pMapSTrigger->getVariable());
        if (!pSrcInstance)
        {
            _config->appendError(
                a_util::strings::format("Trigger references unknown <source> '%s'",
                pMapSTrigger->getVariable().c_str()));
            return ERR_INVALID_ARG;
        }
    }
    const oo::MapDataTrigger* pMapDTrigger =
        dynamic_cast<const oo::MapDataTrigger*>(this);
    if(pMapDTrigger)
    {
        const MapSource* pSrcInstance = _config->getSource(pMapDTrigger->getSource());
        if (!pSrcInstance)
        {
            _config->appendError(
                a_util::strings::format("Assignment references unknown <source> '%s'",
                pMapDTrigger->getSource().c_str()));
            return ERR_INVALID_ARG;
        }
    }
    return a_util::result::SUCCESS;
}

/**
* Periodic Trigger
**/

MapPeriodicTrigger::MapPeriodicTrigger(MapConfiguration* pConfig) :
    MapTriggerBase(pConfig), _period(0)
{
}

double MapPeriodicTrigger::getPeriod() const
{
    return _period;
}

a_util::result::Result MapPeriodicTrigger::setPeriod(const std::string& strPeriod, 
    const std::string& strUnit)
{
    a_util::result::Result res = a_util::result::SUCCESS;
    if (!a_util::strings::isUInt32(strPeriod) || strPeriod.find('-') != std::string::npos ||
        strPeriod.find('+') != std::string::npos)
    {
        _config->appendError(
            "Invalid period attribute for periodic trigger (expected positive integer)");
        res = ERR_INVALID_ARG;
    }

    if(!(strUnit == "us" || strUnit == "ms"
        || strUnit == "s"))
    {
        _config->appendError(
            "Invalid unit attribute for periodic trigger (expected us, ms or s)");
        res = ERR_INVALID_ARG;
    }

    if(isOk(res))
    {
        uint32_t nPeriod = a_util::strings::toUInt32(strPeriod);

        if (strUnit == "us")
        {
            _period = double(nPeriod) / 1000.0;
        }
        else if (strUnit == "ms")
        {
            _period = nPeriod;
        }
        else if (strUnit == "s")
        {
            _period = double(nPeriod) * 1000.0;
        }
    }
    return res;
}

a_util::result::Result MapPeriodicTrigger::loadFromDom(const a_util::xml::DOMElement& oTrigger)
{
    a_util::result::Result res = a_util::result::SUCCESS;
    const a_util::xml::DOMAttributes mapAttrs = oTrigger.getAttributes();
    a_util::xml::DOMAttributes::const_iterator itPeriod = mapAttrs.find("period");
    if (itPeriod == mapAttrs.end() || itPeriod->second.empty())
    {
        _config->appendError("Missing period attribute for periodic trigger");
        res = ERR_INVALID_ARG;
    } 

    a_util::xml::DOMAttributes::const_iterator itUnit = mapAttrs.find("unit");
    if (itUnit == mapAttrs.end() || itUnit->second.empty())
    {
        _config->appendError("Missing unit attribute for periodic trigger (expected us, ms or s)");
        res = ERR_INVALID_ARG;
    }

    if(isOk(res))
    {
        std::string strUnit = itUnit->second;
        a_util::strings::trim(strUnit);
        res = setPeriod(itPeriod->second, strUnit);
    }

    return res;
}

a_util::result::Result MapPeriodicTrigger::writeToDOM(a_util::xml::DOMElement& oDOMElement) const
{
    oDOMElement.setAttribute("type", "periodic");
    if(_period == ceil(_period))
    {
        if((_period/1000) == ceil(_period/1000))
        {
            oDOMElement.setAttribute("period", a_util::strings::format("%d", (uint32_t)floor(_period/1000)));
            oDOMElement.setAttribute("unit", "s");
        }
        else
        {
            oDOMElement.setAttribute("period", a_util::strings::format("%d", (uint32_t)floor(_period)));
            oDOMElement.setAttribute("unit", "ms");
        }        
    }
    else
    {
        oDOMElement.setAttribute("period", a_util::strings::format("%d", (uint32_t)floor(_period*1000)));
        oDOMElement.setAttribute("unit", "us");
    }
    return a_util::result::SUCCESS;
}

MapTriggerBase* MapPeriodicTrigger::clone() const
{
    return new MapPeriodicTrigger(*this);
}

bool MapPeriodicTrigger::isEqual(const MapTriggerBase& oOther) const
{
    const MapPeriodicTrigger* p = dynamic_cast<const MapPeriodicTrigger*>(&oOther);
    if (p)
    {
        return getPeriod() == p->getPeriod();
    }

    return false;
}

/**
* Signal Trigger
**/
MapSignalTrigger::MapSignalTrigger(MapConfiguration* pConfig) : MapTriggerBase(pConfig)
{
}

std::string MapSignalTrigger::getSourceDependency() const
{
    return _variable;
}

const std::string& MapSignalTrigger::getVariable() const
{
    return _variable;
}

a_util::result::Result MapSignalTrigger::setVariable(const std::string& strSignalName)
{
    if(!_config)
    {
        _is_valid = false;
        return ERR_INVALID_STATE;
    }
    setVariableNoTypeCheck(strSignalName);
    if(isFailed(_config->resetErrors()))
    {
        _is_valid = false;
        return ERR_INVALID_STATE;
    }

    const MapSource* pSrcInstance = _config->getSource(_variable);
    if (!pSrcInstance)
    {
        _config->appendError(
            a_util::strings::format("Trigger references unknown <source> '%s'",
            _variable.c_str()));
        _variable.clear();
        _is_valid = false;
        return ERR_INVALID_ARG;
    }
    _is_valid = true;
    return a_util::result::SUCCESS;
}

a_util::result::Result MapSignalTrigger::loadFromDom(const a_util::xml::DOMElement& oTrigger)
{
    const a_util::xml::DOMAttributes mapAttrs = oTrigger.getAttributes();
    a_util::xml::DOMAttributes::const_iterator it = mapAttrs.find("variable");
    if (it == mapAttrs.end() || it->second.empty())
    {
        _config->appendError("Missing variable attribute for signal trigger");
        return ERR_INVALID_ARG;
    }
    return setVariableNoTypeCheck(it->second);
}

a_util::result::Result MapSignalTrigger::writeToDOM(a_util::xml::DOMElement& oDOMElement) const
{
    oDOMElement.setAttribute("type", "signal");
    oDOMElement.setAttribute("variable", _variable);
    return a_util::result::SUCCESS;
}

a_util::result::Result MapSignalTrigger::setVariableNoTypeCheck(const std::string& strSignalName)
{
    _variable = strSignalName;
    return a_util::result::SUCCESS;
}


MapTriggerBase* MapSignalTrigger::clone() const
{
    return new MapSignalTrigger(*this);
}

bool MapSignalTrigger::isEqual(const MapTriggerBase& oOther) const
{
    const MapSignalTrigger* p = dynamic_cast<const MapSignalTrigger*>(&oOther);
    if (p)
    {
        return getVariable() == p->getVariable();
    }

    return false;
}

/**
* Data Trigger
**/
MapDataTrigger::MapDataTrigger(MapConfiguration* pConfig) : MapTriggerBase(pConfig)
{    
}

std::string MapDataTrigger::getSourceDependency() const
{
    return _source;
}

const std::string& MapDataTrigger::getVariable() const
{
    return _variable;
}

const std::string& MapDataTrigger::getSource() const
{
    return _source;
}



a_util::result::Result MapDataTrigger::setSourceNoTypeCheck(const std::string& strSource)
{
    _source = strSource;
    return a_util::result::SUCCESS;
}

const std::string& MapDataTrigger::getOperator() const
{
    return _operator;
}

double MapDataTrigger::getValue() const
{
    return _value;
}

a_util::result::Result MapDataTrigger::loadFromDom(const a_util::xml::DOMElement& oTrigger)
{
    const a_util::xml::DOMAttributes mapAttrs = oTrigger.getAttributes();
    a_util::xml::DOMAttributes::const_iterator itVariable = mapAttrs.find("variable");
    a_util::result::Result res = a_util::result::SUCCESS;
    if (itVariable == mapAttrs.end() || itVariable->second.empty())
    {
        _config->appendError("Missing variable attribute for data trigger");
        res = ERR_INVALID_ARG;
    }

    a_util::xml::DOMAttributes::const_iterator itOperator = mapAttrs.find("operator");
    if (itOperator == mapAttrs.end() || itOperator->second.empty())
    {
        _config->appendError("Missing operator attribute for data trigger");
        res =  ERR_INVALID_ARG;
    }

    a_util::xml::DOMAttributes::const_iterator itValue = mapAttrs.find("value");
    if (itValue == mapAttrs.end() || itValue->second.empty())
    {
        _config->appendError("Missing value attribute for data trigger (expected a source name)");
        res = ERR_INVALID_ARG;
    }
    
    if(isOk(res))
    {
        res = setComparisonNoTypeCheck(itVariable->second, itOperator->second, itValue->second);
    }

    return res;
}

a_util::result::Result MapDataTrigger::writeToDOM(a_util::xml::DOMElement& oDOMElement) const
{
    oDOMElement.setAttribute("type", "data");
    oDOMElement.setAttribute("variable", _source + "." + _variable);
    oDOMElement.setAttribute("operator", _operator);
    oDOMElement.setAttribute("value", a_util::strings::format("%g", _value));
    return a_util::result::SUCCESS;
}

a_util::result::Result MapDataTrigger::setComparisonNoTypeCheck(const std::string& strSourceElementPath, 
    const std::string& strOperator, const std::string& strValue)
{    
    a_util::result::Result res = a_util::result::SUCCESS;

    _variable = strSourceElementPath;
    a_util::strings::trim(_variable);
    _source.clear();

    // parse source out of <from>
    size_t idx = _variable.find('.');
    if (idx != std::string::npos)
    {
        _source = _variable.substr(0, idx);
        _variable = _variable.substr(idx + 1);
    }
    else
    {
        _variable.clear();
        _config->appendError("Variable attribute for data trigger should be a signal element (expected [Signal].[Element])");
        res =  ERR_INVALID_ARG;
    }
    if (!(strOperator == "less_than" || strOperator == "greater_than" ||
        strOperator == "less_than_equal" || strOperator == "greater_than_equal" ||
        strOperator == "equal" || strOperator == "not_equal"))
    {
        _operator.clear();
        _config->appendError(
            "Invalid operator attribute for data trigger (expected less_than, greater_than, "
            "less_than_equal, greater_than_equal, equal or not_equal)");
        res =  ERR_INVALID_ARG;
    }
    else 
    {
        _operator = strOperator;
    }

    if (!a_util::strings::isDouble(strValue))
    {
        _value = 0;
        _config->appendError(
            "Invalid value attribute for data trigger (expected floating point value)");
        res = ERR_INVALID_ARG;
    }
    else 
    {
        _value = a_util::strings::toDouble(strValue);
    }

    return res;
}

MapTriggerBase* MapDataTrigger::clone() const
{
    return new MapDataTrigger(*this);
}

bool MapDataTrigger::isEqual(const MapTriggerBase& oOther) const
{
    const MapDataTrigger* p = dynamic_cast<const MapDataTrigger*>(&oOther);
    if (p)
    {
        return getVariable() == p->getVariable() && getSource() == p->getSource() && 
            getOperator() == p->getOperator() && getValue() == p->getValue();
    }

    return false;
}

a_util::result::Result MapDataTrigger::setComparison(const std::string& strSourceElementPath, 
    const std::string& strOperator, const std::string& strValue)
{
    if(!_config)
    {
        _is_valid = false;
        return ERR_INVALID_STATE;
    }
    RETURN_IF_FAILED(_config->resetErrors());
    RETURN_IF_FAILED(setComparisonNoTypeCheck(strSourceElementPath, strOperator, strValue));
    const MapSource* pSrcInstance = _config->getSource(_source);
    if (!pSrcInstance)
    {
        _config->appendError(
            a_util::strings::format("Assignment references unknown <source> '%s'",
            _source.c_str()));
        _is_valid = false;
        return ERR_INVALID_ARG;
    }
    a_util::result::Result nRes = _config->checkTriggerType(this);
    if(isFailed(nRes))
    {
        _is_valid = false;
    }
    if(isOk(nRes))
    {
        _is_valid = true;
    }
    return nRes;
}
