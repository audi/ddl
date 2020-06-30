/**
 * @file
 * Implementation of the ADTF default media description.
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

#include "a_util/result/error_def.h"
#include "legacy_error_macros.h"

#include "struct_layout.h"

#include <ddl.h>

namespace ddl
{
//define all needed error types and values locally
_MAKE_RESULT(-5, ERR_INVALID_ARG);
_MAKE_RESULT(-19, ERR_NOT_SUPPORTED);
_MAKE_RESULT(-37, ERR_NOT_INITIALIZED);

StructLayout::StructLayout(const DDLComplex* pStruct)
{
    _static_buffer_sizes.deserialized = 0;
    _static_buffer_sizes.serialized = 0;
    _calculations_result = calculate(pStruct);
}

StructLayout::StructLayout():
    _calculations_result(ERR_NOT_INITIALIZED)
{
    _static_buffer_sizes.deserialized = 0;
    _static_buffer_sizes.serialized = 0;
}

#define IF_TYPE(__type, __vtype, __size) \
    if (strType == __type) \
{ \
    type = a_util::variant::__vtype;\
    nTypeSize = __size; \
    return a_util::result::SUCCESS; \
}

static a_util::result::Result getType(const std::string& strType, a_util::variant::VariantType& type, size_t& nTypeSize)
{
    IF_TYPE("tBool", VT_Bool, 8)
    else IF_TYPE("tChar", VT_Int8, 8)
    else IF_TYPE("tInt8", VT_Int8, 8)
    else IF_TYPE("tUInt8", VT_UInt8, 8)
    else IF_TYPE("tInt16", VT_Int16, 16)
    else IF_TYPE("tUInt16", VT_UInt16, 16)
    else IF_TYPE("tInt32", VT_Int32, 32)
    else IF_TYPE("tUInt32", VT_UInt32, 32)
    else IF_TYPE("tInt64", VT_Int64, 64)
    else IF_TYPE("tUInt64", VT_UInt64, 64)
    else IF_TYPE("tFloat32", VT_Float32, 32)
    else IF_TYPE("tFloat64", VT_Float64, 64)

    return ERR_NOT_SUPPORTED;
}

class cConverter
{
    public:
        cConverter(std::vector<StructLayoutElement>& static_elements,
                   std::vector<DynamicStructLayoutElement>& dynamic_elements,
                   std::map<std::string, EnumType>& oEnums):
            m_bDynamicSectionStarted(false),
            _static_elements(static_elements),
            _dynamic_elements(dynamic_elements),
            _enums(oEnums)
        {
            m_sOffsets.deserialized = 0;
            m_sOffsets.serialized = 0;
        }

        a_util::result::Result Convert(DDLComplex* pStruct)
        {
            return addStruct(pStruct, "", m_sOffsets.serialized, pStruct->getDDLVersion() >= DDLVersion::ddl_version_30);
        }

        Offsets getStaticBufferBitSizes()
        {
            return m_sOffsets;
        }

    private:

        // we are not using a standard visitor pattern because we would have to recreate a stack
        // for passing along all the additional parameters
        a_util::result::Result Add(IDDLDataType* pType, const std::string& strFullName, size_t& nSerializedOffset,
                    int byte_order, int nNumBits, const std::string& strConstant, bool is_last_array_element)
        {
            DDLDataType* pPODType = dynamic_cast<DDLDataType*>(pType);
            DDLEnum* p_enum = dynamic_cast<DDLEnum*>(pType);
            EnumType* pCodecEnum = NULL;
            if (p_enum)
            {
                pPODType = dynamic_cast<DDLDataType*>(p_enum->getTypeObject());
                pCodecEnum = GetCodecEnum(p_enum);
            }

            if (pPODType)
            {
                return AddPODElement(pPODType, strFullName, nSerializedOffset, byte_order,
                                     nNumBits, pCodecEnum, strConstant);
            }
            else
            {
                DDLComplex* pStruct = dynamic_cast<DDLComplex*>(pType);
                if (pStruct)
                {
                    return addStruct(pStruct, strFullName, nSerializedOffset,
                        // prior to DDL 3.0 the last array element is not padded to the alignment
                        !is_last_array_element || pStruct->getDDLVersion() >= DDLVersion::ddl_version_30);
                }

                return ERR_INVALID_ARG;
            }
        }

        EnumType* GetCodecEnum(DDLEnum* p_enum)
        {
            std::map<std::string, EnumType>::iterator itEnum = _enums.find(p_enum->getName());
            if (itEnum == _enums.end())
            {
                EnumType oCodecEnum;
                const EnumNameValueVec& vecValues = p_enum->getValues();
                for (EnumNameValueVec::const_iterator it = vecValues.begin(); it != vecValues.end(); ++it)
                {
                    oCodecEnum.insert(std::make_pair(it->first, a_util::variant::Variant(it->second.c_str())));
                }
                itEnum = _enums.insert(std::make_pair(p_enum->getName(), oCodecEnum)).first;
            }
            return &itEnum->second;
        }

        a_util::result::Result AddPODElement(DDLDataType* pDataType, const std::string& strFullName,
                              size_t& nSerializedOffset, int byte_order, int nNumBits,
                              EnumType* p_enum,
                              const std::string& strConstant)
        {
            StructLayoutElement sElement;
            sElement.name = strFullName;
            RETURN_IF_FAILED(getType(pDataType->getName(), sElement.type, sElement.deserialized.bit_size));
            sElement.deserialized.bit_offset = m_sOffsets.deserialized;
            sElement.serialized.bit_offset = nSerializedOffset;
            sElement.serialized.bit_size = nNumBits ? nNumBits : pDataType->getNumBits();
            sElement.byte_order = byte_order;
            sElement.p_enum = p_enum;
            sElement.constant = FindConstant(strConstant);
            _static_elements.push_back(sElement);

            m_sOffsets.deserialized += sElement.deserialized.bit_size;
            nSerializedOffset += sElement.serialized.bit_size;
            return a_util::result::SUCCESS;
        }

        const a_util::variant::Variant* FindConstant(const std::string& strConstant)
        {
            if (!strConstant.empty())
            {
                for (std::map<std::string, EnumType>::const_iterator it = _enums.begin(); it != _enums.end(); ++it)
                {
                    EnumType::const_iterator itValue = it->second.find(strConstant);
                    if (itValue != it->second.end())
                    {
                        return &itValue->second;
                    }
                }
            }

            return NULL;
        }

        a_util::result::Result addStruct(DDLComplex* pStruct, const std::string& strFullName, size_t& nSerializedOffset, bool resize_to_alignment)
        {
            std::string strStructPrefix = (!strFullName.empty()) ? strFullName + "." : "";

            bool bWasDynamicSectionStartedBefore = m_bDynamicSectionStarted;
            size_t nStructBaseOffset = m_sOffsets.deserialized;

            size_t nMaxSerializedOffset = nSerializedOffset;
            DDLElementVec& vecElements = pStruct->getElements();
            for (DDLElementVec::iterator it = vecElements.begin(); it != vecElements.end(); ++it)
            {
                size_t nElementOffset = nSerializedOffset;
                RETURN_IF_FAILED(addElement(*it, strStructPrefix,
                                            nElementOffset,
                                            nStructBaseOffset));
                nMaxSerializedOffset = std::max(nMaxSerializedOffset, nElementOffset);
            }
            nSerializedOffset = nMaxSerializedOffset;

            if (resize_to_alignment)
            {
                MoveOffsetToAlignment(pStruct->getAlignment(), nStructBaseOffset);
            }

            if (m_bDynamicSectionStarted && !bWasDynamicSectionStartedBefore)
            {
                // add an alignment marker to ensure that the next element starts at the
                // correct position
                _dynamic_elements.push_back(pStruct->getAlignment());
            }

            return a_util::result::SUCCESS;
        }

        void MoveOffsetToAlignment(DDLAlignment::AlignmentType eAlignment, size_t alignment_base_offset)
        {
            size_t nRelativeOffset = m_sOffsets.deserialized - alignment_base_offset;
            size_t nBitRest = nRelativeOffset % 8;
            if (nBitRest)
            {
                nRelativeOffset += 8 - nBitRest;
            }

            size_t nByteOffset = nRelativeOffset / 8;
            size_t nRest = nByteOffset % eAlignment;
            if (nRest)
            {
                nRelativeOffset += (eAlignment - nRest) * 8;
            }
            m_sOffsets.deserialized = alignment_base_offset + nRelativeOffset;
        }

        a_util::result::Result addElement(DDLElement* pElement, const std::string& strStructPrefix, size_t& nSerializedOffset, size_t nStructBaseOffset)
        {
            if (!m_bDynamicSectionStarted)
            {
                m_bDynamicSectionStarted = pElement->getArraysize() == 0;
            }

            if (!m_bDynamicSectionStarted)
            {
                MoveOffsetToAlignment(pElement->getAlignment(), nStructBaseOffset);
                nSerializedOffset += pElement->getBytepos() * 8 + pElement->getBitpos();
            }

            size_t nArraySize = std::max(pElement->getArraysize(), 1u);
            for (size_t nArrayIndex = 0; nArrayIndex < nArraySize; ++nArrayIndex)
            {
                std::string strCurrentElementName = strStructPrefix + pElement->getName();
                if (nArraySize > 1)
                {
                    strCurrentElementName += a_util::strings::format("[%d]", nArrayIndex);
                }

                if (!m_bDynamicSectionStarted)
                {
                    RETURN_IF_FAILED(Add(pElement->getTypeObject(), strCurrentElementName,
                                         nSerializedOffset, pElement->getByteorder(),
                                         pElement->getNumBits(),
                                         pElement->getConstantValue(),
                                         nArrayIndex == nArraySize - 1));
                }
                else
                {
                    RETURN_IF_FAILED(addDynamicElement(pElement, strCurrentElementName, strStructPrefix));
                }
            }

            return a_util::result::SUCCESS;
        }

        a_util::result::Result addDynamicElement(const DDLElement* pElement, const std::string& strFullName,
                                  const std::string& strStructPrefix)
        {
            DynamicStructLayoutElement sDynamicElement;

            cConverter oChildConverter(sDynamicElement.static_elements,
                                       sDynamicElement.dynamic_elements,
                                       _enums);
            size_t nDummy = 0;
            RETURN_IF_FAILED(oChildConverter.Add(pElement->getTypeObject(), "",
                                                 nDummy, pElement->getByteorder(),
                                                 pElement->getNumBits(),
                                                 pElement->getConstantValue(),
                                                 false));

            sDynamicElement.name = strFullName;
            sDynamicElement.alignment = pElement->getAlignment();

            if (pElement->getArraysize() == 0)
            {
                if (pElement->getArraySizeSource().empty())
                {
                    return ERR_INVALID_ARG;
                }

                sDynamicElement.size_element_name = strStructPrefix + pElement->getArraySizeSource();
            }

            _dynamic_elements.push_back(sDynamicElement);

            return a_util::result::SUCCESS;
        }

    protected:
        Offsets m_sOffsets;
        bool m_bDynamicSectionStarted;
        std::vector<StructLayoutElement>& _static_elements;
        std::vector<DynamicStructLayoutElement>& _dynamic_elements;
        std::map<std::string, EnumType>& _enums;
};

a_util::result::Result StructLayout::calculate(const DDLComplex* pStruct)
{
    cConverter oConverter(_static_elements,
                          _dynamic_elements,
                          _enums);
    RETURN_IF_FAILED(oConverter.Convert(const_cast<DDLComplex*>(pStruct)));
    _static_buffer_sizes = oConverter.getStaticBufferBitSizes();

    return a_util::result::SUCCESS;
}

size_t StructLayout::getStaticBufferSize(DataRepresentation eRep) const
{
    size_t nResult = eRep == deserialized ?
                        _static_buffer_sizes.deserialized :
                        _static_buffer_sizes.serialized;
    if (nResult % 8)
    {
        return nResult / 8 + 1;
    }
    return nResult / 8;
}

}

