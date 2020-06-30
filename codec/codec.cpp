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

#include "codec.h"

#include "a_util/result/error_def.h"
#include "a_util/logging.h"
#include "legacy_error_macros.h"
#include "struct_layout.h"
#include "static_codec.h"
#include "element_accessor.h"
#include "access_element.h"

namespace ddl
{

//define all needed error types and values locally
_MAKE_RESULT(-5, ERR_INVALID_ARG);
_MAKE_RESULT(-10, ERR_INVALID_INDEX);

static inline void BitToBytes(size_t& size)
{
    if (size % 8)
    {
        size = size/ 8 + 1;
    }
    else
    {
        size /= 8;
    }
}

Decoder::Decoder(const Decoder& oDecoder, const void* pData, size_t nDataSize,
                   DataRepresentation eRep):
    StaticDecoder(oDecoder._layout, pData, nDataSize, eRep),
    _dynamic_elements(oDecoder._dynamic_elements),
    _buffer_sizes(oDecoder._buffer_sizes)
{
}

Decoder::Decoder(a_util::memory::shared_ptr<const StructLayout> pLayout, const void* pData, size_t nDataSize,
                   DataRepresentation eRep):
    StaticDecoder(pLayout, pData, nDataSize, eRep),
    _buffer_sizes(pLayout->getStaticBufferBitSizes())
{
    if (_layout->hasDynamicElements())
    {
        a_util::result::Result oResult = calculateDynamicElements();
        if (isFailed(oResult))
        {
            LOG_ERROR("Failed to calculate dynamic elements");
        }
    }

    BitToBytes(_buffer_sizes.deserialized);
    BitToBytes(_buffer_sizes.serialized);
}

a_util::result::Result Decoder::isValid() const
{
    RETURN_IF_FAILED(_layout->isValid());
    if (_data_size < getBufferSize(getRepresentation()))
    {
        return ERR_INVALID_ARG;
    }

    return a_util::result::SUCCESS;
}

a_util::result::Result Decoder::calculateDynamicElements()
{
    _dynamic_elements.reset(new std::vector<StructLayoutElement>());

    for (std::vector<DynamicStructLayoutElement>::const_iterator
        itDynamicElement = _layout->getDynamicElements().begin();
         itDynamicElement != _layout->getDynamicElements().end(); ++ itDynamicElement)
    {
        RETURN_IF_FAILED(addDynamicElements(*itDynamicElement, _buffer_sizes, ""));
    }

    return a_util::result::SUCCESS;
}

void Decoder::moveToAlignment(size_t& bit_offset, size_t alignment)
{
    size_t nBitRest = bit_offset % 8;
    if (nBitRest)
    {
        bit_offset += 8 - nBitRest;
    }

    size_t nByteOffset = bit_offset / 8;
    size_t nRest = nByteOffset % alignment;
    if (nRest)
    {
        bit_offset += (alignment - nRest) * 8;
    }
}

a_util::result::Result Decoder::addDynamicElements(const DynamicStructLayoutElement& sDynamicElement,
                                     Offsets& sOverallOffsets,
                                     const std::string& strPrefix)
{
    moveToAlignment(sOverallOffsets.deserialized, sDynamicElement.alignment);

    if (sDynamicElement.isAlignmentElement())
    {
        return a_util::result::SUCCESS;
    }

    a_util::variant::Variant oArraySize((uint64_t)1);
    bool bIsArray = sDynamicElement.isDynamicArray();
    if (bIsArray)
    {
        oArraySize = access_element::get_value(*this,
                                               strPrefix + sDynamicElement.size_element_name);
    }
    size_t nArraySize = static_cast<size_t>(oArraySize.asUInt64());

    for (size_t nArrayIndex = 0; nArrayIndex < nArraySize; ++nArrayIndex)
    {
        RETURN_IF_FAILED(addDynamicElement(sDynamicElement,
                                           bIsArray ? a_util::strings::format("[%d]", nArrayIndex) : "",
                                           sOverallOffsets, strPrefix));
    }

    moveToAlignment(sOverallOffsets.deserialized, sDynamicElement.alignment);

    return a_util::result::SUCCESS;
}

a_util::result::Result Decoder::addDynamicElement(const DynamicStructLayoutElement& sDynamicElement,
                                    const std::string& strArrayIndex,
                                    Offsets& sOverallOffsets,
                                    const std::string& strPrefix)
{
    RETURN_IF_FAILED(addStaticStructElements(sDynamicElement, strArrayIndex, sOverallOffsets, strPrefix));
    RETURN_IF_FAILED(addDynamicStructElements(sDynamicElement, strArrayIndex, sOverallOffsets, strPrefix));

    return a_util::result::SUCCESS;
}

a_util::result::Result Decoder::addStaticStructElements(const DynamicStructLayoutElement& sDynamicElement,
                                          const std::string& strArrayIndex,
                                          Offsets& nOverallBitOffset,
                                          const std::string& strPrefix)
{
    Offsets sStartOffsets = nOverallBitOffset;
    for (std::vector<StructLayoutElement>::const_iterator itStaticElement = sDynamicElement.static_elements.begin();
         itStaticElement != sDynamicElement.static_elements.end(); ++itStaticElement)
    {
        StructLayoutElement sElement = *itStaticElement;
        sElement.deserialized.bit_offset += sStartOffsets.deserialized;
        sElement.serialized.bit_offset += sStartOffsets.serialized;
        nOverallBitOffset.deserialized = sElement.deserialized.bit_offset + sElement.deserialized.bit_size;
        nOverallBitOffset.serialized = sElement.serialized.bit_offset + sElement.serialized.bit_size;
        if (!sElement.name.empty())
        {
            sElement.name = a_util::strings::format("%s%s%s.%s", strPrefix.c_str(),
                                                           sDynamicElement.name.c_str(),
                                                           strArrayIndex.c_str(),
                                                           itStaticElement->name.c_str());
        }
        else
        {
            // not part of a struct, just a plain array element
            sElement.name = a_util::strings::format("%s%s%s", strPrefix.c_str(),
                                                           sDynamicElement.name.c_str(),
                                                           strArrayIndex.c_str());
        }
        RETURN_IF_FAILED(addDynamicStructElement(sElement));
    }

    return a_util::result::SUCCESS;
}

a_util::result::Result Decoder::addDynamicStructElements(const DynamicStructLayoutElement& sDynamicElement,
                                           const std::string& strArrayIndex,
                                           Offsets& sOverallOffsets,
                                           const std::string& strPrefix)
{
    std::string strChildPrefix = a_util::strings::format("%s%s%s.", strPrefix.c_str(),
                                                                     sDynamicElement.name.c_str(),
                                                                     strArrayIndex.c_str());
    for (std::vector<DynamicStructLayoutElement>::const_iterator
        itDynamicChildElement = sDynamicElement.dynamic_elements.begin();
         itDynamicChildElement != sDynamicElement.dynamic_elements.end();
         ++itDynamicChildElement)
    {
        RETURN_IF_FAILED(addDynamicElements(*itDynamicChildElement, sOverallOffsets, strChildPrefix));
    }

    return a_util::result::SUCCESS;
}

a_util::result::Result Decoder::addDynamicStructElement(const StructLayoutElement& sElement)
{
    const Position& sPos = _element_accessor->getRepresentation() == deserialized ?
                                sElement.deserialized : sElement.serialized;
    if (sPos.bit_offset + sPos.bit_size > _data_size * 8)
    {
        return ERR_INVALID_ARG;
    }

    _dynamic_elements->push_back(sElement);
    return a_util::result::SUCCESS;
}

size_t Decoder::getElementCount() const
{
    if (_dynamic_elements)
    {
        return _layout->getStaticElements().size() + _dynamic_elements->size();
    }

    return _layout->getStaticElements().size();
}

size_t Decoder::getBufferSize(DataRepresentation eRep) const
{
    return eRep == deserialized ?
                _buffer_sizes.deserialized :
                _buffer_sizes.serialized;
}

Codec Decoder::makeCodecFor(void* pData, size_t nDataSize, DataRepresentation eRep) const
{
    return Codec(*this, pData, nDataSize, eRep);
}

const StructLayoutElement* Decoder::getLayoutElement(size_t nIndex) const
{
    const StructLayoutElement* pElement = NULL;
    size_t nStaticElementCount = _layout->getStaticElements().size();
    if (nIndex < nStaticElementCount)
    {
        pElement = &_layout->getStaticElements()[nIndex];
    }
    else if (_dynamic_elements &&
             nIndex - nStaticElementCount < _dynamic_elements->size())
    {
        pElement = &(*_dynamic_elements)[nIndex - nStaticElementCount];
    }

    return pElement;
}

Codec::Codec(a_util::memory::shared_ptr<const StructLayout> pLayout, void* pData, size_t nDataSize,
               DataRepresentation eRep):
    Decoder(pLayout, pData, nDataSize, eRep)
{
}

Codec::Codec(const Decoder& oDecoder, void* pData, size_t nDataSize, DataRepresentation eRep):
    Decoder(oDecoder, pData, nDataSize, eRep)
{
}

a_util::result::Result Codec::setElementValue(size_t nIndex, const void* pValue)
{
    const StructLayoutElement* pElement = getLayoutElement(nIndex);
    if (!pElement)
    {
        return ERR_INVALID_INDEX;
    }
    return _element_accessor->setValue(*pElement,
                                        const_cast<void*>(_data),
                                        _data_size,
                                        pValue);
}

a_util::result::Result Codec::setElementValue(size_t nIndex, const a_util::variant::Variant& oValue)
{
    const StructLayoutElement* pElement = getLayoutElement(nIndex);
    if (!pElement)
    {
        return ERR_INVALID_INDEX;
    }
    return _element_accessor->setValue(*pElement, const_cast<void*>(_data),
                                        _data_size, oValue);
}

void* Codec::getElementAddress(size_t nIndex)
{
    return const_cast<void*>(StaticDecoder::getElementAddress(nIndex));
}

a_util::result::Result Codec::setConstants()
{
    if (_layout->hasEnums())
    {
        size_t nElementCount = getElementCount();
        for (size_t nElement = 0; nElement < nElementCount; ++nElement)
        {
            const StructLayoutElement* pElement = getLayoutElement(nElement);
            if (pElement->constant)
            {
                RETURN_IF_FAILED(_element_accessor->setValue(*pElement, const_cast<void*>(_data),
                                                              _data_size, *pElement->constant));
            }
        }
    }

    return a_util::result::SUCCESS;
}

}
