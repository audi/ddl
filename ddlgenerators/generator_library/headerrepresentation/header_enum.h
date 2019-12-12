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
 *
 * QNX support Copyright (c) 2019 by dSPACE GmbH, Paderborn, Germany. All Rights Reserved
 */

#ifndef HEADER_ENUM_H_INCLUDED
#define HEADER_ENUM_H_INCLUDED

#include "header_base_intf.h"
#include "header_type.h"
#include <map>
#include <a_util/result.h>

namespace ddl
{

#if defined(__GNUC__) && (__GNUC__ == 5) && defined(__QNX__)
#pragma GCC diagnostic ignored "-Wattributes" // standard type attributes are ignored when used in templates
#endif

    typedef std::map<int32_t, std::string> HeaderEnumValueMap;

#if defined(__GNUC__) && (__GNUC__ == 5) && defined(__QNX__)
#pragma GCC diagnostic warning "-Wattributes" // standard type attributes are ignored when used in templates
#endif

    /**
     * Representation of a enum in a header.
     * The class calculates its own packing from its children
     * every time one or more children have been added or removed.
     * To override the calculated packing, set the packing value AFTER adding
     * children.
     */
    class HeaderEnum : public HeaderType
    {
    public:
        /**
         * Default CTOR
         */
        HeaderEnum();

        /**
         * CTOR
         * @param [in] name            The name of the struct.
         * @param [in] packing          The packing of the struct.
         *                                The struct takes ownership of the passed elements.
         */
        HeaderEnum(std::string name,
                      size_t packing = 4);

        /**
         * Copy CTOR
         * @param [in] other  The other type this instance will be a copy of.
         */
        HeaderEnum(HeaderEnum& other);

        /**
         * DTOR
         */
        virtual ~HeaderEnum();

        /**
         * This method gives access to the values of the enum.
         * @return The enum values as a map.
         */
        const HeaderEnumValueMap& getValues() const;

        /** 
         * Add a value to this enum
         * @param[in] value The enum value (must be unique for this enum)
         * @param[in] name the name
         */
        a_util::result::Result addValue(int32_t value, const std::string& name);


        /**
         * Remove a value from this enum.
         * @param [in] key  The key of the element to be removed.
         * @return Standard result code.
         * @retval ERR_NOERROR  Everything is ok.
         * @retval ERR_NOT_FOUND  No element with this key was found.
         */
        a_util::result::Result removeValue(int32_t value);

    public: // implements cHeaderBaseType

        a_util::result::Result accept(IHeaderVisitor *visitor) const;

    private:

        HeaderEnumValueMap _enum_values;
    };
}   // namespace ddl

#endif  // HEADER_ENUM_H_INCLUDED
