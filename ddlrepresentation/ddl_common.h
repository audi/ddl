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

#ifndef DDL_COMMON_HEADER
#define DDL_COMMON_HEADER
    

 // Disables VS warnings about unsafe C functions
#ifdef WIN32
#define _SCL_SECURE_NO_WARNINGS // NOLINT
#endif

 // Determine whether move semantics are available
#ifdef _MSC_VER
 // On Windows, make the decision using the VS version
#if _MSC_VER >= 1800 // VS 2013 (needs default-generation)
#define DDL_HAS_MOVE 1
#else
#define DDL_HAS_MOVE 0
#endif
#else
 // Otherwise use the standard version macro
#if __cplusplus >= 201103L
#define DDL_HAS_MOVE 1
#else
#define DDL_HAS_MOVE 0
#endif
#endif

 // Include utils
#include <a_util/base.h>
#include <a_util/filesystem.h>
#include <a_util/xml.h>
#include <a_util/memory.h>
#include <a_util/regex.h>
#include <a_util/strings.h>
#include <a_util/variant.h>
#include <a_util/datetime.h>
#include <a_util/system.h>
#include <a_util/concurrency.h>
#include <a_util/logging.h>
#include <a_util/result.h>

 // Include standard headers
#include <stack>
#include <set>
#include <map>
#include <vector>
#include <cassert>
#include <algorithm>


#endif // DDL_COMMON_HEADER

