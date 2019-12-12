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

#include "ddlprinter.h"
#include "a_util/result/error_def.h"
#include "legacy_error_macros.h"

#include "ddlcontainer.h"

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

#include "ddlversion.h"

namespace ddl
{
    //define all needed error types and values locally
    _MAKE_RESULT(-4, ERR_POINTER)
    _MAKE_RESULT(-16, ERR_NOT_IMPL)
    _MAKE_RESULT(-20, ERR_NOT_FOUND)
    _MAKE_RESULT(-38, ERR_FAILED)
    _MAKE_RESULT(-44, ERR_INVALID_VERSION)

    DDLPrinter::DDLPrinter(const bool& full_out):
        _dom{},
        _version(DDLVersion::ddl_version_invalid), 
        _full_out{full_out},
        _last_path{},
        _forced_version(DDLVersion::ddl_version_invalid)
    {
#if defined(WIN32) && _MSC_VER < 1900
        _set_output_format(_TWO_DIGIT_EXPONENT);
#endif // WIN32
    }

    template<typename T, typename V>
    a_util::result::Result AcceptAll(DDLContainer<T>& oElems, V* pVisitor)
    {
        for (typename DDLContainer<T>::iterator itElem = oElems.begin();
             itElem != oElems.end(); ++itElem)
        {
            RETURN_IF_FAILED((*itElem)->accept(pVisitor));
        }

        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLPrinter::visitDDL(const DDLDescription* poDescription)
    {
        if (!poDescription) { return ERR_POINTER; }
        _dom.reset();
        _dom.fromString("<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\n \
                          <adtf:ddl xmlns:adtf=\"adtf\"> \n \
                          </adtf:ddl>");
        RETURN_IF_FAILED(poDescription->getHeader()->accept(this));

        _dom.getRoot().createChild("units");

        DDLContainer<DDLBaseunit> vecDDLBaseunits = poDescription->getBaseunits();
        RETURN_IF_FAILED(AcceptAll(vecDDLBaseunits, this));

        DDLContainer<DDLPrefix> vecDDLPrefixes = poDescription->getPrefixes();
        RETURN_IF_FAILED(AcceptAll(vecDDLPrefixes, this));

        DDLContainer<DDLUnit> vecDDLUnits = poDescription->getUnits();
        RETURN_IF_FAILED(AcceptAll(vecDDLUnits, this));

        _dom.getRoot().createChild("datatypes");

        DDLContainer<DDLDataType> vecDDLDataTypes = poDescription->getDatatypes();
        RETURN_IF_FAILED(AcceptAll(vecDDLDataTypes, this));

        _dom.getRoot().createChild("enums");

        DDLContainer<DDLEnum> vecDDLEnums = poDescription->getEnums();
        RETURN_IF_FAILED(AcceptAll(vecDDLEnums, this));

        _dom.getRoot().createChild("structs");

        DDLContainer<DDLComplex> vecStructs = poDescription->getStructs();
        RETURN_IF_FAILED(AcceptAll(vecStructs, this));

        _dom.getRoot().createChild("streams");

        DDLContainer<DDLStream> vecStreams = poDescription->getStreams();
        RETURN_IF_FAILED(AcceptAll(vecStreams, this));

        if (poDescription->getHeader()->getLanguageVersion() >= DDLVersion::ddl_version_40)
        {
            _dom.getRoot().createChild("streammetatypes");

            DDLContainer<DDLStreamMetaType> vecStreamMetaTypes = poDescription->getStreamMetaTypes();
            RETURN_IF_FAILED(AcceptAll(vecStreamMetaTypes, this));
        }

        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLPrinter::visit(const DDLHeader* poHeader)
    {
        if (!poHeader) { return ERR_POINTER; }
        a_util::xml::DOMElement oHeaderElem = _dom.getRoot().createChild("header");
        a_util::xml::DOMElement oLangElem = oHeaderElem.createChild("language_version");
        if (_forced_version == DDLVersion::ddl_version_invalid)
        {
            _version = poHeader->getLanguageVersion();
        }
        else
        {
            _version = _forced_version;
        }
        oLangElem.setData(_version.toString());
        a_util::xml::DOMElement oAutElem = oHeaderElem.createChild("author");
        oAutElem.setData(poHeader->getAuthor());
        a_util::xml::DOMElement oDateElem = oHeaderElem.createChild("date_creation");
        a_util::datetime::Date sDateTmp = poHeader->getDateCreation();
        oDateElem.setData(sDateTmp.format("%d.%m.%Y"));
        a_util::xml::DOMElement oChangeElem = oHeaderElem.createChild("date_change");
        sDateTmp = poHeader->getDateChange();
        oChangeElem.setData(sDateTmp.format("%d.%m.%Y"));
        a_util::xml::DOMElement oDescElem = oHeaderElem.createChild("description");
        oDescElem.setData(poHeader->getDescription());
        DDLExtDeclarationVec vecExtDecls = poHeader->getExtDeclarations();
        for (DDLExtDeclarationIt itED = vecExtDecls.begin();
            vecExtDecls.end() != itED; ++itED)
        {
            RETURN_IF_FAILED((*itED)->accept(this));
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLPrinter::visit(const DDLExtDeclaration* poExtDeclaration)
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

    a_util::result::Result DDLPrinter::visit(const DDLBaseunit* poBaseunit)
    {
        if (!poBaseunit) { return ERR_POINTER; }
        a_util::xml::DOMElement oUnits;
        if (!_dom.getRoot().findNode("units", oUnits))
        {
            return ERR_NOT_FOUND;
        }
        a_util::xml::DOMElement oBUElement = oUnits.createChild("baseunit");
        oBUElement.setAttribute("name", poBaseunit->getName());
        oBUElement.setAttribute("symbol", poBaseunit->getSymbol());
        oBUElement.setAttribute("description", poBaseunit->getDescription());
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLPrinter::visit(const DDLPrefix* poPrefix)
    {
        if (!poPrefix) { return ERR_POINTER; }
        a_util::xml::DOMElement oUnits;
        if (!_dom.getRoot().findNode("units", oUnits))
        {
            return ERR_NOT_FOUND;
        }
        a_util::xml::DOMElement oPrefixElement = oUnits.createChild("prefixes");
        oPrefixElement.setAttribute("name", poPrefix->getName());
        oPrefixElement.setAttribute("symbol", poPrefix->getSymbol());
        oPrefixElement.setAttribute("power",
            a_util::strings::toString((int32_t)poPrefix->getPower()));
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLPrinter::visit(const DDLUnit* poUnit)
    {
        if (!poUnit) { return ERR_POINTER; }
        a_util::xml::DOMElement oUnits;
        if (!_dom.getRoot().findNode("units", oUnits))
        {
            return ERR_NOT_FOUND;
        }
        a_util::xml::DOMElement oUnitElement = oUnits.createChild("unit");
        oUnitElement.setAttribute("name", poUnit->getName());
        a_util::xml::DOMElement oTmpElement = oUnitElement.createChild("numerator");
        oTmpElement.setData(poUnit->getNumerator());
        oTmpElement = oUnitElement.createChild("denominator");
        oTmpElement.setData(poUnit->getDenominator());
        oTmpElement = oUnitElement.createChild("offset");
        oTmpElement.setData(a_util::strings::toString(static_cast<double>
            (poUnit->getOffset())));
        std::vector<DDLRefUnit*> vecDDLRefUnits = poUnit->getRefUnits();
        _last_path = a_util::strings::format("%s[@name='%s']",
            oUnitElement.getPath().c_str(),
            oUnitElement.getAttribute("name").c_str());
        for (size_t i = 0; i < vecDDLRefUnits.size(); i++)
        {
            RETURN_IF_FAILED(vecDDLRefUnits[i]->accept(this));
        }
        _last_path.clear();
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLPrinter::visit(const DDLRefUnit* poRefUnit)
    {
        if (!poRefUnit) { return ERR_POINTER; }
        a_util::xml::DOMElement oParent;
        if (!_dom.findNode(_last_path, oParent))
        {
            return ERR_NOT_FOUND;
        }
        a_util::xml::DOMElement oRUElement = oParent.createChild("refUnit");
        oRUElement.setAttribute("name", poRefUnit->getName());
        oRUElement.setAttribute("power",
            a_util::strings::toString((int32_t)poRefUnit->getPower()));
        oRUElement.setAttribute("prefix", poRefUnit->getPrefix());
        if (_full_out)
        {
            a_util::xml::DOMElement oElem;
            if (!_dom.findNode(a_util::strings::format("//units/prefixes[@name=\"%s\"]", poRefUnit->getPrefix().c_str()), oElem))
            {
                RETURN_IF_FAILED(poRefUnit->getPrefixObject()->accept(this));
            }
            if (!_dom.findNode(a_util::strings::format("//units/baseunit[@name=\"%s\"]", poRefUnit->getName().c_str()), oElem))
            {
                if (!_dom.findNode(a_util::strings::format("//units/unit[@name=\"%s\"]", poRefUnit->getName().c_str()), oElem))
                {
                    RETURN_IF_FAILED(poRefUnit->getUnitObject()->accept(this));
                }
            }
        }

        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLPrinter::visit(const DDLDataType* poDataType)
    {
        if (!poDataType) { return ERR_POINTER; }
        a_util::xml::DOMElement oDatatypes;
        if (!_dom.getRoot().findNode("datatypes", oDatatypes))
        {
            return ERR_NOT_FOUND;
        }
        a_util::xml::DOMElement oDTElement = oDatatypes.createChild("datatype");
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
        if(poDataType->getArraysize() > 1)
        {
            oDTElement.setAttribute("arraysize",
                a_util::strings::toString(static_cast<uint32_t>
                (poDataType->getArraysize())));
        }
        if (!poDataType->getUnit().empty())
        {
            oDTElement.setAttribute("unit", poDataType->getUnit());
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

    a_util::result::Result DDLPrinter::visit(const DDLComplex* poStruct)
    {
        if (!poStruct) { return ERR_POINTER; }
        a_util::xml::DOMElement oStructs;
        if (_dom.findNode(a_util::strings::format("//structs/struct[@name=\"%s\"]", poStruct->getName().c_str()), oStructs))
        {
            //already added
            return a_util::result::SUCCESS;
        }
        if (!_dom.getRoot().findNode("structs", oStructs))
        {
            return ERR_NOT_FOUND;
        }
        a_util::xml::DOMElement oStructElement = oStructs.createChild("struct");
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
        if (_version != poStruct->getDDLVersion())
        {
            oStructElement.setAttribute("ddlversion", poStruct->getDDLVersion().toString());
        }

        std::vector<DDLElement*> vecDDLElements = poStruct->getElements();
        for (size_t i = 0; i < vecDDLElements.size(); i++)
        {
            _last_path = a_util::strings::format("%s[@name='%s']",
                                            oStructElement.getPath().c_str(), poStruct->getName().c_str());
            RETURN_IF_FAILED(vecDDLElements[i]->accept(this));
        }
        _last_path.clear();
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLPrinter::visit(const DDLElement* poElement)
    {
        if (!poElement) { return ERR_POINTER; }
        a_util::xml::DOMElement oParent;
        a_util::xml::DOMElement oDomElem;
        if (!_dom.findNode(_last_path, oParent))
        {
            return ERR_NOT_FOUND;
        }
        a_util::xml::DOMElement oElem = oParent.createChild("element");
        oElem.setAttribute("type", poElement->getType());
        oElem.setAttribute("name", poElement->getName());

        if (_full_out)
        {
            if (!_dom.findNode(a_util::strings::format("//structs/struct[@name=\"%s\"]", poElement->getType().c_str()), oDomElem))
            {
                if (!_dom.findNode(a_util::strings::format("//datatypes/datatype[@name=\"%s\"]", poElement->getType().c_str()), oDomElem))
                {
                    if (!_dom.findNode(a_util::strings::format("//enums/enum[@name=\"%s\"]", poElement->getType().c_str()), oDomElem))
                    {
                        RETURN_IF_FAILED(poElement->getTypeObject()->accept(this));
                    }
                }
            }
        }

        a_util::xml::DOMElement oDeserializedElement = oElem;
        a_util::xml::DOMElement oSerializedElement = oElem;
        if (_version >= DDLVersion::ddl_version_40)
        {
            oDeserializedElement = oElem.createChild("deserialized");
            oSerializedElement = oElem.createChild("serialized");
        }
        oSerializedElement.setAttribute("bytepos",
            a_util::strings::toString(static_cast<uint32_t>
            (poElement->getBytepos())));
        if ((_version <  DDLVersion::ddl_version_11 && poElement->getBitpos() != 1) ||
            (_version >= DDLVersion::ddl_version_11 && poElement->getBitpos() > 0))
        {
            oSerializedElement.setAttribute("bitpos",
                a_util::strings::toString(static_cast<uint32_t>
                (poElement->getBitpos())));
        }
        if (poElement->getNumBits() > 0)
        {
            oSerializedElement.setAttribute("numbits",
                a_util::strings::toString(static_cast<uint32_t>
                (poElement->getNumBits())));
        }
        oSerializedElement.setAttribute("byteorder",
            DDLByteorder::toString(poElement->getByteorder()));
        oDeserializedElement.setAttribute("alignment",
            DDLAlignment::toString(poElement->getAlignment()));

        if (!poElement->getDescription().empty())
        {
            oElem.setAttribute("description", poElement->getDescription());
        }
        if(poElement->getArraysize() == 0 || !poElement->getArraySizeSource().empty())
        {
            oElem.setAttribute("arraysize", poElement->getArraySizeSource());
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
        }

        if (!poElement->getComment().empty())
        {
            oElem.setAttribute("comment", poElement->getComment());
        }

        if (!poElement->getConstantValue().empty())
        {
            oElem.setAttribute("value", poElement->getConstantValue());
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

    a_util::result::Result DDLPrinter::visit(const DDLStream* poStream)
    {
        if (!poStream) { return ERR_POINTER; }
        a_util::xml::DOMElement oStreams;
        if (!_dom.getRoot().findNode("streams", oStreams))
        {
            return ERR_NOT_FOUND;
        }
        a_util::xml::DOMElement oStreamElement = oStreams.createChild("stream");
        if (!poStream->getName().empty())
        {
            oStreamElement.setAttribute("name", poStream->getName());
        }
        oStreamElement.setAttribute("type", poStream->getType());
        if (_full_out)
        {
            a_util::xml::DOMElement oElem;
            if (!_dom.findNode(a_util::strings::format("//structs/struct[@name=\"%s\"]", poStream->getType().c_str()), oElem))
            {
                RETURN_IF_FAILED(poStream->getTypeObject()->accept(this));
            }
        }
        if (!poStream->getDescription().empty())
        {
            oStreamElement.setAttribute("description",
                poStream->getDescription());
        }
        std::vector<DDLStreamStruct*> vecStructs = poStream->getStructs();
        _last_path = a_util::strings::format("%s[@name='%s']",
            oStreamElement.getPath().c_str(),
            oStreamElement.getAttribute("name").c_str());
        for (size_t i = 0; i < vecStructs.size(); i++)
        {
            RETURN_IF_FAILED(vecStructs[i]->accept(this));
        }
        _last_path.clear();
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLPrinter::visit(const DDLStreamStruct* poStreamStruct)
    {
        if (!poStreamStruct) { return ERR_POINTER; }
        a_util::xml::DOMElement oParent;
        if (!_dom.findNode(_last_path, oParent))
        {
            return ERR_NOT_FOUND;
        }
        a_util::xml::DOMElement oStruct = oParent.createChild("struct");
        oStruct.setAttribute("type", poStreamStruct->getType());
        if (!poStreamStruct->getName().empty())
        {
            oStruct.setAttribute("name", poStreamStruct->getName());
        }
        if (_full_out)
        {
             a_util::xml::DOMElement oElem;
             if (!_dom.findNode(a_util::strings::format("//structs/struct[@name=\"%s\"]", poStreamStruct->getType().c_str()), oElem))
             {
                RETURN_IF_FAILED(poStreamStruct->getTypeObject()->accept(this));
             }
        }
        oStruct.setAttribute("bytepos",
            a_util::strings::toString(static_cast<uint32_t>
            (poStreamStruct->getBytepos())));
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLPrinter::visit(const DDLEnum* poEnum )
    {
        if (!poEnum) { return ERR_POINTER; }
        a_util::xml::DOMElement oEnums;
        if (_dom.findNode(a_util::strings::format("//enums/enum[@name=\"%s\"]", poEnum->getName().c_str()), oEnums))
        {
            //already added
            return a_util::result::SUCCESS;
        }
        if (!_dom.getRoot().findNode("enums", oEnums))
        {
            return ERR_NOT_FOUND;
        }
        a_util::xml::DOMElement oEnumElement = oEnums.createChild("enum");
        oEnumElement.setAttribute("name", poEnum->getName());

        if (!poEnum->getType().empty())
        {
            oEnumElement.setAttribute("type", poEnum->getType());
        }

        EnumNameValueVec vecNameValues = poEnum->getValues();
        for (EnumNameValueVec::iterator itElement = vecNameValues.begin(); itElement != vecNameValues.end(); ++itElement)
        {
            a_util::xml::DOMElement oEnumValueElement = oEnumElement.createChild("element");

            oEnumValueElement.setAttribute("name", itElement->first);
            oEnumValueElement.setAttribute("value", itElement->second);
        }

        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLPrinter::visit(const DDLStreamMetaType* poStreamMetaType)
    {
        a_util::xml::DOMElement oStreamMetaTypes;
        if (!_dom.getRoot().findNode("streammetatypes", oStreamMetaTypes))
        {
            return ERR_NOT_FOUND;
        }
        a_util::xml::DOMElement oElement = oStreamMetaTypes.createChild("streammetatype");
        oElement.setAttribute("name", poStreamMetaType->getName());
        oElement.setAttribute("version", poStreamMetaType->getVersion());
        if (poStreamMetaType->getParentObject())
        {
            oElement.setAttribute("parent", poStreamMetaType->getParent());
        }

        const DDLPropertyVec& vecProperties = poStreamMetaType->getProperties();
        for (DDLPropertyVec::const_iterator it = vecProperties.cbegin(); it != vecProperties.cend(); ++it)
        {
            a_util::xml::DOMElement oPropertyNode = oElement.createChild("property");
            oPropertyNode.setAttribute("name", (*it)->getName());
            oPropertyNode.setAttribute("type", (*it)->getType());
        }

        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLPrinter::visit(const DDLProperty* poProperty)
    {
        return ERR_NOT_IMPL;
    }

    std::string DDLPrinter::getXML() const
    {
        return std::string(_dom.toString());
    }

    a_util::result::Result DDLPrinter::toFile(const std::string& strFilename) const
    {
        if (!_dom.save(strFilename))
        {
            return ERR_FAILED;
        }

        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLPrinter::forceVersion(const DDLVersion& forced_version)
    {
        if (!forced_version.isValidVersion())
        {
            return ERR_INVALID_VERSION;
        }
        else
        {
            _forced_version = forced_version;
            return a_util::result::SUCCESS;
        }
    }
}   // namespace ddl
