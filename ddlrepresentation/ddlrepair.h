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

#ifndef DDL_REPAIR_H_INCLUDED
#define DDL_REPAIR_H_INCLUDED

#include "ddl_common.h"
#include "ddlvisitor_intf.h"

namespace ddl
{
    /**
     * Implementation of IDDLVisitor that will repair and update references after deleting.
     * I.e.: Deleting a Unit the DDLRepair will search for this unit within the elements of structs and resets the unit if set.
     */
    class DDLRepair : public IDDLChangeVisitor
    {
    public:
        a_util::result::Result visitDDL(DDLDescription* description);
        a_util::result::Result visit(DDLHeader* header);
        a_util::result::Result visit(DDLDataType* data_type);
        a_util::result::Result visit(DDLComplex* complex);
        a_util::result::Result visit(DDLStream* stream);
        a_util::result::Result visit(DDLUnit* unit);
        a_util::result::Result visit(DDLBaseunit* baseunit);
        a_util::result::Result visit(DDLExtDeclaration* ext_declaration);
        a_util::result::Result visit(DDLElement* element);
        a_util::result::Result visit(DDLPrefix* prefix);
        a_util::result::Result visit(DDLRefUnit* ref_unit);
        a_util::result::Result visit(DDLStreamStruct* stream_struct);
        a_util::result::Result visit(DDLEnum* ddl_enum);
        a_util::result::Result visit(DDLStreamMetaType* stream_meta_type);
        a_util::result::Result visit(DDLProperty* property);

    private:
        DDLDescription* _ref_ddl;
    };

}   // namespace ddl

#endif  // DDL_REPAIR_H_INCLUDED
