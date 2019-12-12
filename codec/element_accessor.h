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

#ifndef DDL_STRUCT_ELEMENT_ACCESS_PRIVATE_CLASS_HEADER
#define DDL_STRUCT_ELEMENT_ACCESS_PRIVATE_CLASS_HEADER

#include "a_util/result.h"
#include "struct_layout.h"

namespace ddl
{

class ElementAccessor
{
    public:
        virtual a_util::result::Result getValue(const StructLayoutElement& element, const void* data,
                                 size_t data_size, void* element_value) const = 0;
        virtual a_util::result::Result setValue(const StructLayoutElement& element, void* data,
                                 size_t data_size, const void* element_value) const = 0;

        virtual DataRepresentation getRepresentation() const = 0;

        a_util::result::Result getValue(const StructLayoutElement& element, const void* data,
                         size_t data_size, a_util::variant::Variant& value) const;

        a_util::result::Result setValue(const StructLayoutElement& element, void* data,
                         size_t data_size, const a_util::variant::Variant& value) const;
};

class DeserializedAccessor: public ElementAccessor
{    
    public:
        virtual a_util::result::Result getValue(const StructLayoutElement& element, const void* data,
                                 size_t data_size, void* element_value) const;
        virtual a_util::result::Result setValue(const StructLayoutElement& element, void* data,
                                 size_t data_size, const void* element_value) const;

        virtual DataRepresentation getRepresentation() const
        {
            return deserialized;
        }

        static const ElementAccessor& getInstance();
};


class SerializedAccessor: public ElementAccessor
{
    public:
        virtual a_util::result::Result getValue(const StructLayoutElement& element, const void* data,
                                 size_t data_size, void* element_value) const;
        virtual a_util::result::Result setValue(const StructLayoutElement& element, void* data,
                                 size_t data_size, const void* element_value) const;

        virtual DataRepresentation getRepresentation() const
        {
            return serialized;
        }

        static const ElementAccessor& getInstance();
};

}

#endif

