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

#ifndef DDLALIGNMENT_H_INCLUDED
#define DDLALIGNMENT_H_INCLUDED

#include "ddl_common.h"



namespace ddl
{
    /**
     * Abstract wrapper class for the alignment enumeration
     */
    class DDLAlignment
    {
    public:
    /**
     * Representation of the alignment enumeration
     */
    typedef enum
    {
        e0 = 1, /**< for backward compatibility */
        e1 = 1, /**< Default alignment */
        e2 = 2,
        e4 = 4,
        e8 = 8,
        e16 = 16,
        e32 = 32,
        e64 = 64,
        e_invalid = 255
    } AlignmentType;

    /**
     * Converts the given alignment to a string.
     * @param[in] alignment - The alignment to convert
     * @return the alignment as string
     */
    static std::string toString(AlignmentType const alignment);

    /**
     * Extracts the alignment out of a given string.
     * @param[in] alignment - String containing the alignment
     * @return the extracted alignment
     */
    static AlignmentType fromString(const std::string& alignment);

    };

}   // namespace ddl

#endif  // DDLALIGNMENT_H_INCLUDED
