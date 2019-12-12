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


#ifndef HEADER_DDL_HELPER_H
#define HEADER_DDL_HELPER_H

#include "a_util/result.h"

#include "ddlrepresentation/ddlcomplex.h"
#include "ddlrepresentation/ddlelement.h"

namespace mapping
{

#if defined(__GNUC__) && (__GNUC__ == 5) && defined(__QNX__)
#pragma GCC diagnostic ignored "-Wattributes" // standard type attributes are ignored when used in templates
#endif

/// String -> Datatype Enum Map
typedef std::map<std::string, uint8_t> TypeMap;

#if defined(__GNUC__) && (__GNUC__ == 5) && defined(__QNX__)
#pragma GCC diagnostic warning "-Wattributes" // standard type attributes are ignored when used in templates
#endif

/// Enum for all relevant data types
enum DataTypes
{
    e_uint8 = 1,
    e_uint16,
    e_uint32,
    e_uint64,
    e_int8,
    e_int16,
    e_int32,
    e_int64,
    e_float32,
    e_float64,
    e_bool,
    e_char,
};

/**
 * DDLHelper provides common DDL helper functionality needed
 * by both the mapping configuration and runtime
 */
class DDLHelper
{
public:
    /**
    * \c LookupElement finds a DDLElement located inside a DDLComplex
    * by means of a path string. If the path represents an array element,
    * the array itself is returned.
    *
    * @param [in] ddl_struct The base structure of the path
    * @param [in] path The path to be looked up
    * @param [out] element The destination target parameter
    * @param [out] is_array_element Flag indicating whether the path referenced an array element
    *
    * @retval ERR_NOT_FOUND The element was not found or the path is invalid
    * @retval a_util::result::SUCCESS The path was found and oElement contains the element
    */
    static a_util::result::Result LookupElement(const ddl::DDLComplex& ddl_struct,
        const std::string& path, const ddl::DDLElement*& element, bool& is_array_element);
};

} // namespace mapping

#endif // HEADER_DDL_HELPER_H
