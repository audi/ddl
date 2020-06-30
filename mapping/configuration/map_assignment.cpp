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

#include "map_assignment.h"

namespace mapping
{
namespace oo
{
//define all needed error types and values locally
_MAKE_RESULT(-5, ERR_INVALID_ARG);
}
}

using namespace mapping::oo;

MapAssignment::MapAssignment() : _is_valid(true)
{
}

MapAssignment::MapAssignment(std::string strTo) : _to(strTo), _is_valid(true)
{
}

bool MapAssignment::isValid() const
{
    return _is_valid;
}

const std::string& MapAssignment::getFrom() const
{
    return _from;
}

const std::string& MapAssignment::getSource() const
{
    return _source;
}

a_util::result::Result MapAssignment::connect(const std::string& strSourceElementPath)
{    
    _constant.clear();
    _function.clear();
    _modulo.clear();
    _transform.clear();

    _from = strSourceElementPath;
    a_util::strings::trim(_from);
    _source.clear();
    // parse source out of <from>
    // parse source
    size_t idx = _from.find('.');
    if (idx != std::string::npos)
    {
        _source = _from.substr(0, idx);
        _from = _from.substr(idx + 1);
    }
    else
    {
        // no path when the signal itself is referenced
        _source = _from;
        _from.clear();
    }

    if(_source.empty())
    {
        return ERR_INVALID_ARG;
    }

    return a_util::result::SUCCESS;
}

const std::string& MapAssignment::getConstant() const
{
    return _constant;
}

a_util::result::Result MapAssignment::setConstant(const std::string& strConstant)
{
    _from.clear();
    _source.clear();
    _function.clear();
    _modulo.clear();
    _transform.clear();

    _constant = strConstant;
    a_util::strings::trim(_constant);
    if(_constant.empty())
    {
        return ERR_INVALID_ARG;
    }
    return a_util::result::SUCCESS;
}

const std::string& MapAssignment::getFunction() const
{
    return _function;
}

a_util::result::Result MapAssignment::setSimulationTimeFunction()
{
    _from.clear();
    _source.clear();
    _constant.clear();
    _modulo.clear();
    _transform.clear();

    _function = "simulation_time";
    return a_util::result::SUCCESS;
}

a_util::result::Result MapAssignment::setTriggerCounterFunction(const std::string& strModulo)
{
    _from.clear();
    _source.clear();
    _constant.clear();
    _transform.clear();
    _function.clear();

    _modulo = strModulo;
    a_util::strings::trim(_modulo);
    _function = "trigger_counter";
    return a_util::result::SUCCESS;    
}

a_util::result::Result MapAssignment::setReceivedFunction(const std::string& strSource)
{
    _from.clear();
    _source.clear();
    _constant.clear();
    _modulo.clear();
    _transform.clear();

    _source = strSource;
    a_util::strings::trim(_source);
    if(_source.empty())
    {
        return ERR_INVALID_ARG;
    }
    _function = "received";
    return a_util::result::SUCCESS;
}

const std::string& MapAssignment::getModulo() const
{
    return _modulo;
}

const std::string& MapAssignment::getTo() const
{
    return _to;
}

const std::string& MapAssignment::getTransformation() const
{
    return _transform;
}

a_util::result::Result MapAssignment::setTransformation(const std::string& strTransformationName)
{
    _transform.clear();
    if(_source.empty()||_from.empty())
    {
        return ERR_INVALID_ARG;
    }
    _transform = strTransformationName;
    a_util::strings::trim(_transform);
    return a_util::result::SUCCESS;
}

a_util::result::Result MapAssignment::removeTransformation()
{
    _transform.clear();
    return a_util::result::SUCCESS;
}

a_util::result::Result MapAssignment::loadFromDOM(const a_util::xml::DOMElement& oAssignment, MapErrorList& lstErrors)
{
    // parse attributes
    const a_util::xml::DOMAttributes mapAttrs = oAssignment.getAttributes();
    // find target element
    a_util::xml::DOMAttributes::const_iterator itAttrTo = mapAttrs.find("to");
    if (itAttrTo == mapAttrs.end() || itAttrTo->second.empty())
    {
        lstErrors.push_back("Missing <to> attribute for an <assignment>");
        return ERR_INVALID_ARG;
    }
    _to = itAttrTo->second;
    a_util::strings::trim(_to);

    // find Source or constant
    a_util::xml::DOMAttributes::const_iterator itAttrFrom = mapAttrs.find("from");
    a_util::xml::DOMAttributes::const_iterator itAttrConst = mapAttrs.find("constant");
    a_util::xml::DOMAttributes::const_iterator itAttrFct = mapAttrs.find("function");
    if (itAttrFrom == mapAttrs.end() && itAttrConst == mapAttrs.end() && itAttrFct == mapAttrs.end())
    {
        lstErrors.push_back(a_util::strings::format("Missing <from>, <constant> or <function> attribute for <assignment> to '%s'", 
            itAttrTo->second.c_str()));
        return ERR_INVALID_ARG;
    }
    if (itAttrFrom != mapAttrs.end() && itAttrConst != mapAttrs.end())
    {
        lstErrors.push_back(a_util::strings::format("<from> and <constant> attributes set at the same time for <assignment> to '%s'", 
            itAttrTo->second.c_str()));
        return ERR_INVALID_ARG;
    }
    if (itAttrFrom != mapAttrs.end() && itAttrFct != mapAttrs.end())
    {
        lstErrors.push_back(a_util::strings::format("<from> and <function> attributes set at the same time for <assignment> to '%s'", 
            itAttrTo->second.c_str()));
        return ERR_INVALID_ARG;
    }
    if (itAttrFct != mapAttrs.end() && itAttrConst != mapAttrs.end())
    {
        lstErrors.push_back(a_util::strings::format("<function> and <constant> attributes set at the same time for <assignment> to '%s'", 
            itAttrTo->second.c_str()));
        return ERR_INVALID_ARG;
    }
    
    if (itAttrFrom != mapAttrs.end())
    {
        if(itAttrFrom->second.empty())
        {
            lstErrors.push_back(a_util::strings::format("<from> attribute empty for <assignment> to '%s'", 
                itAttrTo->second.c_str()));
            return ERR_INVALID_ARG;
        }
        connect(itAttrFrom->second);
    }
    else if(itAttrConst != mapAttrs.end())
    {
        if(isFailed(setConstant(itAttrConst->second)))
        {
            lstErrors.push_back(a_util::strings::format("<constant> attribute empty for <assignment> \
                                              to '%s'", itAttrTo->second.c_str()));
            return ERR_INVALID_ARG;
        }
    }
    else
    {
        if(itAttrFct->second.empty())
        {
            lstErrors.push_back(a_util::strings::format("<function> attribute empty for <assignment> \
                                              to '%s'", itAttrTo->second.c_str()));
            return ERR_INVALID_ARG;
        }
        _function = itAttrFct->second;

        size_t nIdx1 = _function.find('(');
        size_t nIdx2 = _function.find(')');
        if (nIdx1 == std::string::npos || nIdx2 == std::string::npos ||
            nIdx1 == 0 || nIdx2 == 0 || nIdx1 > nIdx2
            || !(_function.find("simulation_time") == 0
            || _function.find("trigger_counter") == 0
            || _function.find("received") == 0))
        {
            lstErrors.push_back(a_util::strings::format("<function> should be of type 'simulation_time()', \
                                                'trigger_counter([Modulo])' or 'received([Signal])' for <assignment> \
                                                to '%s'", itAttrTo->second.c_str()));
            return ERR_INVALID_ARG;
        }

        int64_t nLength = (int64_t)nIdx2 - (int64_t)nIdx1 - 1;
        if(_function.find("simulation_time") == 0)
        {
            if(nLength > 0)
            {
                lstErrors.push_back(a_util::strings::format("<function> of type 'simulation_time()' takes no argument for <assignment> \
                                                 to '%s'", itAttrTo->second.c_str()));
                return ERR_INVALID_ARG;
            }
            setSimulationTimeFunction();
        }
        if(_function.find("trigger_counter") == 0)
        {
            if(isFailed(setTriggerCounterFunction(_function.substr(nIdx1 + 1, (size_t)nLength))))
            {
                    lstErrors.push_back(a_util::strings::format("<function> of type 'trigger_counter()' takes a positive Integer as argument for <assignment> \
                                                     to '%s'", itAttrTo->second.c_str()));
                    return ERR_INVALID_ARG;
            }
        } 
        else if(_function.find("received") == 0)
        {            
            if(isFailed(setReceivedFunction(_function.substr(nIdx1 + 1, (size_t)nLength))))
            {
                lstErrors.push_back(a_util::strings::format("<function> of type'received([Signal])' has no argument for <assignment> \
                                                  to '%s'", itAttrTo->second.c_str()));
                return ERR_INVALID_ARG;
            }
        }
    }

    a_util::xml::DOMAttributes::const_iterator itAttr = mapAttrs.find("transformation");
    if (itAttr != mapAttrs.end() && !itAttr->second.empty())
    {
        if(isFailed(setTransformation(itAttr->second)))
        {
            lstErrors.push_back(a_util::strings::format("<transformation> is only accepted for connection for <assignment> \
                                             to '%s'", itAttrTo->second.c_str()));
            return ERR_INVALID_ARG;
        }        
    }

    return a_util::result::SUCCESS;
}

a_util::result::Result MapAssignment::writeToDOM(a_util::xml::DOMElement& oDOMElement) const
{
    oDOMElement.setName("assignment");
    oDOMElement.setAttribute("to", _to);
    if(!_function.empty())
    {
        std::string strDOMFunction = _function;
        if(strDOMFunction == "simulation_time")
        {
            strDOMFunction.append("()");
        }
        else if(strDOMFunction == "trigger_counter")
        {
            strDOMFunction.append(a_util::strings::format("(%s)", _modulo.c_str()));
        }
        else if(strDOMFunction == "received")
        {
            strDOMFunction.append(a_util::strings::format("(%s)", _source.c_str()));
        }
        oDOMElement.setAttribute("function", strDOMFunction);
    }
    else if(!_source.empty())
    {
        std::string strDOMFrom = _source;
        if(!_from.empty())
        {
            strDOMFrom.push_back('.');
            strDOMFrom.append(_from);
        }
        oDOMElement.setAttribute("from", strDOMFrom);
        if(!_transform.empty())
        {
            oDOMElement.setAttribute("transformation", _transform);
        }
    }
    else if(!_constant.empty())
    {
        oDOMElement.setAttribute("constant", _constant);
    }
    return a_util::result::SUCCESS;
}
