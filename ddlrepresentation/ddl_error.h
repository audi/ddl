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

#ifndef DDLERROR_H_INCLUDED
#define DDLERROR_H_INCLUDED

#include "ddl_common.h"

namespace ddl
{
    /**
     * Static DDL Error Handling class.
     * 
     * The parser will use this static Error handling to inform about validation errors.
     */
    class DDLError
    {
            std::string            _last_error_description;
            a_util::result::Result _last_error_code;

    public:
            /** 
             * Gets the last DDL Error Code occured.
             * @return the DDL Error.
             */
            static a_util::result::Result getLastDDLErrorCode();
            /**
             * Gets the last DDL Error description occured.
             * @return the last DDL Error description.
             * @remark not thread safe!
             */
            static const char* getLastDDLErrorDescription();
            /**
             * Sets the last DDL Error and error description.
             * @return the last DDL Error description.
             * @remark not thread safe!
             */
            static a_util::result::Result setLastDDLError(a_util::result::Result result, 
                                                          const std::string& error); 
    };

/**
 * Helper to set an error description to the DDL error handling and return a DDL Error.
 * @param [in] \_\_errCode\_\_ the error code to return 
 * @param [in] \_\_desc\_\_    the description to set
 */
#define RETURN_DDLERROR_DESC(__errCode__, __desc__) { ddl::DDLError::setLastDDLError(__errCode__, __desc__); return __errCode__; }
/**
 * Helper to set an error description to the DDL error handling and return a DDL Error if failed.
 * @param [in] \_\_callterm\_\_ statement to execute to. 
 */
#define RETURN_DDLERROR_IF_FAILED(__callterm__) { a_util::result::Result ntmpRes = __callterm__; if (isFailed(ntmpRes)) { RETURN_DDLERROR_DESC(ntmpRes, ""); } } 
/**
 * Helper to set an error description to the DDL error handling and return a DDL Error if failed.
 * @param [in] \_\_callterm\_\_ statement to execute to.
 * @param [in] \_\_desc\_\_ additional error description to set.
 */
#define RETURN_DDLERROR_IF_FAILED_DESC(__callterm__, __desc__) { a_util::result::Result ntmpRes = __callterm__; if (isFailed(ntmpRes)) { RETURN_DDLERROR_DESC(ntmpRes, __desc__); } }
}   // namespace ddl

#endif  // DDLERROR_H_INCLUDED
