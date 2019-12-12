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

#include "ddl_error.h"

namespace ddl
{
    static DDLError s_last_error;

    a_util::result::Result DDLError::setLastDDLError(a_util::result::Result result,
                                                      const std::string& error)
    {
        s_last_error._last_error_code = result;
        if (!error.empty())
        {
            s_last_error._last_error_description = error;
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLError::getLastDDLErrorCode()
    {
        return s_last_error._last_error_code;
    }

    const char* DDLError::getLastDDLErrorDescription()
    {
        return s_last_error._last_error_description.c_str();
    }


}   // namespace ddl


