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

#ifndef BYTEORDER_H_INCLUDED
#define BYTEORDER_H_INCLUDED

#include "ddl_common.h"
#include "ddl_type.h"

namespace ddl
{
    /**
     * Abstract wrapper class for the byteorder enumeration.
     */
    class DDLByteorder : public DDL
    {
    public:
    /**
     * Representation of the byteorder enumeration
     */
    typedef enum
    {
        platform_not_supported   = 0x00,
        plattform_little_endian_8 = 0x01,
        platform_big_endian_8    = 0x02,
        e_noe = platform_not_supported,
        e_le  = plattform_little_endian_8,
        e_be  = platform_big_endian_8
    } Byteorder;

    /**
     * Converts the given byteorder enumeration to a string.
     * @param[in] byteorder - Byteorder value to convert
     * @return the byteorder as string
     */
    static std::string toString(Byteorder const byteorder);

    /**
     * Extracts the byteorder out of the given string.
     * @param[in] byteorder - String containing the byteorder
     * @return the extracted byteorder
     */
    static Byteorder fromString(const std::string& byteorder);
    };

} // namespace ddl

#endif  // BYTEORDER_H_INCLUDED
