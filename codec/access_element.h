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
#ifndef DDL_STRUCT_ELEMENT_ACCESS_CLASS_HEADER
#define DDL_STRUCT_ELEMENT_ACCESS_CLASS_HEADER

#include <a_util/result.h>

#include "struct_element.h"
#include "codec_factory.h"

namespace ddl
{

namespace access_element
{

//define all needed error types and values locally
_MAKE_RESULT(-3, ERR_UNEXPECTED);
_MAKE_RESULT(-20, ERR_NOT_FOUND);

namespace detail
{

/// For internal use only. @internal
template <typename T>
static inline size_t getElementCount(const T& type)
{
    return type.getElementCount();
}

/// For internal use only. @internal
template <typename T>
static inline a_util::result::Result getElement(const T& type, size_t element_index, const StructElement*& element)
{
    return type.getElement(element_index, element);
}

/// For internal use only. @internal
template <>
inline size_t getElementCount(const CodecFactory& type)
{
    return type.getStaticElementCount();
}

/// For internal use only. @internal
template <>
inline a_util::result::Result getElement(const CodecFactory& type, size_t element_index, const StructElement*& element)
{
    return type.getStaticElement(element_index, element);
}

/// For internal use only. @internal
template <typename T>
a_util::result::Result find_complex_index(const T& decoder, const std::string& struct_name,
                           size_t& index, const char* post_fix)
{
    if (struct_name.empty())
    {
        index = 0;
        return a_util::result::SUCCESS;
    }

    size_t element_count = getElementCount(decoder);
    std::string prefix = struct_name + post_fix;
    for (size_t element_index = 0; element_index < element_count; ++element_index)
    {
        const StructElement* element;
        if (isOk(getElement(decoder, element_index, element)))
        {
            
            if (a_util::strings::compare(element->name.c_str(), prefix.c_str(), 0, prefix.size()) == 0)
            {
                index = element_index;
                return a_util::result::SUCCESS;
            }
        }
    }

    return ERR_NOT_FOUND;
}

}

/**
 * find the index of an element by name.
 * @param[in] decoder The decoder or factory.
 * @param[in] element_name The name of the element.
 * @param[out] index The index of the found element.
 * @retval ERR_NOT_FOUND No element with the requested name was found.
 */
template <typename T>
a_util::result::Result find_index(const T& decoder, const std::string& element_name, size_t& index)
{
    size_t element_count = detail::getElementCount(decoder);
    for (size_t element_index = 0; element_index < element_count; ++element_index)
    {
        const StructElement* element;
        if (isOk(detail::getElement(decoder, element_index, element)))
        {
            if (element->name == element_name)
            {
                index = element_index;
                return a_util::result::SUCCESS;
            }
        }
    }

    return ERR_NOT_FOUND;
}

/**
 * find the index of the first element of a sub-structure by name.
 * @param[in] decoder The decoder or factory.
 * @param[in] struct_name The name of the sub-structure.
 * @param[out] index The index of the found element.
 * @retval ERR_NOT_FOUND No element with the requested name was found.
 */
template <typename T>
a_util::result::Result find_struct_index(const T& decoder, const std::string& struct_name, size_t& index)
{
    return detail::find_complex_index<T>(decoder, struct_name, index, ".");
}

/**
 * find the index of the first element of an array by name.
 * @param[in] decoder The decoder or factory.
 * @param[in] array_name The name of the array.
 * @param[out] index The index of the found element.
 * @retval ERR_NOT_FOUND No element with the requested name was found.
 */
template <typename T>
a_util::result::Result find_array_index(const T& decoder, const std::string& array_name, size_t& index)
{
    return detail::find_complex_index<T>(decoder, array_name, index, "[");
}

/**
 * find the index of the first element after an array by name.
 * @param[in] decoder The decoder or factory.
 * @param[in] array_name The name of the array.
 * @param[out] index The index of the found element.
 * @retval ERR_NOT_FOUND No element with the requested name was found.
 */
template <typename T>
a_util::result::Result find_array_end_index(const T& decoder, const std::string& array_name, size_t& index)
{
    size_t element_count = detail::getElementCount(decoder);
    std::string prefix = array_name + "[";
    for (index += 1; index < element_count; ++index)
    {
        const StructElement* element;
        if (isFailed(detail::getElement(decoder, index, element)))
        {
            return a_util::result::SUCCESS;
        }

        if (a_util::strings::compare(element->name.c_str(), prefix.c_str(), 0, prefix.size()) != 0)
        {
            return a_util::result::SUCCESS;
        }
    }

    return a_util::result::SUCCESS;
}

/**
 * Get the value of an element by name.
 * @param[in] decoder The decoder.
 * @param[in] element_name The name of the element.
 * @param[out] value The location where the value should be copied to.
 * @retval ERR_NOT_FOUND No element with the requested name was found.
 */
template <typename T>
a_util::result::Result get_value(const T& decoder, const std::string& element_name, void* value)
{
    size_t element_index;
    a_util::result::Result res = find_index(decoder, element_name, element_index);
    if (a_util::result::isFailed(res)) return res;

    return decoder.getElementValue(element_index, value);
}

/**
 * Set the value of an element by name.
 * @param[in] codec The codec.
 * @param[in] element_name The name of the element.
 * @param[in] value The location where the value should be copied from.
 * @retval ERR_NOT_FOUND No element with the requested name was found.
 */
template <typename T>
a_util::result::Result set_value(T& codec, const std::string& element_name, const void* value)
{
    size_t element_index;
    a_util::result::Result res = find_index(codec, element_name, element_index);
    if (a_util::result::isFailed(res)) return res;

    return codec.setElementValue(element_index, value);
}

/**
 * Get the value of an element by name.
 * @param[in] decoder The decoder.
 * @param[in] element_name The name of the element.
 * @retval The value of the element.
 */
template <typename T>
a_util::variant::Variant get_value(const T& decoder, const std::string& element_name)
{
    a_util::variant::Variant result;
    size_t element_index;
    if (isOk(find_index(decoder, element_name, element_index)))
    {
        decoder.getElementValue(element_index, result);
    }
    return result;
}

/**
 * Set the value of an element by name.
 * @param[in] codec The codec.
 * @param[in] element_name The name of the element.
 * @param[in] value The new value.
 * @retval ERR_NOT_FOUND No element with the requested name was found.
 */
template <typename T>
a_util::result::Result set_value(T& codec, const std::string& element_name, const a_util::variant::Variant& value)
{
    size_t element_index;
    a_util::result::Result res = find_index(codec, element_name, element_index);
    if (a_util::result::isFailed(res)) return res;

    return codec.setElementValue(element_index, value);
}

/**
 * Get the value of an element by index.
 * @param[in] decoder The decoder.
 * @param[in] element_index The index of the element.
 * @retval The value of the element.
 */
template <typename T>
a_util::variant::Variant get_value(const T& decoder, size_t element_index)
{
    a_util::variant::Variant result;
    if (isOk(decoder.getElementValue(element_index, result)))
    {
        return result;
    }

    return a_util::variant::Variant(static_cast<int32_t>(0));
}

/**
 * Get a pointer to an element by name.
 * @param[in] decoder The decoder.
 * @param[in] element_name The name of the element.
 * @return Address of the element.
 */
template <typename T>
const void* get_value_address(const T& decoder, const std::string& element_name)
{
    size_t element_index;
    if (isOk(find_index(decoder, element_name, element_index)))
    {
        return decoder.getElementAddress(element_index);
    }

    return NULL;
}

/**
 * Get a pointer to an element by name.
 * @param[in] codec The codec.
 * @param[in] element_name The name of the element.
 * @return Address of the element.
 */
template <typename T>
void* get_value_address(T& codec, const std::string& element_name)
{
    size_t element_index;
    if (isOk(find_index(codec, element_name, element_index)))
    {
        return codec.getElementAddress(element_index);
    }

    return NULL;
}

/**
 * Get a pointer to a sub-structure by name.
 * @param[in] decoder The codec.
 * @param[in] struct_name The name of the structure.
 * @return Address of the sub-structure.
 */
template <typename STRUCT, typename T>
const STRUCT* get_struct_address(const T& decoder, const std::string& struct_name)
{
    size_t element_index;
    if (isOk(find_struct_index(decoder, struct_name, element_index)))
    {
        return reinterpret_cast<const STRUCT*>(decoder.getElementAddress(element_index));
    }

    return NULL;
}

/**
 * Get a pointer to a sub-structure by name.
 * @param[in] codec The codec.
 * @param[in] struct_name The name of the structure.
 * @return Address of the sub-structure.
 */
template <typename STRUCT, typename T>
STRUCT* get_struct_address(T& codec, const std::string& struct_name)
{
    size_t element_index;
    if (isOk(find_struct_index(codec, struct_name, element_index)))
    {
        return reinterpret_cast<STRUCT*>(codec.getElementAddress(element_index));
    }

    return NULL;
}

/**
 * Copy a sub-structure out of the structure.
 * @param[in] decoder The decoder.
 * @param[in] struct_name The name of the structure.
 * @param[out] struct_value The location the sub-structure will be copied to.
 * @retval ERR_NOT_FOUND No structure with the requested name was found.
 */
template <typename T, typename CODEC>
a_util::result::Result get_struct_value(const CODEC& decoder, const std::string& struct_name, T* struct_value)
{
    const T* address = get_struct_address<T>(decoder, struct_name);
    if (!address)
    {
        return ERR_NOT_FOUND;
    }

    a_util::memory::copy(struct_value, sizeof(T), address, sizeof(T));
    return a_util::result::SUCCESS;
}

/**
 * Copy a sub-structure into the structure.
 * @param[in] codec The codec.
 * @param[in] struct_name The name of the structure.
 * @param[in] struct_value The location the sub-structure will be copied from.
 * @retval ERR_NOT_FOUND No structure with the requested name was found.
 */
template <typename T, typename CODEC>
a_util::result::Result set_struct_value(CODEC& codec, const std::string& struct_name, const T* struct_value)
{
    T* address = get_struct_address<T>(codec, struct_name);
    if (!address)
    {
        return ERR_NOT_FOUND;
    }

    a_util::memory::copy(address, sizeof(T), struct_value, sizeof(T));
    return a_util::result::SUCCESS;
}

/**
 * Get a pointer to an array by name.
 * @param[in] decoder The decoder.
 * @param[in] array_name The name of the array.
 * @return Address of the array.
 */
template <typename ARRAY_TYPE, typename T>
const ARRAY_TYPE* get_array_address(const T& decoder, const std::string& array_name)
{
    size_t element_index;
    if (isOk(find_array_index(decoder, array_name, element_index)))
    {
        return reinterpret_cast<const ARRAY_TYPE*>(decoder.getElementAddress(element_index));
    }

    return NULL;
}

/**
 * Get a pointer to an array by name.
 * @param[in] codec The codec.
 * @param[in] array_name The name of the array.
 * @return Address of the array.
 */
template <typename ARRAY_TYPE, typename T>
ARRAY_TYPE* get_array_address(T& codec, const std::string& array_name)
{
    size_t element_index;
    if (isOk(find_array_index(codec, array_name, element_index)))
    {
        return reinterpret_cast<ARRAY_TYPE*>(codec.getElementAddress(element_index));
    }

    return NULL;
}

/**
 * Get information about an array.
 * @param[in] decoder The decoder.
 * @param[in] array_name The name of the array.
 * @param[out] start_address The address of the first element of the array.
 * @param[out] size The amount of elements of the array.
 * @retval ERR_NOT_FOUND No array with the requested name was found.
 */
template <typename CODEC>
a_util::result::Result get_array(const CODEC& decoder, const std::string& array_name,
                  const void*& start_address, size_t& size)
{
    size_t start_index = 0;
    a_util::result::Result res = find_array_index(decoder, array_name, start_index);
    if (a_util::result::isFailed(res)) return res;

    size_t end_index = start_index;
    res = find_array_end_index(decoder, array_name, end_index);
    if (a_util::result::isFailed(res)) return res;

    start_address = decoder.getElementAddress(start_index);
    if (!start_address)
    {
        return ERR_UNEXPECTED;
    }

    const void* end_adress = decoder.getElementAddress(end_index);
    if (end_adress)
    {
        size = static_cast<const uint8_t*>(end_adress) -
                static_cast<const uint8_t*>(start_address);
    }
    else
    {
        // it reaches til the end
        size_t start_offset = static_cast<const uint8_t*>(start_address) -
                             static_cast<const uint8_t*>(decoder.getElementAddress(0));
        size = decoder.getBufferSize(decoder.getRepresentation()) - start_offset;
    }

    return a_util::result::SUCCESS;
}

/**
 * Copy an array out of the structure.
 * @param[in] decoder The decoder.
 * @param[in] array_name The name of the array.
 * @param[out] array_value The location the array will be copied to.
 * @retval ERR_NOT_FOUND No array with the requested name was found.
 */
template <typename T, typename CODEC>
a_util::result::Result get_array_value(const CODEC& decoder, const std::string& array_name, T* array_value)
{
    const void* start_address;
    size_t size;
    a_util::result::Result res = get_array(decoder, array_name, start_address, size);
    if (a_util::result::isFailed(res)) return res;

    a_util::memory::copy(array_value, size, start_address, size);
    return a_util::result::SUCCESS;
}

/**
 * Set the value of the requested element to zero.
 * @param[in] codec The codec.
 * @param[in] element_name The name of the element.
 * @retval ERR_NOT_FOUND No element with the requested name was found.
 */
template <typename T>
a_util::result::Result reset(T& codec, const std::string& element_name)
{
    size_t element_index;
    a_util::result::Result res = find_index(codec, element_name, element_index);
    if (a_util::result::isFailed(res)) return res;

    uint64_t zero = 0;
    return codec.setElementValue(element_index, &zero);
}

///For internal use only. @internal
#define GET_ENUM_CASE(__variant_type, __data_type) \
case a_util::variant::VT_##__variant_type:\
{\
    __data_type xValue = value.get##__variant_type();\
    for (EnumType::const_iterator it = element->p_enum->begin(); it != element->p_enum->end(); ++it)\
    {\
        if (xValue == it->second.as##__variant_type())\
        {\
            return it->first;\
        }\
    }\
    break;\
}

/**
 * Get the value of an element as a string, using enum value names if available.
 * @param[in] decoder The decoder.
 * @param[in] element The index of the element.
 * @return A string representation of the value.
 */
template <typename T>
std::string get_value_as_string(const T& decoder, size_t element_index)
{
    a_util::variant::Variant value;
    const StructElement* element;
    if (isOk(decoder.getElement(element_index, element)))
    {
        if (isOk(decoder.getElementValue(element_index, value)))
        {
            if (element->p_enum)
            {
                switch (value.getType())
                {
                    GET_ENUM_CASE(Bool, bool)
                    GET_ENUM_CASE(Int8, int8_t)
                    GET_ENUM_CASE(UInt8, uint8_t)
                    GET_ENUM_CASE(Int16, int16_t)
                    GET_ENUM_CASE(UInt16, uint16_t)
                    GET_ENUM_CASE(Int32, int32_t)
                    GET_ENUM_CASE(UInt32, uint32_t)
                    GET_ENUM_CASE(Int64, int64_t)
                    GET_ENUM_CASE(UInt64, uint64_t)
                    GET_ENUM_CASE(Float, float)
                    GET_ENUM_CASE(Double, double)
                    default: break;
                }
            }
        }
    }

    return value.asString();
}

/**
 * Get the value of an element as a string, using enum value names if available.
 * @param[in] decoder The decoder.
 * @param[in] element_name The name of the element.
 * @return A string representation of the value.
 */
template <typename T>
std::string get_value_as_string(const T& decoder, const std::string& element_name)
{
    size_t element_index;
    if (isOk(find_index(decoder, element_name, element_index)))
    {
        return get_value_as_string(decoder, element_index);
    }
    return "";
}

}

}

#endif

