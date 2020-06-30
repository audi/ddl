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

#include <assert.h> 
#include "a_util/result/error_def.h"
#include "a_util/variant.h"
#include "a_util/memory.h"
#include "element_accessor.h"
#include "legacy_error_macros.h"
#include "bitserializer.h"

namespace ddl
{

//define all needed error types and values locally
_MAKE_RESULT(-5, ERR_INVALID_ARG);
_MAKE_RESULT(-19, ERR_NOT_SUPPORTED);

template <typename T>
a_util::result::Result get_typed_element_value(const StructLayoutElement& sElement, const void* pData,
                                size_t nDataSize, a_util::variant::Variant& oValue,
                                const ElementAccessor& oAccessor)
{
    T xValue;
    RETURN_IF_FAILED(oAccessor.getValue(sElement, pData, nDataSize, &xValue));
    oValue = xValue;
    return a_util::result::SUCCESS;
}

#define GET_CASE_TYPE(__variant_type, __data_type) \
    case a_util::variant::__variant_type: \
{ \
    return get_typed_element_value<__data_type>(sElement, pData, nDataSize, oValue, *this); \
}

template <typename T>
a_util::result::Result set_typed_element_value(const StructLayoutElement& sElement, void* pData,
                                size_t nDataSize, const a_util::variant::Variant& oValue,
                                const ElementAccessor& oAccessor)
{
    T xHelper = oValue;
    return oAccessor.setValue(sElement, pData, nDataSize, &xHelper);
}

#define SET_CASE_TYPE(__variant_type, __data_type) \
    case a_util::variant::__variant_type: \
{ \
    return set_typed_element_value<__data_type>(sElement, pData, nDataSize, oValue, *this); \
}

a_util::result::Result ElementAccessor::getValue(const StructLayoutElement& sElement, const void* pData,
                                   size_t nDataSize, a_util::variant::Variant& oValue) const
{
    switch(sElement.type)
    {
        GET_CASE_TYPE(VT_Bool, bool)
        GET_CASE_TYPE(VT_Int8, int8_t)
        GET_CASE_TYPE(VT_UInt8, uint8_t)
        GET_CASE_TYPE(VT_Int16, int16_t)
        GET_CASE_TYPE(VT_UInt16, uint16_t)
        GET_CASE_TYPE(VT_Int32, int32_t)
        GET_CASE_TYPE(VT_UInt32, uint32_t)
        GET_CASE_TYPE(VT_Int64, int64_t)
        GET_CASE_TYPE(VT_UInt64, uint64_t)
        GET_CASE_TYPE(VT_Float32, float)
        GET_CASE_TYPE(VT_Float64, double)
        default: return ERR_NOT_SUPPORTED;
    }
}

a_util::result::Result ElementAccessor::setValue(const StructLayoutElement& sElement, void* pData,
                                   size_t nDataSize, const a_util::variant::Variant& oValue) const
{
    switch(sElement.type)
    {
        SET_CASE_TYPE(VT_Bool, bool)
        SET_CASE_TYPE(VT_Int8, int8_t)
        SET_CASE_TYPE(VT_UInt8, uint8_t)
        SET_CASE_TYPE(VT_Int16, int16_t)
        SET_CASE_TYPE(VT_UInt16, uint16_t)
        SET_CASE_TYPE(VT_Int32, int32_t)
        SET_CASE_TYPE(VT_UInt32, uint32_t)
        SET_CASE_TYPE(VT_Int64, int64_t)
        SET_CASE_TYPE(VT_UInt64, uint64_t)
        SET_CASE_TYPE(VT_Float32, float)
        SET_CASE_TYPE(VT_Float64, double)
        default: return ERR_NOT_SUPPORTED;
    }
}

a_util::result::Result DeserializedAccessor::getValue(const StructLayoutElement& sElement, const void* pData,
                                        size_t nDataSize, void* pElementValue) const
{
    assert(sElement.deserialized.bit_offset % 8 == 0);
    assert(sElement.deserialized.bit_size % 8 == 0);
    size_t nByteOffset = sElement.deserialized.bit_offset / 8;
    size_t nByteSize = sElement.deserialized.bit_size / 8;
    if (nDataSize < nByteOffset + nByteSize)
    {
        return ERR_INVALID_ARG;
    }
    a_util::memory::copy(pElementValue, nByteSize,
                            reinterpret_cast<const void*>(static_cast<const uint8_t*>(pData) + nByteOffset),
                            nByteSize);
    return a_util::result::SUCCESS;
}

a_util::result::Result DeserializedAccessor::setValue(const StructLayoutElement& sElement, void* pData,
                                        size_t nDataSize, const void* pElementValue) const
{
    assert(sElement.deserialized.bit_offset % 8 == 0);
    assert(sElement.deserialized.bit_size % 8 == 0);
    size_t nByteOffset = sElement.deserialized.bit_offset / 8;
    size_t nByteSize = sElement.deserialized.bit_size / 8;
    if (nDataSize < nByteOffset + nByteSize)
    {
        return ERR_INVALID_ARG;
    }
    a_util::memory::copy(static_cast<uint8_t*>(pData) + nByteOffset, nByteSize,
                          pElementValue,
                          nByteSize);
    return a_util::result::SUCCESS;
}

const ElementAccessor& DeserializedAccessor::getInstance()
{
    static DeserializedAccessor oInstance;
    return oInstance;
}

const ElementAccessor& SerializedAccessor::getInstance()
{
    static SerializedAccessor oInstance;
    return oInstance;
}


template <typename T>
a_util::result::Result read_typed_bits(const StructLayoutElement& sElement, const void* pData,
                        size_t nDataSize, void* pElementValue)
{
    a_util::memory::BitSerializer oHelper(const_cast<void*>(pData), nDataSize);
    return oHelper.read<T>(sElement.serialized.bit_offset, sElement.serialized.bit_size,
                           reinterpret_cast<T*>(pElementValue),
                           (a_util::memory::Endianess)sElement.byte_order);
}

#define GET_CASE_TYPE_SER(__variant_type, __data_type) \
    case a_util::variant::__variant_type: \
{ \
    return read_typed_bits<__data_type>(sElement, pData, nDataSize, pElementValue); \
}

a_util::result::Result SerializedAccessor::getValue(const StructLayoutElement& sElement, const void* pData,
                                      size_t nDataSize, void* pElementValue) const
{
    switch(sElement.type)
    {
        GET_CASE_TYPE_SER(VT_Bool, bool)
        GET_CASE_TYPE_SER(VT_Int8, int8_t)
        GET_CASE_TYPE_SER(VT_UInt8, uint8_t)
        GET_CASE_TYPE_SER(VT_Int16, int16_t)
        GET_CASE_TYPE_SER(VT_UInt16, uint16_t)
        GET_CASE_TYPE_SER(VT_Int32, int32_t)
        GET_CASE_TYPE_SER(VT_UInt32, uint32_t)
        GET_CASE_TYPE_SER(VT_Int64, int64_t)
        GET_CASE_TYPE_SER(VT_UInt64, uint64_t)
        GET_CASE_TYPE_SER(VT_Float32, float)
        GET_CASE_TYPE_SER(VT_Float64, double)
        default: return ERR_NOT_SUPPORTED;
    }
}

template <typename T>
a_util::result::Result write_typed_bits(const StructLayoutElement& sElement, void* pData,
                         size_t nDataSize, const void* pElementValue)
{
    a_util::memory::BitSerializer oHelper(pData, nDataSize);
    return oHelper.write<T>(sElement.serialized.bit_offset, sElement.serialized.bit_size,
                            *reinterpret_cast<const T*>(pElementValue),
                            (a_util::memory::Endianess)sElement.byte_order);
}

#define SET_CASE_TYPE_SER(__variant_type, __data_type) \
    case a_util::variant::__variant_type: \
{ \
    return write_typed_bits<__data_type>(sElement, pData, nDataSize, pElementValue); \
}

a_util::result::Result SerializedAccessor::setValue(const StructLayoutElement& sElement, void* pData,
                                      size_t nDataSize, const void* pElementValue) const
{
    switch(sElement.type)
    {
        SET_CASE_TYPE_SER(VT_Bool, bool)
        SET_CASE_TYPE_SER(VT_Int8, int8_t)
        SET_CASE_TYPE_SER(VT_UInt8, uint8_t)
        SET_CASE_TYPE_SER(VT_Int16, int16_t)
        SET_CASE_TYPE_SER(VT_UInt16, uint16_t)
        SET_CASE_TYPE_SER(VT_Int32, int32_t)
        SET_CASE_TYPE_SER(VT_UInt32, uint32_t)
        SET_CASE_TYPE_SER(VT_Int64, int64_t)
        SET_CASE_TYPE_SER(VT_UInt64, uint64_t)
        SET_CASE_TYPE_SER(VT_Float32, float)
        SET_CASE_TYPE_SER(VT_Float64, double)
        default: return ERR_NOT_SUPPORTED;
    }
}


}
