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

#include "ddlbyteorder.h"

namespace ddl
{
    std::string DDLByteorder::toString(DDLByteorder::Byteorder const byteorder)
    {
        switch(byteorder)
        {
        case e_le:
            {
                return std::string("LE");
            }
        case e_be:
            {
                return std::string("BE");
            }
        default:
            {
                // error case
                return a_util::strings::empty_string;
            }
        }
    }

    DDLByteorder::Byteorder DDLByteorder::fromString(const std::string& byteorder)
    {
        if (byteorder == "LE")
        {
            return e_le;
        }
        if (byteorder == "BE")
        {
            return e_be;
        }
        if (byteorder == "Motorola")
        {
            return e_be;
        }
        if (byteorder == "Intel")
        {
            return e_le;
        }
        // fallback to default
        return e_le;
    }
}   // namespace ddl
