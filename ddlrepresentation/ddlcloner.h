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

#ifndef DDL_CLONER_H_INCLUDED
#define DDL_CLONER_H_INCLUDED

#include "ddl_common.h"
#include "ddlfactorymethod_intf.h"
#include "ddldescription.h"

namespace ddl
{
    /**
     * Implementation of IDDLFactorMethod for cloning of DDL representations.
     */
    class DDLCloner : public IDDLFactoryMethod
    {
    public: // implements IDDLFactoryMethod

        DDLDescription * getDDL() const;

        a_util::result::Result createNew(const DDLVersion& version = DDLVersion::ddl_version_invalid);

        void destroyDDL();

        a_util::result::Result buildHeader();

        a_util::result::Result buildUnits();

        a_util::result::Result buildDatatypes();

        a_util::result::Result buildEnums();

        a_util::result::Result buildStructs();

        a_util::result::Result buildStreams();

        a_util::result::Result buildStreamMetaTypes();
    public: 
        /**
         * Setter method for the original DDL object.
         * @param[in] original - Pointer to the DDL object
         * @retval ERR_POINTER Null-pointer committed
         */
        a_util::result::Result setOriginal(const DDLDescription* original);

    private:    // members
        DDLDescription* _ddl_desc;
        const DDLDescription* _original_desc;
    };
}   // namespace ddl

#endif  // DDL_CLONER_H_INCLUDED
