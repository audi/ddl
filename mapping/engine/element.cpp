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

#include "element.h"

using namespace mapping;
using namespace mapping::rt;

namespace mapping
{
namespace rt
{
    //define all needed error types and values locally
    _MAKE_RESULT(-3, ERR_UNEXPECTED)
    _MAKE_RESULT(-5, ERR_INVALID_ARG)
    _MAKE_RESULT(-42, ERR_INVALID_TYPE)
}
}

/// Helper method that sets a casted value in the coder
template <typename T>
static void SetCastedValue(void* pDestination, unsigned int nArrayIndex, T* pData,
    uint32_t type32)
{
    switch (type32)
    {
    case e_uint8:
        {
            uint8_t ui8Temp = static_cast<uint8_t>(*pData);
            a_util::memory::copy((uint8_t*)pDestination + nArrayIndex, sizeof(uint8_t), &ui8Temp, sizeof(uint8_t));
        }
        break;
    case e_uint16:
        {
            uint16_t ui16Temp = static_cast<uint16_t>(*pData);
            a_util::memory::copy((uint16_t*)pDestination + nArrayIndex, sizeof(uint16_t), &ui16Temp, sizeof(uint16_t));
        }
        break;
    case e_uint32:
        {
            uint32_t ui32Temp = static_cast<uint32_t>(*pData);
            a_util::memory::copy((uint32_t*)pDestination + nArrayIndex, sizeof(uint32_t), &ui32Temp, sizeof(uint32_t));
        }
        break;
    case e_uint64:
        {
            uint64_t ui64Temp = static_cast<uint64_t>(*pData);
            a_util::memory::copy((uint64_t*)pDestination + nArrayIndex, sizeof(uint64_t), &ui64Temp, sizeof(uint64_t));
        }
        break;
    case e_int8:
        {
            int8_t i8Temp = static_cast<int8_t>(*pData);
            a_util::memory::copy((int8_t*)pDestination + nArrayIndex, sizeof(int8_t), &i8Temp, sizeof(int8_t));
        }
        break;
    case e_int16:
        {
            int16_t i16Temp = static_cast<int16_t>(*pData);
            a_util::memory::copy((int16_t*)pDestination + nArrayIndex, sizeof(int16_t), &i16Temp, sizeof(int16_t));
        }
        break;
    case e_int32:
        {
            int32_t i32Temp = static_cast<int32_t>(*pData);
            a_util::memory::copy((int32_t*)pDestination + nArrayIndex, sizeof(int32_t), &i32Temp, sizeof(int32_t));
        }
        break;
    case e_int64:
        {
            int64_t i64Temp = static_cast<int64_t>(*pData);
            a_util::memory::copy((int64_t*)pDestination + nArrayIndex, sizeof(int64_t), &i64Temp, sizeof(int64_t));
        }
        break;
    case e_float32:
        {
            float f32Temp = static_cast<float>(*pData);
            a_util::memory::copy((float*)pDestination + nArrayIndex, sizeof(float), &f32Temp, sizeof(float));
        }
        break;
    case e_float64:
        {
            double f64Temp = static_cast<double>(*pData);
            a_util::memory::copy((double*)pDestination + nArrayIndex, sizeof(double), &f64Temp, sizeof(double));
        }
        break;
    case e_bool:
        {
            bool bTemp = *pData != 0 ? true : false;
            a_util::memory::copy((bool*)pDestination + nArrayIndex, sizeof(bool), &bTemp, sizeof(bool));
        }
        break;
    case e_char:
        {
            char cTemp = static_cast<char>(*pData);
            a_util::memory::copy((char*)pDestination + nArrayIndex, sizeof(char), &cTemp, sizeof(char));
        }
        break;
    default:
        assert(false);
        break;
    }
}

template <>
void SetCastedValue<bool>(void* pDestination, unsigned int nArrayIndex, bool* pData,
    uint32_t type32)
{
    int nBool = *pData ? 1 : 0;
    switch (type32)
    {
    case e_uint8:
        {
            uint8_t ui8Temp = static_cast<uint8_t>(nBool);
            a_util::memory::copy((uint8_t*)pDestination + nArrayIndex, sizeof(uint8_t), &ui8Temp, sizeof(uint8_t));
        }
        break;
    case e_uint16:
        {
            uint16_t ui16Temp = static_cast<uint16_t>(nBool);
            a_util::memory::copy((uint16_t*)pDestination + nArrayIndex, sizeof(uint16_t), &ui16Temp, sizeof(uint16_t));
        }
        break;
    case e_uint32:
        {
            uint32_t ui32Temp = static_cast<uint32_t>(nBool);
            a_util::memory::copy((uint32_t*)pDestination + nArrayIndex, sizeof(uint32_t), &ui32Temp, sizeof(uint32_t));
        }
        break;
    case e_uint64:
        {
            uint64_t ui64Temp = static_cast<uint64_t>(nBool);
            a_util::memory::copy((uint64_t*)pDestination + nArrayIndex, sizeof(uint64_t), &ui64Temp, sizeof(uint64_t));
        }
        break;
    case e_int8:
        {
            int8_t i8Temp = static_cast<int8_t>(nBool);
            a_util::memory::copy((int8_t*)pDestination + nArrayIndex, sizeof(int8_t), &i8Temp, sizeof(int8_t));
        }
        break;
    case e_int16:
        {
            int16_t i16Temp = static_cast<int16_t>(nBool);
            a_util::memory::copy((int16_t*)pDestination + nArrayIndex, sizeof(int16_t), &i16Temp, sizeof(int16_t));
        }
        break;
    case e_int32:
        {
            int32_t i32Temp = static_cast<int32_t>(nBool);
            a_util::memory::copy((int32_t*)pDestination + nArrayIndex, sizeof(int32_t), &i32Temp, sizeof(int32_t));
        }
        break;
    case e_int64:
        {
            int64_t i64Temp = static_cast<int64_t>(nBool);
            a_util::memory::copy((int64_t*)pDestination + nArrayIndex, sizeof(int64_t), &i64Temp, sizeof(int64_t));
        }
        break;
    case e_float32:
        {
            float f32Temp = static_cast<float>(nBool);
            a_util::memory::copy((float*)pDestination + nArrayIndex, sizeof(float), &f32Temp, sizeof(float));
        }
        break;
    case e_float64:
        {
            double f64Temp = static_cast<double>(nBool);
            a_util::memory::copy((double*)pDestination + nArrayIndex, sizeof(double), &f64Temp, sizeof(double));
        }
        break;
    case e_bool:
        a_util::memory::copy((bool*)pDestination + nArrayIndex, sizeof(bool), pData, sizeof(bool));
        break;
    case e_char:
        {
            char cTemp = static_cast<char>(nBool);
            a_util::memory::copy((char*)pDestination + nArrayIndex, sizeof(char), &cTemp, sizeof(char));
        }
        break;
    default:
        assert(false);
        break;
    }
}

template <typename T>
static inline void TransformAndSetValueImpl(const void* pData, void* pDestination,
    unsigned int nArraySize, const oo::MapTransformationBase* pTrans, uint32_t type32)
{
    assert(pTrans);
    assert(pData);
    for(unsigned int i = 0; i < nArraySize; i++)
    {
        double fValue = pTrans->evaluate(static_cast<double>(*((T*)pData + i)));
        SetCastedValue(pDestination, i, &fValue, type32);
    }
}

template <typename T>
static inline void SetValueImpl(const void* pData, void* pDestination, unsigned int nArraySize,
    uint32_t type32)
{
    assert(pData);
    for(unsigned int i = 0; i < nArraySize; i++)
    {
        SetCastedValue<T>(pDestination, i, (T*)pData + i, type32);
    }
}

TargetElement::TargetElement(Target* pTarget) :
    _target(pTarget), _element_ptr(NULL), _type_int(0),
    _type(NULL), _struct(NULL),
    _array_size(0), _transformation(NULL)
{
    _type_map["tUInt8"] =   e_uint8;
    _type_map["tUInt16"] =  e_uint16;
    _type_map["tUInt32"] =  e_uint32;
    _type_map["tUInt64"] =  e_uint64;
    _type_map["tInt8"] =    e_int8;
    _type_map["tInt16"] =   e_int16;
    _type_map["tInt32"] =   e_int32;
    _type_map["tInt64"] =   e_int64;
    _type_map["tFloat32"] = e_float32;
    _type_map["tFloat64"] = e_float64;
    _type_map["tBool"] =    e_bool;
    _type_map["tChar"] =    e_char;
}

TargetElement::~TargetElement()
{
    delete _transformation;
    _transformation = NULL;
}

a_util::result::Result TargetElement::create(void* pElementPtr, ddl::IDDLDataType* pDataType,
    const unsigned int nArraySize, const std::string& strElementName)
{
    _element_ptr = pElementPtr;
    _array_size = nArraySize;
    _element_name = strElementName;

    _type = dynamic_cast<ddl::DDLDataType*>(pDataType);
    _struct = dynamic_cast<ddl::DDLComplex*>(pDataType);
    _enum = dynamic_cast<ddl::DDLEnum*>(pDataType);

    if (_enum)
    {
        _type = dynamic_cast<ddl::DDLDataType*>(_enum->getTypeObject());
        if (!_type)
        {
            return  ERR_UNEXPECTED;
        }

        _enum = NULL;
    }

    if(_type)
    {
        TypeMap::const_iterator it = _type_map.find(_type->getName());
        if(it != _type_map.end())
        {
            _type_int = it->second;
        }
    }

    return a_util::result::SUCCESS;
}

a_util::result::Result TargetElement::setTransformation(const oo::MapTransformationBase* pTransformation)
{
    if (_transformation)
    {
        delete _transformation;
        _transformation = NULL;
    }

    if (pTransformation)
    {
        _transformation = pTransformation->clone();
    }

    return a_util::result::SUCCESS;
}

a_util::result::Result TargetElement::setValue(const void* pData, uint32_t ui32SrcType, size_t szMem)
{
    // Scalars (and enums converted to scalars)
    if(_type)
    {
        if(_transformation)
        {
            switch(ui32SrcType)
            {
            case e_uint8:
                TransformAndSetValueImpl<uint8_t>(pData, _element_ptr, _array_size, _transformation, _type_int);
                break;
            case e_uint16:
                TransformAndSetValueImpl<uint16_t>(pData, _element_ptr, _array_size, _transformation, _type_int);
                break;
            case e_uint32:
                TransformAndSetValueImpl<uint32_t>(pData, _element_ptr, _array_size, _transformation, _type_int);
                break;
            case e_uint64:
                TransformAndSetValueImpl<uint64_t>(pData, _element_ptr, _array_size, _transformation, _type_int);
                break;
            case e_int8:
                TransformAndSetValueImpl<int8_t>(pData, _element_ptr, _array_size, _transformation, _type_int);
                break;
            case e_int16:
                TransformAndSetValueImpl<int16_t>(pData, _element_ptr, _array_size, _transformation, _type_int);
                break;
            case e_int32:
                TransformAndSetValueImpl<int32_t>(pData, _element_ptr, _array_size, _transformation, _type_int);
                break;
            case e_int64:
                TransformAndSetValueImpl<int64_t>(pData, _element_ptr, _array_size, _transformation, _type_int);
                break;
            case e_float32:
                TransformAndSetValueImpl<float>(pData, _element_ptr, _array_size, _transformation, _type_int);
                break;
            case e_float64:
                TransformAndSetValueImpl<double>(pData, _element_ptr, _array_size, _transformation, _type_int);
                break;
            case e_bool:
                TransformAndSetValueImpl<uint8_t>(pData, _element_ptr, _array_size, _transformation, _type_int);
                break;
            case e_char:
                TransformAndSetValueImpl<char>(pData, _element_ptr, _array_size, _transformation, _type_int);
                break;
            default: 
                return ERR_INVALID_TYPE;
            }
        }
        else if(_type_int == ui32SrcType)
        {
            // Source and target have the same type
            a_util::memory::copy(_element_ptr, szMem, pData, szMem);
        }
        else
        {
            // Source and target have different types
            switch(ui32SrcType)
            {
            case e_uint8:
                SetValueImpl<uint8_t>(pData, _element_ptr, _array_size, _type_int);
                break;
            case e_uint16:
                SetValueImpl<uint16_t>(pData, _element_ptr, _array_size, _type_int);
                break;
            case e_uint32:
                SetValueImpl<uint32_t>(pData, _element_ptr, _array_size, _type_int);
                break;
            case e_uint64:
                SetValueImpl<uint64_t>(pData, _element_ptr, _array_size, _type_int);
                break;
            case e_int8:
                SetValueImpl<int8_t>(pData, _element_ptr, _array_size, _type_int);
                break;
            case e_int16:
                SetValueImpl<int16_t>(pData, _element_ptr, _array_size, _type_int);
                break;
            case e_int32:
                SetValueImpl<int32_t>(pData, _element_ptr, _array_size, _type_int);
                break;
            case e_int64:
                SetValueImpl<int64_t>(pData, _element_ptr, _array_size, _type_int);
                break;
            case e_float32:
                SetValueImpl<float>(pData, _element_ptr, _array_size, _type_int);
                break;
            case e_float64:
                SetValueImpl<double>(pData, _element_ptr, _array_size, _type_int);
                break;
            case e_bool:
                SetValueImpl<uint8_t>(pData, _element_ptr, _array_size, _type_int);
                break;
            case e_char:
                SetValueImpl<char>(pData, _element_ptr, _array_size, _type_int);
                break;
            default: 
                return ERR_INVALID_TYPE;
            }
        }
    }
    else if(_struct) // Structures
    {
        // Transformations are not allowed
        a_util::memory::copy(_element_ptr, szMem, pData, szMem);
    }
    else
    {
        return ERR_INVALID_ARG;
    }

    return a_util::result::SUCCESS;
}

Target* TargetElement::getTarget()
{
    return _target;
}

a_util::result::Result TargetElement::setDefaultValue(const std::string& strDefault)
{
    // Scalars and  arrays
    if(_type)
    {
        double fVal = 0;
        if(!strDefault.empty() && a_util::strings::isDouble(strDefault))
        {
            fVal = a_util::strings::toDouble(strDefault);
        }
        
        for(unsigned int i = 0; i < _array_size; i++)
        {
            SetCastedValue<double>(_element_ptr, i, &fVal, _type_int);
        }
    }

    //Structures
    if(_struct)
    {
        //TODO: What is default value for a structure?
        return ERR_INVALID_ARG;
    }

    return a_util::result::SUCCESS;
}
