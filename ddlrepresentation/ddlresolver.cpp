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

#include "ddlresolver.h"

#include "a_util/result/error_def.h"
#include "legacy_error_macros.h"

#include "ddldescription.h"
#include "ddlunit.h"
#include "ddlbaseunit.h"
#include "ddlprefix.h"
#include "ddldatatype.h"
#include "ddlenum.h"
#include "ddlcomplex.h"
#include "ddlstream.h"
#include "ddlstreammetatype.h"
#include "ddlheader.h"
#include "ddlextdeclaration.h"
#include "ddlrefunit.h"
#include "ddlelement.h"
#include "ddlstreamstruct.h"


namespace ddl
{
    //define all needed error types and values locally
    _MAKE_RESULT(-4, ERR_POINTER)
    _MAKE_RESULT(-5, ERR_INVALID_ARG)
    _MAKE_RESULT(-16, ERR_NOT_IMPL)
    _MAKE_RESULT(-20, ERR_NOT_FOUND)
    _MAKE_RESULT(-37, ERR_NOT_INITIALIZED)

    DDLResolver::DDLResolver() :
        _dom{},
        _target{},
        _version(DDLVersion::ddl_version_invalid),
        _stack_parents{},
        _resolved_types{},
        _units{},
        _data_types{},
        _enums{},
        _structs{},
        _streams{}
    {
#if defined(WIN32) && _MSC_VER < 1900
        _set_output_format(_TWO_DIGIT_EXPONENT);
#endif // WIN32
    }

    DDLResolver::DDLResolver(const std::string& strTarget) :
        DDLResolver()
    {
        _target = strTarget;
    }

    a_util::result::Result DDLResolver::createBase(const DDLDescription* poDescription)
    {
        _resolved_types.clear();
        _dom.fromString("<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\n \
                          <adtf:ddl xmlns:adtf=\"adtf\"> \n \
                          </adtf:ddl>");
        RETURN_IF_FAILED(poDescription->getHeader()->accept(this));

        _units = _dom.getRoot().createChild("units");
        _data_types = _dom.getRoot().createChild("datatypes");
        if (_version >= DDLVersion::ddl_version_20)
        {
            _enums = _dom.getRoot().createChild("enums");
        }
        _structs = _dom.getRoot().createChild("structs");
        _streams = _dom.getRoot().createChild("streams");

        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLResolver::visitDDL(const DDLDescription* poDescription)
    {
        if (!poDescription) { return ERR_POINTER; }
        if (_target.empty())
        {
            // no target specified
            return ERR_NOT_INITIALIZED;
        }

        RETURN_IF_FAILED(createBase(poDescription));

        a_util::result::Result nResult = ERR_NOT_FOUND;

        DDLUnitVec vecDDLUnits = poDescription->getUnits();
        DDLUnitIt itUnitFound = std::find_if(vecDDLUnits.begin(), vecDDLUnits.end(),
                                              DDLCompareFunctor<>(_target));

        if (vecDDLUnits.end() != itUnitFound)
        {
            nResult = (*itUnitFound)->accept(this);
            return nResult;
        }

        DDLDTVec vecDTs = poDescription->getDatatypes();
        DDLDTIt itDTFound = std::find_if(vecDTs.begin(), vecDTs.end(), DDLCompareFunctor<>(_target));

        if (itDTFound != vecDTs.end())
        {
            // primitive data type
            nResult = (*itDTFound)->accept(this);
            return nResult;
        }

        DDLComplexVec vecStructs = poDescription->getStructs();
        DDLComplexIt itStructFound = std::find_if(vecStructs.begin(),
                                                   vecStructs.end(), DDLCompareFunctor<>(_target));

        if (itStructFound != vecStructs.end())
        {
            // complex data type
            nResult = (*itStructFound)->accept(this);
            return nResult;
        }

        DDLStreamVec vecStreams = poDescription->getStreams();
        DDLStreamIt itStreamFound = std::find_if(vecStreams.begin(),
                                                  vecStreams.end(), DDLCompareFunctor<>(_target));

        if (itStreamFound != vecStreams.end())
        {
            // stream
            nResult = (*itStreamFound)->accept(this);
            return nResult;
        }
        return nResult;
    }

    a_util::result::Result DDLResolver::visit(const DDLHeader* poHeader)
    {
        if (!poHeader) { return ERR_POINTER; }
        a_util::xml::DOMElement oHeaderElement = _dom.getRoot().createChild("header");
        a_util::xml::DOMElement oTmpElement;
        oTmpElement = oHeaderElement.createChild("language_version");
        _version = poHeader->getLanguageVersion();
        oTmpElement.setData(_version.toString());
        oTmpElement = oHeaderElement.createChild("author");
        oTmpElement.setData(poHeader->getAuthor());
        // Use current date for creation and change date
        oTmpElement = oHeaderElement.createChild("date_creation");
        oTmpElement.setData(poHeader->getDateCreation().format("%d.%m.%Y"));
        oTmpElement = oHeaderElement.createChild("date_change");
        oTmpElement.setData(poHeader->getDateChange().format("%d.%m.%Y"));
        oTmpElement = oHeaderElement.createChild("description");
        oTmpElement.setData(poHeader->getDescription());
        DDLExtDeclarationVec vecExtDecls = poHeader->getExtDeclarations();
        for (DDLExtDeclarationIt itED = vecExtDecls.begin();
            vecExtDecls.end() != itED; ++itED)
        {
            RETURN_IF_FAILED((*itED)->accept(this));
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLResolver::visit(const DDLExtDeclaration* poExtDeclaration)
    {
        if (!poExtDeclaration) { return ERR_POINTER; }
        a_util::xml::DOMElement oHeader;
        if (!_dom.getRoot().findNode("header", oHeader))
        {
            return ERR_NOT_FOUND;
        }
        a_util::xml::DOMElement oEDElement = oHeader.createChild("ext_declaration");
        oEDElement.setAttribute("key", poExtDeclaration->getKey());
        oEDElement.setAttribute("value", poExtDeclaration->getValue());
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLResolver::visit(const DDLBaseunit* poBaseunit)
    {
        if (!poBaseunit) { return ERR_POINTER; }
        a_util::xml::DOMElement oBUElement;
        if (_units.findNode(a_util::strings::format("baseunit[@name='%s']",
            poBaseunit->getName().c_str()), oBUElement))
        {
            // this element was already visited
            return a_util::result::SUCCESS;
        }
        oBUElement = _units.createChild("baseunit");
        oBUElement.setAttribute("name", poBaseunit->getName());
        oBUElement.setAttribute("symbol", poBaseunit->getSymbol());
        oBUElement.setAttribute("description", poBaseunit->getDescription());
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLResolver::visit(const DDLPrefix* poPrefix)
    {
        if (!poPrefix) { return ERR_POINTER; }
        a_util::xml::DOMElement oUnits;
        a_util::xml::DOMElement oPrefixElement;
        if (!_dom.getRoot().findNode("units", oUnits))
        {
            return ERR_NOT_FOUND;
        }
        if (oUnits.findNode(a_util::strings::format("prefixes[@name='%s']",
            poPrefix->getName().c_str()), oPrefixElement))
        {
            // this element was already visited
            return a_util::result::SUCCESS;
        }
        oPrefixElement = oUnits.createChild("prefixes");
        oPrefixElement.setAttribute("name", poPrefix->getName());
        oPrefixElement.setAttribute("symbol", poPrefix->getSymbol());
        oPrefixElement.setAttribute("power",
            a_util::strings::toString((int32_t)poPrefix->getPower()));
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLResolver::visit(const DDLUnit* poUnit)
    {
        if (!poUnit) { return ERR_POINTER; }
        a_util::xml::DOMElement oUnitElement;
        a_util::xml::DOMElement oTmpElement;
        if (_units.findNode(a_util::strings::format("unit[@name='%s']",
            poUnit->getName().c_str()), oUnitElement))
        {
            // this element was already visited
            return a_util::result::SUCCESS;
        }
        oUnitElement = _units.createChild("unit");
        oUnitElement.setAttribute("name", poUnit->getName());
        oTmpElement = oUnitElement.createChild("numerator");
        oTmpElement.setData(poUnit->getNumerator());
        oTmpElement = oUnitElement.createChild("denominator");
        oTmpElement.setData(poUnit->getDenominator());
        oTmpElement = oUnitElement.createChild("offset");
        oTmpElement.setData(a_util::strings::toString(static_cast<double>
            (poUnit->getOffset())));
        std::vector<DDLRefUnit*> vecDDLRefUnits = poUnit->getRefUnits();
        _stack_parents.push(oUnitElement);
        for (size_t i = 0; i < vecDDLRefUnits.size(); i++)
        {
            RETURN_IF_FAILED(vecDDLRefUnits[i]->accept(this));
        }
        _stack_parents.pop();
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLResolver::visit(const DDLRefUnit* poRefUnit)
    {
        if (!poRefUnit) { return ERR_POINTER; }
        a_util::xml::DOMElement oRUElement;
        oRUElement = _stack_parents.top().createChild("refUnit");
        oRUElement.setAttribute("name", poRefUnit->getName());
        RETURN_IF_FAILED(poRefUnit->getUnitObject()->accept(this));
        oRUElement.setAttribute("power",
            a_util::strings::toString((int32_t)poRefUnit->getPower()));
        oRUElement.setAttribute("prefix", poRefUnit->getPrefix());
        return poRefUnit->getPrefixObject()->accept(this);
    }

    a_util::result::Result DDLResolver::visit(const DDLDataType* poDataType)
    {
        if (!poDataType) { return ERR_POINTER; }
        a_util::xml::DOMElement oDTElement;
        if (_resolved_types.find(poDataType->getName()) != _resolved_types.end())
        {
            return a_util::result::SUCCESS;
        }
        _resolved_types.insert(poDataType->getName());

        oDTElement = _data_types.createChild("datatype");
        if (_version <  DDLVersion::ddl_version_11)
        {
            oDTElement.setAttribute("type", poDataType->getName());
        }
        else
        {
            oDTElement.setAttribute("name", poDataType->getName());
        }
        oDTElement.setAttribute("size",
            a_util::strings::toString(static_cast<uint32_t>(poDataType->getNumBits())));
        if (!poDataType->getDescription().empty())
        {
            oDTElement.setAttribute("description",
                poDataType->getDescription());
        }
        if (poDataType->getArraysize() > 1)
        {
            oDTElement.setAttribute("arraysize",
                a_util::strings::toString(static_cast<uint32_t>
                (poDataType->getArraysize())));
        }
        if (!poDataType->getUnit().empty())
        {
            oDTElement.setAttribute("unit", poDataType->getUnit());
            RETURN_IF_FAILED(poDataType->getUnitObject()->accept(this));
        }
        if (_version >= DDLVersion::ddl_version_30)
        {
            // min and max values are supported since DDL 3.0
            if (poDataType->isMinValid())
            {
                oDTElement.setAttribute("min", poDataType->getMinValue());
            }
            if (poDataType->isMaxValid())
            {
                oDTElement.setAttribute("max", poDataType->getMaxValue());
            }
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLResolver::visit(const DDLComplex* poStruct)
    {
        if (!poStruct) { return ERR_POINTER; }
        a_util::xml::DOMElement oStructElement;
        if (_resolved_types.find(poStruct->getName()) != _resolved_types.end())
        {
            return a_util::result::SUCCESS;
        }
        _resolved_types.insert(poStruct->getName());

        oStructElement = _structs.createChild("struct");
        oStructElement.setAttribute("name", poStruct->getName());
        oStructElement.setAttribute("version",
            a_util::strings::toString(static_cast<uint32_t>(poStruct->getVersion())));
        if (!poStruct->getComment().empty())
        {
            oStructElement.setAttribute("comment", poStruct->getComment());
        }
        if (_version >= DDLVersion::ddl_version_11)
        {
            oStructElement.setAttribute("alignment",
                DDLAlignment::toString(poStruct->getAlignment()));
        }
        _stack_parents.push(oStructElement);
        std::vector<DDLElement*> vecDDLElements = poStruct->getElements();
        for (size_t i = 0; i < vecDDLElements.size(); i++)
        {
            RETURN_IF_FAILED(vecDDLElements[i]->accept(this));
        }
        _stack_parents.pop();
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLResolver::visit(const DDLElement* poElement)
    {
        if (!poElement) { return ERR_POINTER; }
        a_util::xml::DOMElement oElem;
        oElem = _stack_parents.top().createChild("element");
        oElem.setAttribute("type", poElement->getType());
        IDDLDataType* pTypeObject = poElement->getTypeObject();
        if (!pTypeObject)
        {
            return ERR_NOT_FOUND;
        }
        RETURN_IF_FAILED(pTypeObject->accept(this));
        oElem.setAttribute("name", poElement->getName());
        oElem.setAttribute("bytepos",
            a_util::strings::toString(static_cast<uint32_t>
            (poElement->getBytepos())));
        if ((_version <  DDLVersion::ddl_version_11 && poElement->getBitpos() != 1) ||
            (_version >= DDLVersion::ddl_version_11 && poElement->getBitpos() > 0))
        {
            oElem.setAttribute("bitpos",
                a_util::strings::toString(static_cast<uint32_t>
                (poElement->getBitpos())));
        }
        if (poElement->getNumBits() > 0)
        {
            if (1 == poElement->getArraysize())
            {
                // only check the "numbits"-attribute for data-types (DDLDataType), because
                // "numbits"-attribute for complex-types (DDLComplex) may tempt to configuration errors
                DDLDataType *poDT = dynamic_cast<DDLDataType*>(pTypeObject);
                if (NULL != poDT)
                {
                    if (poElement->getNumBits() == poDT->getNumBits())
                    {
                        oElem.setAttribute("numbits", a_util::strings::toString(static_cast<uint32_t>
                            (poElement->getNumBits())));
                    }
                    else
                    {
                        return ERR_INVALID_ARG;
                    }
                }
            }
            else
            {
                return ERR_INVALID_ARG;
            }
        }
        if (!poElement->getDescription().empty())
        {
            oElem.setAttribute("description", poElement->getDescription());
        }
        if(poElement->getArraysize() == 0 || !poElement->getArraySizeSource().empty())
        {
            oElem.setAttribute("arraysize",poElement->getArraySizeSource());
        }
        else
        {
            oElem.setAttribute("arraysize",
                a_util::strings::toString(static_cast<uint32_t>
                (poElement->getArraysize())));
        }
        if (!poElement->getUnit().empty())
        {
            oElem.setAttribute("unit", poElement->getUnit());
            RETURN_IF_FAILED(poElement->getUnitObject()->accept(this));
        }
        oElem.setAttribute("byteorder",
            DDLByteorder::toString(poElement->getByteorder()));
        oElem.setAttribute("alignment",
            DDLAlignment::toString(poElement->getAlignment()));
        if (!poElement->getComment().empty())
        {
            oElem.setAttribute("comment", poElement->getComment());
        }

        std::string strConstValue = poElement->getConstantValue();
        if (!strConstValue.empty())
        {
            oElem.setAttribute("value", strConstValue);
        }

        if (_version >= DDLVersion::ddl_version_30)
        {
            // min, max, default, scale, and offset values are supported since DDL 3.0
            if (poElement->isMinValid())
            {
                oElem.setAttribute("min", poElement->getMinValue());
            }

            if (poElement->isMaxValid())
            {
                oElem.setAttribute("max", poElement->getMaxValue());
            }

            if (poElement->isDefaultValid())
            {
                oElem.setAttribute("default", poElement->getDefaultValue());
            }

            if (poElement->isScaleValid())
            {
                oElem.setAttribute("scale", poElement->getScaleValue());
            }

            if (poElement->isOffsetValid())
            {
                oElem.setAttribute("offset", poElement->getOffsetValue());
            }
        }

        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLResolver::visit(const DDLStream* poStream)
    {
        if (!poStream) { return ERR_POINTER; }
        a_util::xml::DOMElement oStreamElement;
        oStreamElement = _streams.createChild("stream");
        if (!poStream->getName().empty())
        {
            oStreamElement.setAttribute("name", poStream->getName());
        }
        oStreamElement.setAttribute("type", poStream->getType());
        RETURN_IF_FAILED(poStream->getTypeObject()->accept(this));
        if (!poStream->getDescription().empty())
        {
            oStreamElement.setAttribute("description",
                poStream->getDescription());
        }
        std::vector<DDLStreamStruct*> vecStructs = poStream->getStructs();
        _stack_parents.push(oStreamElement);
        for (size_t i = 0; i < vecStructs.size(); i++)
        {
            RETURN_IF_FAILED(vecStructs[i]->accept(this));
        }
        _stack_parents.pop();
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLResolver::visit(const DDLStreamStruct* poStreamStruct)
    {
        if (!poStreamStruct) { return ERR_POINTER; }
        a_util::xml::DOMElement oStruct;
        oStruct = _stack_parents.top().createChild("struct");
        oStruct.setAttribute("type", poStreamStruct->getType());
        RETURN_IF_FAILED(poStreamStruct->getTypeObject()->accept(this));
        if (!poStreamStruct->getName().empty())
        {
            oStruct.setAttribute("name", poStreamStruct->getName());
        }
        oStruct.setAttribute("bytepos",
            a_util::strings::toString(static_cast<uint32_t>
            (poStreamStruct->getBytepos())));
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLResolver::visit(const DDLEnum* poEnum)
    {
        if (!poEnum) { return ERR_POINTER; }
        a_util::xml::DOMElement oDTElement;
        if (_enums.findNode(a_util::strings::format("enum[@name='%s']",
            poEnum->getName().c_str()), oDTElement))
        {
            // this element was already visited
            return a_util::result::SUCCESS;
        }
        oDTElement = _enums.createChild("enum");

        oDTElement.setAttribute("name", poEnum->getName());
       // oDTElement->setAttribute("value", poEnum->getValue());
        oDTElement.setAttribute("type", poEnum->getType());

         EnumNameValueVec vecNameValues = poEnum->getValues();
         a_util::xml::DOMElement oEnumElement;
         for (EnumNameValueVec::iterator itElem = vecNameValues.begin(); itElem != vecNameValues.end(); ++itElem)
         {
             oEnumElement = oDTElement.createChild("element");

             oEnumElement.setAttribute("name", itElem->first);
             oEnumElement.setAttribute("value", itElem->second);
         }


        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLResolver::visit(const DDLStreamMetaType* poStreamMetaType)
    {
        return ERR_NOT_IMPL;
    }

    a_util::result::Result DDLResolver::visit(const DDLProperty* poProperty)
    {
        return ERR_NOT_IMPL;
    }

    std::string DDLResolver::getResolvedXML() const
    {
        std::string strResult = _dom.toString().c_str();
        if (strResult == "<xml />")
        {
            // DOM is empty
            return a_util::strings::empty_string;
        }
        return strResult;
    }

    void DDLResolver::setTargetName(const std::string& name)
    {
        _target = name;
    }

}   // namespace ddl
