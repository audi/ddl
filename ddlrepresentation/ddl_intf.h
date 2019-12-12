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

#ifndef DDL_INTF_H_INCLUDED
#define DDL_INTF_H_INCLUDED

#include "ddl_common.h"

namespace ddl
{

    class IDDLVisitor;
    class IDDLChangeVisitor;

    /**
     * Interface class for object representation of DDL descriptions.
     * E.g. it provides the accept() method for the Visitor design-pattern.
     */
    class IDDL
    {
    public:
        /**
         * Virtual DTOR
         */
        virtual ~IDDL(){}

        /**
         * Acceptance method for Visitor design-pattern.
         * @param[in] visitor - Pointer to Visitor instance
         * @retval ERR_POINTER Null-pointer committed
         * @retval ERR_NOT_FOUND Required node not found.
         * @retval ERR_NOT_INITIALIZED The object was not or not correctly
         * initialized
         */
        virtual a_util::result::Result accept (IDDLVisitor * visitor) const = 0;

        /**
         * Acceptance method for Visitor and Change design-pattern.
         * @param[in] visitor - Pointer to Visitor instance
         * @retval ERR_POINTER Null-pointer committed
         * @retval ERR_NOT_FOUND Required node not found.
         * @retval ERR_NOT_INITIALIZED The object was not or not correctly
         * initialized
         */
        virtual a_util::result::Result accept(IDDLChangeVisitor * visitor) = 0;

        /**
         * Getter for the initialization flag.
         * @retval true The object was initialized correctly
         * @retval false The object was not or not correctly initialized
         */
        virtual bool isInitialized() const = 0;

        /**
         * Getter for the name of the representation object.
         * @return the name
         */
        virtual const std::string& getName() const = 0;

        /**
         * Getter for the predefinition flag.
         * @retval true The object was predefined
         * @retval false The object was defined later
         */
        virtual bool isPredefined() const = 0;

        /**
         * Getter for the predefinition flag.
         * @retval true The object was predefined
         * @retval false The object was defined later
         */
        virtual bool isOverwriteable() const = 0;

        /**
         * Getter for the creation level.
         * @return the level at creation time of this representation object
         */
        virtual int getCreationLevel() const = 0;

        /** 
         * Getter for complex data type flag.
         * @retval true This is a complex data type
         */
        virtual bool isComplex() const
        {
            // default is not complex
            return false;
        }

        /**
         * Flags for Merging
         *
         */
        enum TagMergeFlags
        {
            ddl_merge_force_overwrite = 0x01
        };
    };

}   // namespace ddl

#endif  // DDL_INTF_H_INCLUDED
