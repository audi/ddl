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

#ifndef DDL_SERIALIZER_CLASS_HEADER
#define DDL_SERIALIZER_CLASS_HEADER

#include <assert.h>

#include "a_util/result.h"

#include "codec/codec.h"

namespace ddl
{

namespace serialization
{

/**
 * Copies all elements from a decoder to a codec
 * @param[in] decoder The source decoder.
 * @param[out] encoder The destination codec.
 * @return Standard result.
 */
template <typename DECODER, typename ENCODER>
a_util::result::Result transform(const DECODER& decoder, ENCODER& encoder)
{
    size_t element_count = decoder.getElementCount();
    assert(element_count == encoder.getElementCount());
    for (size_t element = 0; element < element_count; ++element)
    {
        uint64_t buffer = 0;
        a_util::result::Result result = decoder.getElementValue(element, &buffer);
        if (a_util::result::isFailed(result))
        {
            return result;
        }

        result = encoder.setElementValue(element, &buffer);
        if (a_util::result::isFailed(result))
        {
            return result;
        }
    }

    return a_util::result::SUCCESS;
}

/**
 * Tranforms the data from a given decoder into the opposite data representation.
 * Allocates the buffer accordingly.
 * @param[in] decoder The source decoder.
 * @param[out] buffer The destination buffer object.
 * @param[in] zero Whether or not to memzero the buffer before writing the elements to it.
 * @return Standard result.
 */
a_util::result::Result transform_to_buffer(const Decoder& decoder, a_util::memory::MemoryBuffer& buffer, bool zero = false);

}

}

#endif
