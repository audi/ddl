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

#ifndef DDL_CODEC_CLASS_HEADER
#define DDL_CODEC_CLASS_HEADER

#include "a_util/result.h"

#include "static_codec.h"

namespace ddl
{

class Codec;

/**
 * Decoder for dynamic structures defined by a DDL definition.
 */
class Decoder: public StaticDecoder
{
    public:
        /**
         * @copydoc StaticDecoder::isValid
         */
        virtual a_util::result::Result isValid() const;

        /**
         * @copydoc StaticDecoder::getElementCount
         */
        virtual size_t getElementCount() const;

        /**
         * @param[in] rep The data representation for which the buffer size should be returned.
         * @return The size of the structure in the requested data representation.
         */
        size_t getBufferSize(DataRepresentation rep = deserialized) const;

        /**
         * Create a new codec with the current dynamic structure layout for a new data buffer.
         * @param[in] data The pointer to the new raw data.
         * @param[in] data_size The size of the new raw data.
         * @param[in] rep The representation that the data should be encoded in.
         * @return A codec.
         */
        Codec makeCodecFor(void* data, size_t data_size, DataRepresentation rep) const;

    protected:
        friend class CodecFactory;
        /// For internal use only. @internal
        Decoder(a_util::memory::shared_ptr<const StructLayout> layout, const void* data, size_t data_size,
                DataRepresentation rep);
        /// For internal use only. @internal
        Decoder(const Decoder& decoder, const void* data, size_t data_size,
                DataRepresentation rep);
        /// For internal use only. @internal
        virtual const StructLayoutElement* getLayoutElement(size_t index) const;

    private:
        /// For internal use only. @internal
        a_util::result::Result calculateDynamicElements();
        /// For internal use only. @internal
        a_util::result::Result addDynamicElements(const DynamicStructLayoutElement& dynamic_element,
                                   Offsets& overall_offsets,
                                   const std::string& prefix);
        /// For internal use only. @internal
        a_util::result::Result addDynamicElement(const DynamicStructLayoutElement& dynamic_element,
                                  const std::string& array_index,
                                  Offsets& overall_offsets,
                                  const std::string& prefix);
        /// For internal use only. @internal
        a_util::result::Result addStaticStructElements(const DynamicStructLayoutElement& dynamic_element,
                                        const std::string& array_index,
                                        Offsets& overall_offsets,
                                        const std::string& prefix);
        /// For internal use only. @internal
        a_util::result::Result addDynamicStructElements(const DynamicStructLayoutElement& dynamic_element,
                                         const std::string& array_index,
                                         Offsets& overall_offsets,
                                         const std::string& prefix);
        /// For internal use only. @internal
        a_util::result::Result addDynamicStructElement(const StructLayoutElement& element);
        /// For internal use only. @internal
        void moveToAlignment(size_t& bit_offset, size_t alignment);

    protected:
        /// For internal use only. @internal
        a_util::memory::shared_ptr<std::vector<StructLayoutElement> > _dynamic_elements;
        /// For internal use only. @internal
        Offsets _buffer_sizes;
};

/**
 * Decoder for dynamic structures defined by a DDL definition.
 * Currently the amount of dynamic elements is determined during construction
 * only (by the current values in the structure).
 */
class Codec: public Decoder
{
    public:
        /**
         * Sets the current value of the given element by copying its data
         * from the passed-in location.
         * @param[in] index The index of the element.
         * @param[in] value The location where the data should be copied from.
         * @retval ERR_INVALID_INDEX Invalid element index.
         */
        a_util::result::Result setElementValue(size_t index, const void* value);

        /**
         * Sets the current value of the given element to the given value.
         * @param[in] index The index of the element.
         * @param[in] value The value.
         * @retval ERR_INVALID_INDEX Invalid element index.
         */
        a_util::result::Result setElementValue(size_t index, const a_util::variant::Variant& value);

        /**
         * @param[in] index The index of the element.
         * @return A pointer to the element or NULL in case of an error.
         */
        void* getElementAddress(size_t index);
        using StaticDecoder::getElementAddress;

        /**
         * Sets all elements to their constant values defined in the DDL.
         * @return Standard result.
         */
        a_util::result::Result setConstants();

    protected:
        friend class CodecFactory;
        friend class Decoder;
        /// For internal use only. @internal
        Codec(a_util::memory::shared_ptr<const StructLayout> layout, void* data, size_t data_size,
              DataRepresentation rep);
        /// For internal use only. @internal
        Codec(const Decoder& decoder, void* data, size_t data_size,
              DataRepresentation rep);
};


}

#endif
