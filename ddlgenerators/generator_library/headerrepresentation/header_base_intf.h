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
#ifndef HEADER_INTF_H_INCLUDED
#define HEADER_INTF_H_INCLUDED

namespace ddl
{

    class IHeaderVisitor;
    
    /**
     * Basic interface class for object representation of a C header.
     * E.g. it provides the accept() method for the Visitor design-pattern.
     */
    class IHeaderBase
    {
    public:
        /**
         * Acceptance method for Visitor design-pattern.
         * @param[in] visitor - Pointer to Visitor instance
         * @retval ERR_POINTER Null-pointer committed
         * @retval ERR_NOT_FOUND Required node not found.
         * @retval ERR_NOT_INITIALISED The object was not or not correctly
         * initialized
         */
        virtual a_util::result::Result accept (IHeaderVisitor * visitor) const = 0;
    };

}   // namespace ddl

#endif  // HEADER_INTF_H_INCLUDED
