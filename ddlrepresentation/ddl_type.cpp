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

#include "ddl_type.h"

namespace ddl
{

    bool DDL::isEqual(IDDL *lhs, IDDL *rhs)
    {
        if (NULL == lhs ||
            NULL == rhs)
        {
            return false;
        }
        return a_util::strings::isEqualNoCase(lhs->getName().c_str(), rhs->getName().c_str());
    }

    bool DDL::isSorted(IDDL *lhs, IDDL *rhs)
    {
        if (NULL == lhs)
        {
            return false;
        }
        if (NULL == rhs)
        {
            return true;
        }
        return lhs->getName() < rhs->getName();
    }

    bool DDL::isInitialized() const
    {
        // default case
        return true;
    }

    bool DDL::isPredefined() const
    {
        // default case
        return false;
    }

    bool DDL::isOverwriteable() const
    {
        return getCreationLevel() > 0;   // cMediaManager::DL_AlwaysThere
    }

    int DDL::getCreationLevel() const
    {
        // default case
        // cMediaManager::DL_AlwaysThere
        return 0;
    }

}   // namespace ddl
