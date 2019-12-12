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

#ifndef DDL_CODEC_FACTORY_CLASS_HEADER
#define DDL_CODEC_FACTORY_CLASS_HEADER

#include "codec.h"

#include "ddlrepresentation/ddlcomplex.h"
#include "struct_element.h"
#include "static_codec.h"



namespace ddl
{

/**
 * Factory class for ddl codecs.
 */
class CodecFactory
{
    public:
        /**
         * Empty constructor. This exists to enable uninitialized member variables of this type
         * that are move-assigned later on.
         */
        CodecFactory();

        /**
         * Constructor that take a DDL string for initialization.
         * @param[in] struct_name The name of the struct for which codecs should be generated.
         * @param[in] media_description The DDL description.
         */
        CodecFactory(const char* struct_name,
                      const char* media_description);

        /**
         * Constructor that uses an OO-DDL struct for initialization.
         * @param[in] ddl_struct The struct definition.
         */
        CodecFactory(const DDLComplex* ddl_struct);

        /**
         * Check if the factory is in a valid state.
         * @return Any errors during construction.
         */
        a_util::result::Result isValid() const;

        /**
         * Creates a static decoder for the given data.
         * @param[in] data The pointer to the raw data.
         * @param[in] data_size The size of the raw data.
         * @param[in] rep The representation that the data is encoded in.
         * @return a static decoder.
         */
        inline StaticDecoder makeStaticDecoderFor(const void* data, size_t data_size,
                                                   DataRepresentation rep = deserialized) const
        {
            return StaticDecoder(_layout, data, data_size, rep);
        }

        /**
         * Creates a static codec for the given data.
         * @param[in] data The pointer to the raw data.
         * @param[in] data_size The size of the raw data.
         * @param[in] rep The representation that the data is encoded in.
         * @return a static codec.
         */
        inline StaticCodec makeStaticCodecFor(void* data, size_t data_size,
                                               DataRepresentation rep = deserialized) const
        {
            return StaticCodec(_layout, data, data_size, rep);
        }

        /**
         * Creates a decoder for the given data.
         * @param[in] data The pointer to the raw data.
         * @param[in] data_size The size of the raw data.
         * @param[in] rep The representation that the data is encoded in.
         * @return a decoder.
         */
        inline Decoder makeDecoderFor(const void* data, size_t data_size,
                                       DataRepresentation rep = deserialized) const
        {
            return Decoder(_layout, data, data_size, rep);
        }

        /**
         * Creates a codec for the given data.
         * @param[in] data The pointer to the raw data.
         * @param[in] data_size The size of the raw data.
         * @param[in] rep The representation that the data is encoded in.
         * @return a codec.
         */
        inline Codec makeCodecFor(void* data, size_t data_size,
                                   DataRepresentation rep = deserialized) const
        {
            return Codec(_layout, data, data_size, rep);
        }

        /**
         * @return The amount of static elements contained in the handled structure.
         */
        size_t getStaticElementCount() const;

        /**
         * Access information about an element.
         * @param[in] index The index of the element.
         * @param[out] element Pointer that will be updated to point to the element information.
         * @retval ERR_INVALID_INDEX Invalid index.
         */
        a_util::result::Result getStaticElement(size_t index, const StructElement*& element) const;

        /**
         * @param[in] rep The data representation for which the buffer size should be returned.
         * @return The size of the structure in the requested data representation.
         */
        size_t getStaticBufferSize(DataRepresentation rep = deserialized) const;

    private:        
        /// For internal use only.  @internal The struct layout.
        a_util::memory::shared_ptr<const StructLayout> _layout;
        /// For internal use only. @internal The constructor result.
        a_util::result::Result _constructor_result;
};

}

#endif
