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

#include "static_codec.h"

#include "a_util/result/error_def.h"
#include "legacy_error_macros.h"
#include "element_accessor.h"

namespace ddl
{

//define all needed error types and values locally
_MAKE_RESULT(-5, ERR_INVALID_ARG)
_MAKE_RESULT(-10, ERR_INVALID_INDEX)

StaticDecoder::StaticDecoder(a_util::memory::shared_ptr<const StructLayout> pLayout,
                               const void* pData, size_t nDataSize,
                               DataRepresentation eRep):
    _layout(pLayout),
    _data(pData),
    _data_size(nDataSize),
    _element_accessor(eRep == deserialized ?
                       &DeserializedAccessor::getInstance() :
                       &SerializedAccessor::getInstance())
{
}

a_util::result::Result StaticDecoder::isValid() const
{
    RETURN_IF_FAILED(_layout->isValid());
    if (_data_size < getStaticBufferSize(getRepresentation()))
    {
        return ERR_INVALID_ARG;
    }
    return a_util::result::SUCCESS;
}

size_t StaticDecoder::getElementCount() const
{
    return _layout->getStaticElements().size();
}

a_util::result::Result StaticDecoder::getElement(size_t nIndex, const StructElement*& pElement) const
{
    pElement = getLayoutElement(nIndex);
    if (!pElement)
    {
        return ERR_INVALID_INDEX;
    }

    return a_util::result::SUCCESS;
}

a_util::result::Result StaticDecoder::getElementValue(size_t nIndex, void* pValue) const
{
    const StructLayoutElement* pElement = getLayoutElement(nIndex);
    if (!pElement)
    {
        return ERR_INVALID_INDEX;
    }
    return _element_accessor->getValue(*pElement, _data, _data_size, pValue);
}

a_util::result::Result StaticDecoder::getElementValue(size_t nIndex, a_util::variant::Variant& oValue) const
{
    const StructLayoutElement* pElement = getLayoutElement(nIndex);
    if (!pElement)
    {
        return ERR_INVALID_INDEX;
    }
    return _element_accessor->getValue(*pElement, _data, _data_size, oValue);
}

const void* StaticDecoder::getElementAddress(size_t nIndex) const
{
    const StructLayoutElement* pElement = getLayoutElement(nIndex);
    if (!pElement)
    {
        return NULL;
    }

    size_t nBitPos = _element_accessor->getRepresentation() == deserialized ?
                        pElement->deserialized.bit_offset :
                        pElement->serialized.bit_offset;

    if (nBitPos % 8)
    {
        return NULL;
    }

    size_t bit_size = _element_accessor->getRepresentation() == deserialized ?
                        pElement->deserialized.bit_size :
                        pElement->serialized.bit_size;

    if ((nBitPos / 8) + (bit_size / 8) + (bit_size % 8 ? 1 : 0) >
        _data_size)
    {
        return NULL;
    }

    return static_cast<const uint8_t*>(_data) + (nBitPos / 8);
}

const StructLayoutElement* StaticDecoder::getLayoutElement(size_t nIndex) const
{
    const StructLayoutElement* pElement = NULL;
    if (nIndex < _layout->getStaticElements().size())
    {
        pElement = &_layout->getStaticElements()[nIndex];
    }

    return pElement;
}

size_t StaticDecoder::getStaticBufferSize(DataRepresentation eRep) const
{
    return _layout->getStaticBufferSize(eRep);
}

DataRepresentation StaticDecoder::getRepresentation() const
{
    return _element_accessor->getRepresentation();
}

StaticCodec::StaticCodec(a_util::memory::shared_ptr<const StructLayout> pLayout,
                           void* pData, size_t nDataSize, DataRepresentation eRep):
    StaticDecoder(pLayout, pData, nDataSize, eRep)
{
}

a_util::result::Result StaticCodec::setElementValue(size_t nIndex, const void* pValue)
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

a_util::result::Result StaticCodec::setElementValue(size_t nIndex, const a_util::variant::Variant& oValue)
{
    const StructLayoutElement* pElement = getLayoutElement(nIndex);
    if (!pElement)
    {
        return ERR_INVALID_INDEX;
    }
    return _element_accessor->setValue(*pElement, const_cast<void*>(_data),
                                        _data_size, oValue);
}

void* StaticCodec::getElementAddress(size_t nIndex)
{
    return const_cast<void*>(StaticDecoder::getElementAddress(nIndex));
}

a_util::result::Result StaticCodec::setConstants()
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
