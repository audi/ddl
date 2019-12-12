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

#include "ddlinspector.h"
#include <algorithm>
#include <limits>
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

#include "ddlversion.h"

namespace ddl
{
    //define all needed error types and values locally
    _MAKE_RESULT(-4, ERR_POINTER)
    _MAKE_RESULT(-5, ERR_INVALID_ARG)
    _MAKE_RESULT(-8, ERR_INVALID_HANDLE)
    _MAKE_RESULT(-16, ERR_NOT_IMPL)
    _MAKE_RESULT(-20, ERR_NOT_FOUND)
    _MAKE_RESULT(-43, ERR_EMPTY)

    /// Alignment target
    static size_t const s_szAlignSize = 4;

    DDLInspector::DDLInspector(bool bAutoCorrect /* = false */) :
        _auto_correct{bAutoCorrect},
        _base_unit_names{},
        _complex_names{},
        _data_types_names{},
        _ddl_desc{},
        _enum_names{},
        _has_dynamic_arrays{},
        _has_struct_dynamic_arrays{},
        _prefix_names{},
        _stream_names{},
        _struct_infos{},
        _suggestions{},
        _unit_names{},
        _version(0, 0),
        _warning_level{WarningLevel::moderate}
    {
        InitNewCheck();
    }

    a_util::result::Result DDLInspector::visitDDL(DDLDescription* poDescription)
    {
        InitNewCheck();

        if (!poDescription) { return ERR_POINTER; }

        _ddl_desc = poDescription;
        a_util::result::Result nResult = a_util::result::SUCCESS;
        a_util::result::Result nRes = a_util::result::SUCCESS;

        nResult = poDescription->getHeader()->accept(this);

        _has_dynamic_arrays = false;

        //// validate Base Unit types
        DDLBaseunitVec vecBaseUnits = poDescription->getBaseunits();
        for (DDLBaseunitIt itUnit = vecBaseUnits.begin(); itUnit != vecBaseUnits.end(); ++itUnit)
        {
            nRes = (*itUnit)->accept(this);
            if (isFailed(nRes))
            {
                nResult = nRes;
            }
        }

        //// validate Prefix types
        DDLPrefixVec vecPrefix = poDescription->getPrefixes();
        for (DDLPrefixIt itPrefix = vecPrefix.begin(); itPrefix != vecPrefix.end(); ++itPrefix)
        {
            nRes = (*itPrefix)->accept(this);
            if (isFailed(nRes))
            {
                nResult = nRes;
            }
        }

        //// validate Unit data types
        DDLUnitVec vecUnits = poDescription->getUnits();
        for (DDLUnitIt itUnit = vecUnits.begin(); itUnit != vecUnits.end(); ++itUnit)
        {
            nRes = (*itUnit)->accept(this);
            if (isFailed(nRes))
            {
                nResult = nRes;
            }
        }

        // validate primitive data types
        DDLDTVec vecDTs = poDescription->getDatatypes();
        for (DDLDTIt itDT = vecDTs.begin(); itDT != vecDTs.end(); ++itDT)
        {
            nRes = (*itDT)->accept(this);
            if (isFailed(nRes))
            {
                nResult = nRes;
            }
        }

        //// validate Enum types
        DDLEnumVec vecEnums = poDescription->getEnums();
        for (DDLEnumIt itEnum = vecEnums.begin(); itEnum != vecEnums.end(); ++itEnum)
        {
            nRes = (*itEnum)->accept(this);
            if (isFailed(nRes))
            {
                nResult = nRes;
            }
        }

        // validate complex data types
        DDLComplexVec vecStructs = poDescription->getStructs();
        _complex_names.clear();
        for (DDLComplexIt itStruct = vecStructs.begin(); itStruct != vecStructs.end(); ++itStruct)
        {
            if (_has_struct_dynamic_arrays && !_has_dynamic_arrays)
            {
                _has_dynamic_arrays = true;
            }

            _has_struct_dynamic_arrays = false;

            if (_complex_names.end() != _complex_names.find((*itStruct)->getName()))
            {
                addSuggestion(a_util::strings::format("The Name '%s' is duplicate",
                    (*itStruct)->getName().c_str()), importer_error);
                nResult = ERR_INVALID_ARG;
            }
            else
            {
                _complex_names.insert((*itStruct)->getName());
            }

            nRes = (*itStruct)->accept(this);
            if (isFailed(nRes))
            {
                // store error but continue with the remaining structs
                nResult = nRes;
            }
        }

        _ddl_desc->getHeader()->setDateChange(a_util::datetime::getCurrentLocalDate());

        //// validate Stream types
        DDLStreamVec vecStream = poDescription->getStreams();
        for (DDLStreamIt itStream = vecStream.begin(); itStream != vecStream.end(); ++itStream)
        {
            nRes = (*itStream)->accept(this);
            if (isFailed(nRes))
            {
                nResult = nRes;
            }
        }

        if (_has_dynamic_arrays)
        {
            addSuggestion(a_util::strings::format("Dynamic Arrays detected. (Performance is not good)."),
                importer_warning);
            nResult = ERR_INVALID_ARG;
        }
        RETURN_IF_FAILED(nResult);
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLInspector::visit(DDLHeader* poHeader)
    {
        if (NULL == poHeader)
        {
            addSuggestion(a_util::strings::format("DDL does not contain a header."),
                importer_error);
            return ERR_POINTER;
        }
        _version = poHeader->getLanguageVersion();
        if (!_version.isValidVersion())
        {
            addSuggestion(a_util::strings::format("The version %s is an invalid DDL version.",
                _version.toString().c_str()), importer_error);
            return ERR_INVALID_ARG;
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLInspector::visit(DDLExtDeclaration* poExtDeclaration)
    {
        // should not be called
        return ERR_NOT_IMPL;
    }

    a_util::result::Result DDLInspector::visit(DDLBaseunit* poBaseunit)
    {
        if (!poBaseunit) { return ERR_POINTER; }
        if (_base_unit_names.end() != _base_unit_names.find(poBaseunit->getName()))
        {
            addSuggestion(a_util::strings::format("Duplicate identifier (Baseunit Type) '%s'.",
                poBaseunit->getName().c_str()), importer_error);
            return ERR_INVALID_ARG;
        }
        else
        {
            _base_unit_names.insert(poBaseunit->getName());
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLInspector::visit(DDLPrefix* poPrefix)
    {
        if (!poPrefix) { return ERR_POINTER; }
        // Check if an prefix name already exist.
        if (_prefix_names.end() != _prefix_names.find(poPrefix->getName()))
        {
            addSuggestion(a_util::strings::format("Duplicate identifier (Prefix Type) '%s'.",
                poPrefix->getName().c_str()), importer_error);
            return ERR_INVALID_ARG;
        }
        else
        {
            _prefix_names.insert(poPrefix->getName());
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLInspector::visit(DDLUnit* poUnit)
    {
        if (!poUnit) { return ERR_POINTER; }
        if (poUnit->getDenominator() == "0")
        {
            addSuggestion(a_util::strings::format("Unit '%s' with denominator '0' is not defined.",
                poUnit->getName().c_str()), importer_warning);
        }
        DDLRefUnitVec vecRUs = poUnit->getRefUnits();
        for (DDLRefUnitIt itRU = vecRUs.begin(); vecRUs.end() != itRU; ++itRU)
        {
            RETURN_IF_FAILED((*itRU)->accept(this));
        }
        _unit_names.insert(poUnit->getName());
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLInspector::visit(DDLRefUnit* poRefUnit)
    {
        if (!poRefUnit) { return ERR_POINTER; }
        if (_base_unit_names.end() == _base_unit_names.find(poRefUnit->getName()))
        {
            if (_base_unit_names.end() == _base_unit_names.find(poRefUnit->getName()))
            {
                addSuggestion(a_util::strings::format("(Base-)Unit '%s' not found.",
                    poRefUnit->getName().c_str()), importer_error);
                return ERR_INVALID_HANDLE;
            }
        }
        if (_prefix_names.end() == _prefix_names.find(poRefUnit->getPrefix()))
        {
            addSuggestion(a_util::strings::format("Prefix '%s' not found.",
                poRefUnit->getName().c_str()), importer_error);
            return ERR_INVALID_HANDLE;
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLInspector::visit(DDLDataType* poDataType)
    {
        if (!poDataType) { return ERR_POINTER; }
        std::string strType = poDataType->getName();
        if (poDataType->isDynamic())
        {
            if (_version <  DDLVersion::ddl_version_12)
            {
                addSuggestion(a_util::strings::format("Datatype '%s' is a dynamic array and therefore not supported.",
                    strType.c_str()), importer_error);
                return ERR_INVALID_ARG;
            }
        }

        // Check if an element name already exist as complex data type.
        if (_complex_names.end() != _complex_names.find(strType))
        {
            addSuggestion(a_util::strings::format("Duplicate identifier (Complex_Datatype) '%s'.",
                strType.c_str()), importer_error);
            return ERR_INVALID_ARG;
        }

        // Check if an element name already exist as enum.
        if (_enum_names.end() != _enum_names.find(strType))
        {
            addSuggestion(a_util::strings::format("Duplicate identifier (Enum Type) '%s'.",
                strType.c_str()), importer_error);
            return ERR_INVALID_ARG;
        }

        // Check if an element name already exist.
        if (_data_types_names.end() != _data_types_names.find(strType))
        {
            addSuggestion(a_util::strings::format("Duplicate identifier (Data Type) '%s'.",
                strType.c_str()), importer_error);
            return ERR_INVALID_ARG;
        }

        std::string strUnit = poDataType->getUnit();
        if (!strUnit.empty())
        {
            if (_base_unit_names.end() == _base_unit_names.find(strUnit))
            {
                if (_unit_names.end() == _unit_names.find(strUnit))
                {
                    addSuggestion(a_util::strings::format("(Base-)Unit '%s' not found.",
                        strUnit.c_str()), importer_error);
                    return ERR_INVALID_HANDLE;
                }
            }
        }
        _data_types_names.insert(strType);
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLInspector::visit(DDLEnum* poEnum)
    {
        if (!poEnum) { return ERR_POINTER; }
        EnumNameValueVec vecNameValues = poEnum->getValues();
        EnumNameValueVec::iterator it = vecNameValues.begin();
        std::set<std::string> setEnumsElementNames;
        std::set<std::string> enumValues;
        for (; it != vecNameValues.end(); ++it)
        {
            if (setEnumsElementNames.end() != setEnumsElementNames.find(it->first))
            {
                addSuggestion(a_util::strings::format("Duplicate identifier (Enum Element Type) '%s'.",
                    it->first.c_str()), importer_warning);
            }
            else
            {
                setEnumsElementNames.insert(it->first);
            }

            if (enumValues.end() != enumValues.find(it->second))
            {
                addSuggestion(a_util::strings::format("Duplicate Element value (Enum Element Value) '%s' '%s'.",
                    it->first.c_str(), it->second.c_str()), importer_warning);
            }
            else
            {
                enumValues.insert(it->second);
            }
        }

        // Check if an element name already exist as data type.
        if (_data_types_names.end() != _data_types_names.find(poEnum->getName()))
        {
            addSuggestion(a_util::strings::format("Duplicate identifier (Data Type) '%s'.",
                poEnum->getName().c_str()), importer_error);
            return ERR_INVALID_ARG;
        }

        // Check if an element name already exist as complex data type.
        if (_complex_names.end() != _complex_names.find(poEnum->getName()))
        {
            addSuggestion(a_util::strings::format("Duplicate identifier (Complex_Datatype) '%s'.",
                poEnum->getName().c_str()), importer_error);
            return ERR_INVALID_ARG;
        }

        // Check if an element name already exist.
        if (_enum_names.end() != _enum_names.find(poEnum->getName()))
        {
            addSuggestion(a_util::strings::format("Duplicate identifier (Enum Type) '%s'.",
                poEnum->getName().c_str()), importer_error);
            return ERR_INVALID_ARG;
        }
        else
        {
            _enum_names.insert(poEnum->getName());
        }

        // Check if no elements defined in enum
        if (vecNameValues.size() == 0)
        {
            addSuggestion(a_util::strings::format("No elements defined in enum '%s'.",
                poEnum->getName().c_str()), importer_warning);
        }

        if (_data_types_names.end() == _data_types_names.find(poEnum->getType()))
        {
            addSuggestion(a_util::strings::format("Type '%s' not found.",
                poEnum->getType().c_str()), importer_error);
            return ERR_INVALID_HANDLE;
        }

        std::string strTypeName = poEnum->getType();
        for (EnumNameValueVec::iterator itValue = vecNameValues.begin();
            itValue != vecNameValues.end();
            ++itValue)
        {
            bool bError = false;
            if (itValue->second.empty() || (!a_util::strings::isInt64(itValue->second) && !a_util::strings::isFloat(itValue->second)))
            {
                bError = true;
            }
            else if (strTypeName == "tBool")
            {
                if (itValue->second != "0" && itValue->second != "1")
                {
                    bError = true;
                }
            }
            else if (strTypeName == "tUInt8")
            {
                uint64_t ui64Val = a_util::strings::toUInt64(itValue->second);
                if ((!itValue->second.empty() && itValue->second[0] == '-') || ui64Val > std::numeric_limits<uint8_t>::max())
                {
                    bError = true;
                }
            }
            else if (strTypeName == "tInt8" || strTypeName == "tChar")
            {
                int64_t i64Val = a_util::strings::toInt64(itValue->second);
                if (i64Val > std::numeric_limits<int8_t>::max() || i64Val < std::numeric_limits<int8_t>::min())
                {
                    bError = true;
                }
            }
            else if (strTypeName == "tInt16")
            {
                int64_t i64Val = a_util::strings::toInt64(itValue->second);
                if (i64Val > std::numeric_limits<int16_t>::max() || i64Val < std::numeric_limits<int16_t>::min())
                {
                    bError = true;
                }
            }
            else if (strTypeName == "tUInt16")
            {
                uint64_t ui64Val = a_util::strings::toUInt64(itValue->second);
                if ((!itValue->second.empty() && itValue->second[0] == '-') || ui64Val > std::numeric_limits<uint16_t>::max())
                {
                    bError = true;
                }
            }
            else if (strTypeName == "tInt32")
            {
                int64_t i64Val = a_util::strings::toInt64(itValue->second);
                if (i64Val > std::numeric_limits<int32_t>::max() || i64Val < std::numeric_limits<int32_t>::min())
                {
                    bError = true;
                }
            }
            else if (strTypeName == "tUInt32")
            {
                uint64_t ui64Val = a_util::strings::toUInt64(itValue->second);
                if ((!itValue->second.empty() && itValue->second[0] == '-') || ui64Val > std::numeric_limits<uint32_t>::max())
                {
                    bError = true;
                }
            }
            else if (strTypeName == "tInt64")
            {
                int64_t i64Val = a_util::strings::toInt64(itValue->second);
                if (i64Val > std::numeric_limits<int64_t>::max() || i64Val < std::numeric_limits<int64_t>::min())
                {
                    bError = true;
                }
            }
            else if (strTypeName == "tUInt64")
            {
                uint64_t ui64Val = a_util::strings::toUInt64(itValue->second);
                if ((!itValue->second.empty() && itValue->second[0] == '-') || ui64Val > std::numeric_limits<uint64_t>::max())
                {
                    bError = true;
                }
            }
            else
            {
                bError = false;
            }
            if (bError)
            {
                addSuggestion(
                    a_util::strings::format("The element '%s' in the enumeration '%s' has a value of '%s' which is outside the range of the enumeration type '%s'.",
                    itValue->first.c_str(), poEnum->getName().c_str(),
                    itValue->second.c_str(), strTypeName.c_str()), importer_error);
                return ERR_INVALID_HANDLE;
            }
        }

        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLInspector::visit(DDLComplex* poComplex)
    {
        if (!poComplex) { return ERR_POINTER; }

        StructInfo& sStructInfo = _struct_infos[poComplex];
        sStructInfo.valid = false;
        sStructInfo.size = 0;

        // Check if an element name already exist as enum.
        if (_enum_names.end() != _enum_names.find(poComplex->getName()))
        {
            addSuggestion(a_util::strings::format("Duplicate identifier (Enum Type) '%s'.",
                poComplex->getName().c_str()), importer_error);
            return ERR_INVALID_ARG;
        }

        // Check if an element name already exist as data type.
        if (_data_types_names.end() != _data_types_names.find(poComplex->getName()))
        {
            addSuggestion(a_util::strings::format("Duplicate identifier (Data Type) '%s'.",
                poComplex->getName().c_str()), importer_error);
            return ERR_INVALID_ARG;
        }

        DDLElementVec vecElements = poComplex->getElements();

        // Check if no elements defined in complex datatype
        if (vecElements.size() == 0)
        {
            addSuggestion(a_util::strings::format("No elements defined in complex datatype '%s'.",
                poComplex->getName().c_str()), importer_warning);
        }

        // clone elements for own changes => save back on success
        std::transform(vecElements.begin(), vecElements.end(),
            vecElements.begin(), DDLDescription::clone<DDLElement>);

        //this is cloned now !!!! We need to delete it then !!!! 
        DDLAutoVec<DDLElement> m_oVecSync(&vecElements);

        for (DDLElementIt itElem = vecElements.begin();
            vecElements.end() != itElem; ++itElem)
        {
            const IDDLDataType * poDT = _ddl_desc->getDataTypeByName((*itElem)->getType());
            if (NULL == poDT)
            {
                // no primitive data type
                poDT = _ddl_desc->getStructByName((*itElem)->getType());
                if (NULL == poDT)
                {
                    poDT = _ddl_desc->getEnumByName((*itElem)->getType());
                    if (NULL == poDT)
                    {
                        addSuggestion(a_util::strings::format("Type '%s' not found.",
                            (*itElem)->getType().c_str()), importer_error);
                        return ERR_INVALID_HANDLE;
                    }

                }
            }
            else if (/*poElement->getTypeObject()->isPredefined() && */
                (*itElem)->isDefaultValid() &&
                ((lessDDL((*itElem)->getDefaultValue(), (*itElem)->getMinValue(),
                dynamic_cast<DDLDataType*>((*itElem)->getTypeObject()))) ||
                (lessDDL((*itElem)->getMaxValue(), (*itElem)->getDefaultValue(),
                dynamic_cast<DDLDataType*>((*itElem)->getTypeObject())))))
            {
                // primitive data type and default value out of range detected
                addSuggestion(a_util::strings::format("Default value of element '%s' is out of range.",
                    (*itElem)->getName().c_str()), importer_error);
                return ERR_INVALID_ARG;
            }
        }

        // validate of nested structs
        std::string strLog = poComplex->getName();
        if (false == checkValidyOfNestedStructs(_ddl_desc, poComplex->getName(), poComplex->getName(), strLog))
        {
            addSuggestion(a_util::strings::format("A complex data type is recursive defined: %s",
                strLog.c_str()), importer_error);
            return ERR_INVALID_ARG;
        }

        std::string name = a_util::strings::empty_string;
        unsigned int uiBytePos = 0;
        unsigned int uiElementSize = 0;
        size_t szLastElementSize = 0;
        unsigned int szLastElementBytePos = 0;
        bool bIsDymArraySize = false;
        std::set<std::string> setComplexElementName;

        for (DDLElementIt itElem = vecElements.begin();
            vecElements.end() != itElem; ++itElem)
        {
            name = (*itElem)->getName();

            uint32_t ui32ArraySize = (*itElem)->getArraysize();

            if (_has_struct_dynamic_arrays && !(*itElem)->isDynamic())
            {
                addSuggestion(a_util::strings::format("Please use Dynamic arrays always as last element '%s'.'%s'",
                    poComplex->getName().c_str(), name.c_str()), importer_error);
                // FIXME: RETURN_ERROR() needed
            }

            // Check if an element name already exist.
            if (setComplexElementName.end() != setComplexElementName.find(name))
            {
                addSuggestion(a_util::strings::format("Duplicate identifier (Complex_Datatypes Element Name) '%s' in %s.",
                    name.c_str(), poComplex->getName().c_str()),
                    importer_error);
                return ERR_INVALID_ARG;
            }
            else
            {
                setComplexElementName.insert(name);
            }

            if ((*itElem)->isDynamic() && _version <  DDLVersion::ddl_version_12)
            {
                //Element '%s.%s' has an illegal array size of 0
                addSuggestion(a_util::strings::format("Element (Complex_Datatypes) '%s.%s' is a dynamic array "
                    "and therefore not supported in v%s. ",
                    poComplex->getName().c_str(), name.c_str(), _version.toString().c_str()),
                    importer_error);
                return ERR_INVALID_ARG;
            }

            if (0 == ui32ArraySize && (*itElem)->getArraySizeSource() == "")
            {
                addSuggestion(a_util::strings::format("Element (Complex_Datatypes Element Name) '%s.%s' has an illegal array size of 0.",
                    poComplex->getName().c_str(), name.c_str()),
                    importer_warning);
                // FIXME: RETURN_ERROR(ERR_INVALID_ARG) ??
            }

            std::string strType = (*itElem)->getType();
            const DDLDataType *poDT = _ddl_desc->getDataTypeByName(strType);
            if (NULL == poDT)
            {
                const DDLEnum *poEnum = _ddl_desc->getEnumByName(strType);
                if (NULL == poEnum)
                {
                    // complex data type
                    DDLComplex* pSubStruct = _ddl_desc->getStructByName(strType);
                    if (!pSubStruct)
                    {
                        addSuggestion(a_util::strings::format("Element (Complex_Datatypes Element Name) '%s.%s' has an unkown type '%s'.",
                            poComplex->getName().c_str(), name.c_str(),
                            strType.c_str()), importer_error);
                        return ERR_NOT_FOUND;
                    }
                    if (isFailed(pSubStruct->accept(this)) ||
                        !_struct_infos[pSubStruct].valid)
                    {
                        addSuggestion(a_util::strings::format("Element (Complex_Datatypes Element Name) '%s.%s' has type '%s' that contains errors.",
                            poComplex->getName().c_str(), name.c_str(),
                            strType.c_str()), importer_error);
                        return ERR_INVALID_ARG;
                    }

                    uiElementSize = _struct_infos[pSubStruct].size;
                }
                else
                {
                    // enum type => treat like primitive data type
                    poDT = _ddl_desc->getDataTypeByName(poEnum->getType());
                    uiElementSize = poDT->getNumBits() / 8;
                }
            }
            else
            {
                // primitive data type
                uiElementSize = poDT->getNumBits() / 8;

                // Verify Min and Max values
                std::string strMax = (*itElem)->getMaxValue();
                if ((*itElem)->getType() == "tBool")
                {
                    if (strMax == "tTrue")
                    {
                        strMax = "1";
                    }
                }
                if (!a_util::strings::isDouble(strMax))
                {
                    if (_auto_correct)
                    {
                        double d = a_util::strings::toDouble(strMax);
                        // Try to remove multiple points
                        if (strMax.find(".") != std::string::npos)
                        {
                            strMax = a_util::strings::replace(strMax, ".", "");
                            d = a_util::strings::toDouble(strMax);
                        }
                        (*itElem)->setMaxValue(a_util::strings::format("%f", d));
                        addSuggestion(a_util::strings::format("Element (Complex_Datatypes Element Name)\
                        '%s.%s' maximum value was set to the floating point value %f.",
                            poComplex->getName().c_str(), name.c_str(),
                            d), importer_info);
                    }
                    else
                    {
                        addSuggestion(a_util::strings::format("Element (Complex_Datatypes Element Name)\
                        '%s.%s' maximum value is \'%s\' but it should be a floating point value or tTrue "
                            "if using type tBool.",
                            poComplex->getName().c_str(), name.c_str(),
                            strMax.c_str()), importer_error);
                    }
                }
                std::string strMin = (*itElem)->getMinValue();
                if ((*itElem)->getType() == "tBool")
                {
                    if (strMin == "tFalse")
                    {
                        strMin = "0";
                    }
                }
                if (!a_util::strings::isDouble(strMin))
                {
                    if (_auto_correct)
                    {
                        double d = a_util::strings::toDouble(strMin);
                        // Try to remove multiple points
                        if (strMin.find(".") != std::string::npos)
                        {
                            strMin = a_util::strings::replace(strMin, ".", "");
                            d = a_util::strings::toDouble(strMin);
                        }
                        (*itElem)->setMinValue(a_util::strings::format("%f", d));
                        addSuggestion(a_util::strings::format("Element (Complex_Datatypes Element Name)\
                        '%s.%s' minimum value was set to the floating point value %f.",
                            poComplex->getName().c_str(), name.c_str(),
                            d), importer_info);
                    }
                    else
                    {
                        addSuggestion(a_util::strings::format("Element (Complex_Datatypes Element Name)\
                        '%s.%s' minimum value is \'%s\' but it should be a floating point value or tFalse "
                            "if using type tBool.",
                            poComplex->getName().c_str(), name.c_str(),
                            strMin.c_str()), importer_error);
                    }
                }
            }

            if (uiElementSize > szLastElementSize && szLastElementSize != 0)
            {
                if (_warning_level == WarningLevel::verbose)
                {
                    addSuggestion(a_util::strings::format("Please use the biggest elements/types first "
                        "and then the smaller ones (Complex_Datatypes) "
                        "'%s.%s'.", poComplex->getName().c_str(),
                        name.c_str()), importer_warning);
                }
            }

            // check continuity of elements
            if ((*itElem)->getBytepos() != uiBytePos)
            {
                // element byte-position is different from calculated
                // byte-position
                if (_auto_correct)
                {
                    if ((*itElem)->getBytepos() < uiBytePos)
                    {
                        // one or more preceding elements have been altered by
                        // the DDLInspector so that the elements would overlap
                        // now => adapt byte-position
                        (*itElem)->setBytepos(uiBytePos);

                        addSuggestion(a_util::strings::format("Byte-position of (Complex_Datatypes) "
                            "'%s.%s' has been modified.", poComplex->getName().c_str(),
                            name.c_str()), importer_info);
                    }
                    else
                    {
                        (*itElem)->setBytepos(uiBytePos);

                        addSuggestion(a_util::strings::format("Correct the Byte position (Complex_Datatypes) "
                            "'%s.%s'.", poComplex->getName().c_str(),
                            name.c_str()), importer_info);
                    }
                }
                else
                {
                    addSuggestion(a_util::strings::format("Gap / Padding element '%s.%s' detected.",
                        poComplex->getName().c_str(), name.c_str()),
                        importer_warning);
                }
            }

            uiElementSize *= ui32ArraySize;

            sStructInfo.last_byte_pos_real = (*itElem)->getBytepos() + uiElementSize;

            uiBytePos += uiElementSize;
            szLastElementSize = uiElementSize;
            szLastElementBytePos = uiBytePos;

            if ((*itElem)->isDynamic() && !bIsDymArraySize)
            {
                bIsDymArraySize = (*itElem)->isDynamic();

                if (!_has_struct_dynamic_arrays)
                {
                    _has_struct_dynamic_arrays = true;
                }
            }
        } //end of for iterator!!

        if (_auto_correct)
        {
            m_oVecSync.reset();
            // save back changed element vector
            poComplex->setElements(vecElements);
        }

        // save struct size
        sStructInfo.size = uiBytePos;
        sStructInfo.last_byte_pos = szLastElementBytePos;
        sStructInfo.valid = true;
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLInspector::visit(DDLElement* poElement)
    {
        // should not be called
        return ERR_NOT_IMPL;
    }

    a_util::result::Result DDLInspector::visit(DDLStream* const poStream)
    {
        if (!poStream) { return ERR_POINTER; }

        DDLStreamStructVec vecStreamStruct = poStream->getStructs();

        // Check if no strucs defined in stream
        if (vecStreamStruct.size() == 0)
        {
            addSuggestion(a_util::strings::format("No complex datatypes defined in stream '%s'.",
                poStream->getName().c_str()), importer_warning);
        }

        std::set<std::string> setStreamsElementNames;
        for (size_t posI = 0; posI < vecStreamStruct.size(); posI++)
        {
            if ((vecStreamStruct[posI]->getName() != "") && _version <  DDLVersion::ddl_version_12)
            {
                addSuggestion(a_util::strings::format("Name in Streams is not supported '%s' "
                    "in this Version v%s. ", vecStreamStruct[posI]->getName().c_str(),
                    _version.toString().c_str()), importer_error);
                return ERR_INVALID_ARG;
            }

            if ((vecStreamStruct[posI]->getType() != "") && _version <  DDLVersion::ddl_version_10)
            {
                addSuggestion(a_util::strings::format("Types in Streams is not supported '%s' "
                    "in this Version v%s. ", vecStreamStruct[posI]->getName().c_str(),
                    _version.toString().c_str()), importer_error);
                return ERR_INVALID_ARG;
            }

            if (setStreamsElementNames.end() != setStreamsElementNames.find(vecStreamStruct[posI]->getName()))
            {
                addSuggestion(a_util::strings::format("Duplicate identifier (Stream Element) '%s'.",
                    vecStreamStruct[posI]->getName().c_str()), importer_error);
            }
            else
            {
                setStreamsElementNames.insert(vecStreamStruct[posI]->getName());
            }
        }

        // Check if an stream name already exist.
        if (_stream_names.end() != _stream_names.find(poStream->getName()))
        {
            addSuggestion(a_util::strings::format("Duplicate identifier (Stream Type) '%s'.",
                poStream->getName().c_str()), importer_error);
        }
        else
        {
            _stream_names.insert(poStream->getName());
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLInspector::visit(DDLStreamStruct* poStreamStruct)
    {
        // should not be called
        return ERR_NOT_IMPL;
    }

    a_util::result::Result DDLInspector::visit(DDLStreamMetaType* poStreamMetaType)
    {
        return ERR_NOT_IMPL;
    }

    a_util::result::Result DDLInspector::visit(DDLProperty* poProperty)
    {
        return ERR_NOT_IMPL;
    }

    bool DDLInspector::isAlignable(size_t const posBytePos,
        size_t const szElementSize)
    {
        if (0 == posBytePos % s_szAlignSize)
        {
            // previous elements (if there are any) are aligned => accept everything
            return true;
        }
        if (posBytePos % s_szAlignSize + szElementSize <= s_szAlignSize &&
            0 == posBytePos % szElementSize)
        {
            // element is alignable
            return true;
        }
        // element is not alignable
        return false;
    }

    ImporterMsgList DDLInspector::getSuggestions() const
    {
        return _suggestions;
    }

    std::string DDLInspector::getLastErrorDesc() const
    {
        // return description of most recently added error element
        for (ImporterMsgList::const_reverse_iterator itMsgs = _suggestions.rbegin();
            _suggestions.rend() != itMsgs; ++itMsgs)
        {
            if (importer_error == itMsgs->severity)
            {
                return std::string(itMsgs->desc);
            }
        }
        return a_util::strings::empty_string;
    }

    void DDLInspector::InitNewCheck()
    {
        _complex_names.clear();
        _data_types_names.clear();

        _enum_names.clear();

        _base_unit_names.clear();

        _stream_names.clear();

        _prefix_names.clear();

        _version = DDLVersion::ddl_version_invalid;

        _has_struct_dynamic_arrays = false;

        _suggestions.clear();
    }

    bool DDLInspector::foundDynamicArrays()
    {
        return _has_dynamic_arrays;
    }

    unsigned int DDLInspector::getLastBytePosOfAStructCorrected(const std::string& strStructName)
    {
        DDLComplex *poComplex = _ddl_desc->getStructByName(strStructName);
        visit(poComplex);
        return _struct_infos[poComplex].last_byte_pos;
    }

    unsigned int DDLInspector::getLastBytePosOfAStructReal(const std::string& strStructName)
    {
        DDLComplex *poComplex = _ddl_desc->getStructByName(strStructName);
        visit(poComplex);
        return _struct_infos[poComplex].last_byte_pos_real;
    }

    void DDLInspector::setAutoCorrect(bool autoCorrection)
    {
        _auto_correct = autoCorrection;
    }

    void DDLInspector::setWarningLevel(WarningLevel warning_level)
    {
        _warning_level = warning_level;
    }

    ddl::WarningLevel DDLInspector::getWarningLevel() const
    {
        return _warning_level;
    }

    uint64_t boolToInt(const std::string& bVar)
    {
        if ((bVar) == "true" || (bVar) == "on" || (bVar) == "1" || (bVar) == "tTrue")
        {
            return 1;
        }
        else if ((bVar) == "false" || (bVar) == "off" || (bVar) == "0" || (bVar) == "tFalse")
        {
           return 0;
        }
        else if(a_util::strings::toUInt64(bVar) == 0)
        {
            return 0;
        }
        return 1;
    }

    bool DDLInspector::lessDDL(const std::string& strLHS,
        const std::string& strRHS, const DDLDataType* poType)
    {
        if (NULL == poType || !poType->isPredefined())
        {
            // not interpretable
            return false;
        }

        if (poType->getName() == "tBool")
        {
            // compare boolean type
            return boolToInt(strLHS) < boolToInt(strRHS);
        }
        if (poType->getName() == "tChar" ||
            poType->getName() == "tUInt8" ||
            poType->getName() == "tUInt16" ||
            poType->getName() == "tUInt32" ||
            poType->getName() == "tUInt64")
        {
            // compare unsigned integer types
            return a_util::strings::toUInt64(strLHS) < a_util::strings::toUInt64(strRHS);
        }
        if (poType->getName() == "tInt8" ||
            poType->getName() == "tInt16" ||
            poType->getName() == "tInt32" ||
            poType->getName() == "tInt64")
        {
            // compare signed integer types
            return a_util::strings::toInt64(strLHS) < a_util::strings::toInt64(strRHS);
        }
        if (poType->getName() == "tFloat32" ||
            poType->getName() == "tFloat64")
        {
            // compare floating-point types
            return a_util::strings::toDouble(strLHS) < a_util::strings::toDouble(strRHS);
        }

        // other type => not interpretable
        return false;
    }

    ImporterMsgList DDLInspector::checkValidyOfNestedStructs(const DDLDescription* poDescription)
    {
        ImporterMsgList lstErrors;
        if (!poDescription)
        {
            ImporterMsg sMsg =
            {
                a_util::strings::format("Description is invalid."),
                importer_error
            };
            lstErrors.push_back(sMsg);
            return lstErrors;
        }
        // validate of nested structs
        DDLComplexVec vecStructs = poDescription->getStructs();
        for (DDLComplexIt itStruct = vecStructs.begin(); itStruct != vecStructs.end(); ++itStruct)
        {
            std::string strStructTypeName = (*itStruct)->getName();
            std::string strLog = strStructTypeName;
            if (false == checkValidyOfNestedStructs(poDescription, strStructTypeName, strStructTypeName, strLog))
            {
                ImporterMsg sMsg =
                {
                    a_util::strings::format(strLog.c_str()),
                    importer_error
                };
                lstErrors.push_back(sMsg);
            }
        }
        return lstErrors;
    }

    bool DDLInspector::checkValidyOfNestedStructs(const DDLDescription* poDescription,
        const std::string& strStructTypeName,
        const std::string& strNestedStructTypeName,
        std::string& strLog,
        int nStructDepth /*=1*/)
    {
        if (!poDescription)
        {
            strLog = a_util::strings::format("Description is invalid.");
            return false;
        }
        if (nStructDepth > DDL_MAX_DESC_HIERARCHY)
        {
            strLog = a_util::strings::format("The depth of the hierarchy of the complex data type '%s' is higher than %u.", strStructTypeName.c_str(), DDL_MAX_DESC_HIERARCHY);
            return false;
        }
        const DDLComplex * poComplex = poDescription->getStructByName(strNestedStructTypeName);
        if (NULL != poComplex)
        {
            DDLElementVec vecElements = poComplex->getElements();

            for (DDLElementIt itElem = vecElements.begin();
                vecElements.end() != itElem; ++itElem)
            {
                std::string strStructElementName = (*itElem)->getType();
                const DDLComplex * poDT = poDescription->getStructByName(strStructElementName);
                if (NULL != poDT)
                {
                    // complex data type
                    std::vector<std::string> vecStructs = a_util::strings::split(strLog, ",", true);
                    strLog.append(a_util::strings::format(",%s", strStructElementName.c_str()));
                    if (std::find(vecStructs.begin(), vecStructs.end(), strStructElementName) != vecStructs.end())
                    {
                        // includes nested struct element with same data type as nested struct above
                        return false;
                    }
                    else
                    {
                        if (false == checkValidyOfNestedStructs(poDescription, strStructTypeName, strStructElementName, strLog, nStructDepth + 1))
                        {
                            // includes nested struct with same data type as base complex data type or hierarchy of the complex data type is higher than 5
                            return false;
                        }
                    }
                }
            }
        }
        strLog = a_util::strings::empty_string;
        return true;
    }

    a_util::result::Result DDLInspector::addSuggestion(std::string const &desc,
        ImporterMsgSeverity const severity)
    {
        if (_warning_level != WarningLevel::silent)
        {
            if (desc.empty())
            {
                return ERR_EMPTY;
            }
            if (importer_info > severity ||
                importer_error < severity)
            {
                return ERR_INVALID_ARG;
            }
            ImporterMsg sMsg = { desc, severity };
            _suggestions.push_back(sMsg);
        }
        return a_util::result::SUCCESS;
    }

} // namespace ddl
