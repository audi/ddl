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

#ifndef DDLDATATYPE_INTF_H_INCLUDED
#define DDLDATATYPE_INTF_H_INCLUDED

#include "ddl_common.h"
#include "ddl_intf.h"

namespace ddl
{
    class DDLAlignment;

    /**
     * Common interface for DDL datatypes.
     */
    class IDDLDataType : public IDDL
    {
    public:

        /**
         * Virtual DTOR
         */
        virtual ~IDDLDataType(){}
    };

}   // namespace ddl

#endif  // _DATATYPE_INTF_H_INCLUDED_
