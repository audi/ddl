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

#include "map_transformation.h"

#include "a_util/result/error_def.h"
#include "legacy_error_macros.h"

#include "ddlrepresentation/ddlenum.h"

#include "map_configuration.h"

namespace mapping
{
namespace oo
{
    //define all needed error types and values locally
    _MAKE_RESULT(-5, ERR_INVALID_ARG)
    _MAKE_RESULT(-40, ERR_INVALID_STATE)
    _MAKE_RESULT(-42, ERR_INVALID_TYPE)
}
}

using namespace mapping::oo;

MapTransformationBase::MapTransformationBase(MapConfiguration* pConfig, const std::string& name)
    : _config(pConfig), _name(name), _is_valid(true)
{
}

MapTransformationBase::~MapTransformationBase()
{
}

a_util::result::Result MapTransformationBase::createFromDOM(MapConfiguration* pConfig, const a_util::xml::DOMElement& oTrans,
    MapTransformationBase*& pDestination)
{
    const a_util::xml::DOMAttributes mapAttrs = oTrans.getAttributes();
    a_util::xml::DOMAttributes::const_iterator it = mapAttrs.find("name");
    if (it == mapAttrs.end() || it->second.empty())
    {
        pConfig->appendError("Missing name attribute for a transformation element");
        return ERR_INVALID_ARG;
    }
    if(pConfig->getTransformation(it->second))
    {
        pConfig->appendError("A transformation with this name already exists");
        return ERR_INVALID_ARG;
    }
    std::string name(it->second);

    if (oTrans.getName() == "polynomial")
    {
        MapPolynomTransformation* pPoly = new MapPolynomTransformation(pConfig, name);
        if (isFailed(pPoly->loadFromDom(oTrans)))
        {
            delete pPoly;
            return ERR_INVALID_ARG;
        }

        pDestination = pPoly;
        return a_util::result::SUCCESS;
    }

    if (oTrans.getName() == "enum_table")
    {
        MapEnumTableTransformation* pPoly = new MapEnumTableTransformation(pConfig, name);
        if (isFailed(pPoly->loadFromDom(oTrans)))
        {
            delete pPoly;
            return ERR_INVALID_ARG;
        }

        pDestination = pPoly;
        return a_util::result::SUCCESS;
    }
    
    pConfig->appendError("Invalid transformation element (expected <polynomial>)");

    return ERR_INVALID_ARG;
}

a_util::result::Result MapTransformationBase::create(MapConfiguration* pConfig, 
    const std::string& name, const std::string& strType, MapTransformationBase*& pDestination)
{
    if (strType == "polynomial")
    {
        MapPolynomTransformation* pPoly = new MapPolynomTransformation(pConfig, name);
        pDestination = pPoly;
        return a_util::result::SUCCESS;
    }

    if (strType == "enum_table")
    {
        MapEnumTableTransformation* pPoly = new MapEnumTableTransformation(pConfig, name);
        pDestination = pPoly;
        return a_util::result::SUCCESS;
    }

    pConfig->appendError("Invalid transformation type (expected <polynomial> or <enum_table>)");
    return ERR_INVALID_ARG;
}

a_util::result::Result MapTransformationBase::setTypeFromDDL()
{
    MapEnumTableTransformation* pMapETransf =
        dynamic_cast<MapEnumTableTransformation*>(this);
    if(pMapETransf)
    {
        return pMapETransf->convertValuesWithDDL();
    }
    return a_util::result::SUCCESS;
}

a_util::result::Result MapTransformationBase::writeToDOM(a_util::xml::DOMElement& oDOMElement) const
{
    const MapPolynomTransformation* pMapPTransf =
        dynamic_cast<const MapPolynomTransformation*>(this);
    if(pMapPTransf)
    {
        return pMapPTransf->writeToDOM(oDOMElement);
    }
    const MapEnumTableTransformation* pMapETransf =
        dynamic_cast<const MapEnumTableTransformation*>(this);
    if(pMapETransf)
    {
        return pMapETransf->writeToDOM(oDOMElement);
    }

    return a_util::result::SUCCESS;
}

const std::string& MapTransformationBase::getName() const
{
    return _name;
}

bool MapTransformationBase::isValid() const
{
    return _is_valid;
}

MapPolynomTransformation::MapPolynomTransformation(MapConfiguration* pConfig, const std::string& name)
    : MapTransformationBase(pConfig, name), _a(0), _b(0), _c(0), _d(0), _e(0)
{
}

double MapPolynomTransformation::getA() const
{
    return _a;
}

double MapPolynomTransformation::getB() const
{
    return _b;
}

double MapPolynomTransformation::getC() const
{
    return _c;
}

double MapPolynomTransformation::getD() const
{
    return _d;
}

double MapPolynomTransformation::getE() const
{
    return _e;
}

MapTransformationBase* MapPolynomTransformation::clone() const
{
    return new MapPolynomTransformation(*this);
}

bool MapPolynomTransformation::isEqual(const MapTransformationBase& oOther) const
{
    if (oOther.getName() != getName())
    {
        return false;
    }

    const MapPolynomTransformation* p =
        dynamic_cast<const MapPolynomTransformation*>(&oOther);
    if (p)
    {
        return getA() == p->getA() && getB() == p->getB() &&
            getC() == p->getC() && getD() == p->getD() && getE() == p->getE();
    }

    return false;
}

a_util::result::Result MapPolynomTransformation::loadFromDom(const a_util::xml::DOMElement& oTrans)
{
    const a_util::xml::DOMAttributes mapAttrs = oTrans.getAttributes();
    std::string strCoefs[5];
    char c = 'a';
    for (int i = 0; i < 5; ++i, ++c)
    {
        a_util::xml::DOMAttributes::const_iterator it = mapAttrs.find(std::string(1, c));
        if (it != mapAttrs.end())
        {
            strCoefs[i] = it->second;
        }
    }

    return setCoefficients(strCoefs);
}

a_util::result::Result MapPolynomTransformation::setCoefficients(const std::string strCoefs[5])
{
    a_util::result::Result res = a_util::result::SUCCESS;
    double p[5] = {0};
    char c = 'a';
    for (int i = 0; i < 5; ++i, c++)
    {
        if(!strCoefs[i].empty())
        {
            if (!a_util::strings::isDouble(strCoefs[i]))
            {
                _config->appendError(
                    a_util::strings::format("Invalid coefficient attribute '%c' for polynomial transformation '%s' (expected number)", 
                    c, this->getName().c_str()));
                res = ERR_INVALID_ARG;
            }
            else
            {
                p[i] = a_util::strings::toDouble(strCoefs[i]);
            }
        }
    }
    _a = p[0];
    _b = p[1];
    _c = p[2];
    _d = p[3];
    _e = p[4];
    return res;
}

a_util::result::Result MapPolynomTransformation::writeToDOM(a_util::xml::DOMElement& oDOMElement) const
{
    oDOMElement.setName("polynomial");
    oDOMElement.setAttribute("name", _name);
    if(_a != 0)
    {
        oDOMElement.setAttribute("a", a_util::strings::format("%g",_a));
    }
    if(_b != 0)
    {
        oDOMElement.setAttribute("b", a_util::strings::format("%g",_b));
    }
    if(_c != 0)
    {
        oDOMElement.setAttribute("c", a_util::strings::format("%g",_c));
    }
    if(_d != 0)
    {
        oDOMElement.setAttribute("d", a_util::strings::format("%g",_d));
    }
    if(_e != 0)
    {
        oDOMElement.setAttribute("e", a_util::strings::format("%g",_e));
    }
    return a_util::result::SUCCESS;
}

double MapPolynomTransformation::evaluate(double value) const
{
    double f64Pow2 = value*value;
    double f64Pow3 = f64Pow2*value;
    return (_a + _b*value + _c*f64Pow2 + _d*f64Pow3 + _e*f64Pow3*value);
}


MapEnumTableTransformation::MapEnumTableTransformation(MapConfiguration* pConfig, const std::string& name)
    : MapTransformationBase(pConfig, name), _default_int(0), _default_value("0")
{
}

MapTransformationBase* MapEnumTableTransformation::clone() const
{
    return new MapEnumTableTransformation(*this);
}

bool MapEnumTableTransformation::isEqual(const MapTransformationBase& oOther) const
{
    if (oOther.getName() != getName())
    {
        return false;
    }

    const MapEnumTableTransformation* p =
        dynamic_cast<const MapEnumTableTransformation*>(&oOther);
    if (p)
    {
        return getDefaultStr() == p->getDefaultStr() && getConversionsStr() == p->getConversionsStr();
    }

    return false;
}

a_util::result::Result MapEnumTableTransformation::loadFromDom(const a_util::xml::DOMElement& oTrans)
{    
    const a_util::xml::DOMAttributes mapAttrs = oTrans.getAttributes();
    a_util::result::Result nRes = a_util::result::SUCCESS;
    // Read <from> attribute
    a_util::xml::DOMAttributes::const_iterator itAttrFrom = mapAttrs.find("from");
    if (itAttrFrom == mapAttrs.end() || itAttrFrom->second.empty())
    {
        _config->appendError(a_util::strings::format("Missing <from> attribute for <transformation> '%s'", 
            getName().c_str()));
        nRes = ERR_INVALID_ARG;
    }

    // Read <to> attribute
    a_util::xml::DOMAttributes::const_iterator itAttrTo = mapAttrs.find("to");
    if (itAttrTo == mapAttrs.end() || itAttrTo->second.empty())
    {
        _config->appendError(a_util::strings::format("Missing <to> attribute for <transformation> '%s'", 
            getName().c_str()));
        nRes = ERR_INVALID_ARG;
    }

    if(isOk(nRes))
    {
        setEnumsStr(itAttrFrom->second, itAttrTo->second);

        // Read <default> attribute
        a_util::xml::DOMAttributes::const_iterator itAttrDefault = mapAttrs.find("default");
        if (itAttrDefault == mapAttrs.end() || itAttrDefault->second.empty())
        {
            _config->appendError(a_util::strings::format("Missing <default> attribute for <transformation> '%s'", 
                getName().c_str()));
            return ERR_INVALID_ARG;
        }
        setDefaultStr(itAttrDefault->second);

        // Search conversions in DDLEnums
        const a_util::xml::DOMElementList lstConv = oTrans.getChildren();
    
        for (a_util::xml::DOMElementList::const_iterator it = lstConv.begin(); it != lstConv.end(); ++it)
        {
            a_util::result::Result res = a_util::result::SUCCESS;
            const a_util::xml::DOMAttributes mapConvAttrs = it->getAttributes();
            
            a_util::xml::DOMAttributes::const_iterator itConvAttrFrom = mapConvAttrs.find("from");
            if (itConvAttrFrom == mapConvAttrs.end() || itConvAttrFrom->second.empty())
            {
                _config->appendError(a_util::strings::format("Missing <from> attribute for <conversion> in <transformation> '%s'", 
                    getName().c_str()));
                res = ERR_INVALID_ARG;
            }
            
            a_util::xml::DOMAttributes::const_iterator itConvAttrTo = mapConvAttrs.find("to");
            if (itConvAttrTo == mapConvAttrs.end() || itConvAttrTo->second.empty())
            {
                _config->appendError(a_util::strings::format("Missing <to> attribute for <conversion> in <transformation> '%s'", 
                    getName().c_str()));
                res = ERR_INVALID_ARG;
            }            

            if(a_util::result::isOk(res))
            {
                if(isFailed(addConversionStr(itConvAttrFrom->second, itConvAttrTo->second)))
                {
                    _config->appendError(a_util::strings::format("<from> attribute '%s' appears more than once in <transformation> '%s'", 
                        itConvAttrFrom->second.c_str(), getName().c_str()));
                    res = ERR_INVALID_ARG;
                }
            }

            if(a_util::result::isFailed(res))
            {
                nRes = res;
            }
        }
    }
    return nRes;
}

a_util::result::Result MapEnumTableTransformation::writeToDOM(a_util::xml::DOMElement& oDOMElement) const
{
    oDOMElement.setName("enum_table");
    oDOMElement.setAttribute("name", _name);
    oDOMElement.setAttribute("from", _enum_from);
    oDOMElement.setAttribute("to", _enum_to);
    oDOMElement.setAttribute("default", _default_value);
    for(MapStrConversionList::const_iterator itConv = _conversions.begin(); itConv != _conversions.end(); itConv++)
    {
        a_util::xml::DOMElement oDOMConv = oDOMElement.createChild("conversion");
        oDOMConv.setAttribute("from", itConv->first);
        oDOMConv.setAttribute("to", itConv->second);
    }
    return a_util::result::SUCCESS;
}

const std::string& MapEnumTableTransformation::getEnumFrom() const
{
    return _enum_from;
}

const std::string& MapEnumTableTransformation::getEnumTo() const
{
    return _enum_to;
}

a_util::result::Result mapping::oo::MapEnumTableTransformation::setEnums(const std::string& strEnumFrom, const std::string& strEnumTo)
{
    RETURN_IF_FAILED(setEnumsStr(strEnumFrom, strEnumTo));
    a_util::result::Result nRes = convertValuesWithDDL();
    if(isFailed(nRes))
    {
        _enum_from.clear();
        _enum_to.clear();
    }
    return nRes;
}

int64_t MapEnumTableTransformation::getDefault() const
{
    return _default_int;
}

std::string MapEnumTableTransformation::getDefaultStr() const
{
    return _default_value;
}

a_util::result::Result mapping::oo::MapEnumTableTransformation::setDefault(const std::string& strDefault)
{
    RETURN_IF_FAILED(setDefaultStr(strDefault));
    a_util::result::Result nRes = convertValuesWithDDL();
    if(isFailed(nRes))
    {
        _default_value.clear();
    }
    return nRes;
}

const std::map<int64_t, int64_t>& MapEnumTableTransformation::getConversions() const
{
    return _conversions_int;
}

const std::map<std::string, std::string>& MapEnumTableTransformation::getConversionsStr() const
{
    return _conversions;
}

a_util::result::Result mapping::oo::MapEnumTableTransformation::addConversion(const std::string& strFrom, const std::string& strTo)
{
    RETURN_IF_FAILED(addConversionStr(strFrom, strTo));
    a_util::result::Result nRes = convertValuesWithDDL();
    if(isFailed(nRes))
    {
        _conversions.erase(strFrom);
    }
    return nRes;
}

a_util::result::Result MapEnumTableTransformation::removeConversion(const std::string& strFrom)
{
    _conversions.erase(strFrom);
    return convertValuesWithDDL();
}

double MapEnumTableTransformation::evaluate(double f64Value) const
{
    std::map<int64_t, int64_t>::const_iterator it = _conversions_int.find((int64_t)f64Value);
    if(it != _conversions_int.end())
    {
        return (double)it->second;
    }
    return (double)_default_int;
}

a_util::result::Result MapEnumTableTransformation::setEnumsStr(const std::string& strEnumFrom, const std::string& strEnumTo)
{
    _enum_from = strEnumFrom;
    _enum_to = strEnumTo;

    _conversions_int.clear();
    _conversions.clear();
    _default_value.clear();
    return a_util::result::SUCCESS;
}

a_util::result::Result MapEnumTableTransformation::setDefaultStr(const std::string& strDefault)
{
    _default_value = strDefault;
    return a_util::result::SUCCESS;
}

a_util::result::Result MapEnumTableTransformation::addConversionStr(const std::string& strFrom, const std::string& strTo)
{
    if(_conversions.find(strFrom) != _conversions.end())
    {
        return ERR_INVALID_ARG;
    }

    _conversions[strFrom] = strTo;
    return a_util::result::SUCCESS;
}

a_util::result::Result MapEnumTableTransformation::convertValuesWithDDL()
{
    if(!_config)
    {
        _is_valid = false;
        return ERR_INVALID_STATE;
    }
    if(!_config->getDescription())
    {
        _config->appendError(a_util::strings::format("No description found", 
            getName().c_str()));
        _is_valid = false;
        return ERR_INVALID_STATE;
    }
    // Search <from> in DDL
    const ddl::DDLEnum * pEnumFrom = _config->getDescription()->getEnumByName(_enum_from);
    if(pEnumFrom == NULL)
    {
        _config->appendError(a_util::strings::format("<from> attribute for <transformation> '%s' should be an enumeration", 
            getName().c_str()));
        _is_valid = false;
        return ERR_INVALID_TYPE;
    }
    // Search <to> in DDL
    const ddl::DDLEnum * pEnumTo = _config->getDescription()->getEnumByName(_enum_to);
    if(pEnumTo == NULL)
    {
        _config->appendError(a_util::strings::format("<to> attribute for <transformation> '%s' should be an enumeration", 
            getName().c_str()));
        _is_valid = false;
        return ERR_INVALID_TYPE;
    }

    // Search <default> in DDL
    if(!_default_value.empty())
    {
        std::string strValue;
        if(isFailed(pEnumTo->getValue(_default_value, strValue)) || !a_util::strings::isInt64(strValue))
        {
            _config->appendError(a_util::strings::format("<default> attribute for <transformation> '%s' should be an element of '%s' <enumeration>", 
                getName().c_str(), _enum_to.c_str()));
            _is_valid = false;
            return ERR_INVALID_TYPE;
        }

        _default_int = a_util::strings::toInt64(strValue);
    }
    
    a_util::result::Result res = a_util::result::SUCCESS;
    _conversions_int.clear();
    for (std::map<std::string, std::string>::iterator it = _conversions.begin(); it != _conversions.end(); ++it)
    {
        std::string strFromVal, strToVal;

        if(isFailed(pEnumFrom->getValue(it->first, strFromVal)) || !a_util::strings::isInt64(strFromVal))
        {
            _config->appendError(a_util::strings::format("<from> attribute '%s' for <conversion> in <transformation> '%s' should be a element of '%s' <enumeration>", 
                it->first.c_str(), getName().c_str(), pEnumFrom->getName().c_str()));
            _is_valid = false;
            res = ERR_INVALID_TYPE;
        } 
        if(isFailed(pEnumTo->getValue(it->second, strToVal)) || !a_util::strings::isInt64(strToVal))
        {
            _config->appendError(a_util::strings::format("<to> attribute '%s' for <conversion> in <transformation> '%s' should be a element of '%s' <enumeration>", 
                it->second.c_str(), getName().c_str(), pEnumTo->getName().c_str()));
            _is_valid = false;
            res = ERR_INVALID_TYPE;
        }

        if(isOk(res))
        {
            _is_valid = true;
            _conversions_int[a_util::strings::toInt64(strFromVal)] = a_util::strings::toInt64(strToVal);
        }
    }
    return res;
}
