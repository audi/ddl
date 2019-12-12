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

#include "ddlalignment.h"

namespace ddl
{
    std::string DDLAlignment::toString(AlignmentType const alignment)
    {
        if (alignment != e_invalid)
        {
            return a_util::strings::toString((int32_t)alignment);
        }

        return "";
    }

    DDLAlignment::AlignmentType DDLAlignment::fromString(const std::string &alignment)
    {
        if (!a_util::strings::isInt32(alignment))
        {
            // fallback to default
            return e_invalid;
        }
        int alignment_int = a_util::strings::toInt32(alignment);
        if (alignment_int <= 0)
        {
            return e1;
        }
        else
        {
            switch(alignment_int)
            {
            case 1:
                {
                    return e1;
                }
            case 2:
                {
                    return e2;
                }
            case 4:
                {
                    return e4;
                }
            case 8:
                {
                    return e8;
                }
            case 16:
                {
                    return e16;
                }
            case 32:
                {
                    return e32;
                }
            case 64:
                {
                    return e64;
                }
            default:
                {
                    break;
                }
            }
        }

        return e_invalid;
    }
}   // namespace ddl
