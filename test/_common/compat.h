/**
 * @file
 * Compat header containing structs and defines needed for the ddlcoder (mediacoder) test
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

#ifndef DDL_TEST_COMMON_COMPAT_HEADER_INCLUDED_
#define DDL_TEST_COMMON_COMPAT_HEADER_INCLUDED_

#ifndef TEST_REQ
/**
 * Porting AEV specific macro - only necessary for internal use
 * @param[in] req_id Unused
 */
#define TEST_REQ(req_id)
#endif // TEST_REQ

#endif // DDL_TEST_COMMON_COMPAT_HEADER_INCLUDED_
