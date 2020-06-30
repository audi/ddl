/**
 * @file
 * Implementation of the ADTF default media description.
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

#include "serialization.h"
#include "a_util/result/error_def.h"

namespace ddl
{

namespace serialization
{
//define all needed error types and values locally
_MAKE_RESULT(-12, ERR_MEMORY);

a_util::result::Result transform_to_buffer(const Decoder& decoder, a_util::memory::MemoryBuffer& buffer, bool zero)
{
    DataRepresentation target_rep = decoder.getRepresentation() == deserialized ?
                                         serialized :
                                         deserialized;
    size_t needed_size = decoder.getBufferSize(target_rep);
    if (buffer.getSize() < needed_size)
    {
        if (!buffer.allocate(needed_size))
        {
            return ERR_MEMORY;
        }
    }

    if (zero)
    {
        a_util::memory::set(buffer.getPtr(), buffer.getSize(), 0, buffer.getSize());
    }
    Codec codec = decoder.makeCodecFor(buffer.getPtr(), buffer.getSize(), target_rep);
    return transform(decoder, codec);
}

}

}
