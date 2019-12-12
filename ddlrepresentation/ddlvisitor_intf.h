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

#ifndef DDL_VISITOR_H_INCLUDED
#define DDL_VISITOR_H_INCLUDED

#include "ddl_common.h"

namespace ddl
{
    class DDLHeader;
    class DDLDataType;
    class DDLComplex;
    class DDLStream;
    class DDLUnit;
    class DDLBaseunit;
    class DDLExtDeclaration;
    class DDLElement;
    class DDLDescription;
    class DDLPrefix;
    class DDLRefUnit;
    class DDLStreamStruct;
    class DDLEnum;
    class DDLStreamMetaType;
    class DDLProperty;

    /**
     * Abstract base class/interface for Visitor design-pattern.
     */
    class IDDLVisitor
    {
    public:
        /**
         * DTOR
         */
        virtual ~IDDLVisitor(){}

        /**
         * Visitor for a whole DDL description.
         * @param[in] description - Pointer to the description object
         * @retval ERR_POINTER Null-pointer committed
         * @retval ERR_NOT_FOUND Required node not found.
         * @retval ERR_NOT_INITIALIZED Not yet initialized
         */
        virtual a_util::result::Result visitDDL(const DDLDescription* description) = 0;
        /**
         * Visitor for DDL header objects.
         * @param[in] header - Pointer to the header object
         * @retval ERR_POINTER Null-pointer committed
         * @retval ERR_NOT_FOUND Required node not found.
         */
        virtual a_util::result::Result visit(const DDLHeader* header) = 0;
        /**
         * Visitor for DDL datatype objects.
         * @param[in] data_type - Pointer to the datatype object
         * @retval ERR_POINTER Null-pointer committed
         * @retval ERR_NOT_FOUND Required node not found.
         * @retval ERR_NOT_SUPPORTED Data type detected which is not supported.
         */
        virtual a_util::result::Result visit(const DDLDataType* data_type) = 0;
        /**
         * Visitor for DDL struct objects.
         * @param[in] ddl_struct - Pointer to the struct object
         * @retval ERR_POINTER Null-pointer committed
         * @retval ERR_NOT_FOUND Required node not found.
         */
        virtual a_util::result::Result visit(const DDLComplex* ddl_struct) = 0;
        /**
         * Visitor for DDL stream objects.
         * @param[in] stream - Pointer to the stream object
         * @retval ERR_POINTER Null-pointer committed
         * @retval ERR_NOT_FOUND Required node not found.
         */
        virtual a_util::result::Result visit(const DDLStream* stream) = 0;
        /**
         * Visitor for DDL unit objects.
         * @param[in] unit - Pointer to the unit object
         * @retval ERR_POINTER Null-pointer committed
         * @retval ERR_NOT_FOUND Required node not found.
         */
        virtual a_util::result::Result visit(const DDLUnit* unit) = 0;
        /**
         * Visitor for DDL baseunit objects.
         * @param[in] baseunit - Pointer to the baseunit object
         * @retval ERR_POINTER Null-pointer committed
         * @retval ERR_NOT_FOUND Required node not found.
         */
        virtual a_util::result::Result visit(const DDLBaseunit* baseunit) = 0;
        /**
         * Visitor for external DDL declaration objects.
         * @param[in] ext_declaration - Pointer to the extdeclaration object
         * @retval ERR_POINTER Null-pointer committed
         * @retval ERR_NOT_FOUND Required node not found.
         */
        virtual a_util::result::Result visit(const DDLExtDeclaration* ext_declaration) = 0;
        /**
         * Visitor for DDL element objects.
         * @param[in] element - Pointer to the element object
         * @retval ERR_POINTER Null-pointer committed
         * @retval ERR_NOT_FOUND Required node not found.
         */
        virtual a_util::result::Result visit(const DDLElement* element) = 0;
        /**
         * Visitor for DDL prefix objects.
         * @param[in] prefix - Pointer to the prefix object
         * @retval ERR_POINTER Null-pointer committed
         * @retval ERR_NOT_FOUND Required node not found.
         */
        virtual a_util::result::Result visit(const DDLPrefix* prefix) = 0;
        /**
         * Visitor for DDL reference unit objects.
         * @param[in] ref_unit - Pointer to the refunit object
         * @retval ERR_POINTER Null-pointer committed
         * @retval ERR_NOT_FOUND Required node not found.
         */
        virtual a_util::result::Result visit(const DDLRefUnit* ref_unit) = 0;
        /**
         * Visitor for DDL streamstruct objects.
         * @param[in] stream_struct - Pointer to the streamstruct object
         * @retval ERR_POINTER Null-pointer committed
         * @retval ERR_NOT_FOUND Required node not found.
         */
        virtual a_util::result::Result visit(const DDLStreamStruct* stream_struct) = 0;

        /**
         * Visitor for DDL enum objects.
         * @param[in] ddl_enum - Pointer to the enum object
         * @retval ERR_POINTER Null-pointer committed
         * @retval ERR_NOT_FOUND Required node not found.
         */
        virtual a_util::result::Result visit(const DDLEnum* ddl_enum) = 0;

        /**
         * Visitor for DDL stream meta type objects.
         * @param[in] stream_meta_type - Pointer to the object
         * @retval ERR_POINTER Null-pointer committed
         * @retval ERR_NOT_FOUND Required node not found.
         */
        virtual a_util::result::Result visit(const DDLStreamMetaType* stream_meta_type) = 0;

        /**
         * Visitor for DDL property objects.
         * @param[in] ddl_property - Pointer to the object
         * @retval ERR_POINTER Null-pointer committed
         * @retval ERR_NOT_FOUND Required node not found.
         */
        virtual a_util::result::Result visit(const DDLProperty* ddl_property) = 0;
    };

    /**
    * Abstract base class/interface for Visitor design-pattern.
    */
    class IDDLChangeVisitor
    {
    public:
        /**
        * DTOR
        */
        virtual ~IDDLChangeVisitor() {}

        /**
        * Visitor for a whole DDL description.
        * @param[in] description - Pointer to the description object
        * @retval ERR_POINTER Null-pointer committed
        * @retval ERR_NOT_FOUND Required node not found.
        * @retval ERR_NOT_INITIALIZED Not yet initialized
        */
        virtual a_util::result::Result visitDDL(DDLDescription* description) = 0;
        /**
        * Visitor for DDL header objects.
        * @param[in] header - Pointer to the header object
        * @retval ERR_POINTER Null-pointer committed
        * @retval ERR_NOT_FOUND Required node not found.
        */
        virtual a_util::result::Result visit(DDLHeader* header) = 0;
        /**
        * Visitor for DDL datatype objects.
        * @param[in] data_type - Pointer to the datatype object
        * @retval ERR_POINTER Null-pointer committed
        * @retval ERR_NOT_FOUND Required node not found.
        * @retval ERR_NOT_SUPPORTED Data type detected which is not supported.
        */
        virtual a_util::result::Result visit(DDLDataType* data_type) = 0;
        /**
        * Visitor for DDL struct objects.
        * @param[in] ddl_struct - Pointer to the struct object
        * @retval ERR_POINTER Null-pointer committed
        * @retval ERR_NOT_FOUND Required node not found.
        */
        virtual a_util::result::Result visit(DDLComplex* ddl_struct) = 0;
        /**
        * Visitor for DDL stream objects.
        * @param[in] stream - Pointer to the stream object
        * @retval ERR_POINTER Null-pointer committed
        * @retval ERR_NOT_FOUND Required node not found.
        */
        virtual a_util::result::Result visit(DDLStream* stream) = 0;
        /**
        * Visitor for DDL unit objects.
        * @param[in] unit - Pointer to the unit object
        * @retval ERR_POINTER Null-pointer committed
        * @retval ERR_NOT_FOUND Required node not found.
        */
        virtual a_util::result::Result visit(DDLUnit* unit) = 0;
        /**
        * Visitor for DDL baseunit objects.
        * @param[in] baseunit - Pointer to the baseunit object
        * @retval ERR_POINTER Null-pointer committed
        * @retval ERR_NOT_FOUND Required node not found.
        */
        virtual a_util::result::Result visit(DDLBaseunit* baseunit) = 0;
        /**
        * Visitor for external DDL declaration objects.
        * @param[in] ext_declaration - Pointer to the extdeclaration object
        * @retval ERR_POINTER Null-pointer committed
        * @retval ERR_NOT_FOUND Required node not found.
        */
        virtual a_util::result::Result visit(DDLExtDeclaration* ext_declaration) = 0;
        /**
        * Visitor for DDL element objects.
        * @param[in] element - Pointer to the element object
        * @retval ERR_POINTER Null-pointer committed
        * @retval ERR_NOT_FOUND Required node not found.
        */
        virtual a_util::result::Result visit(DDLElement* element) = 0;
        /**
        * Visitor for DDL prefix objects.
        * @param[in] prefix - Pointer to the prefix object
        * @retval ERR_POINTER Null-pointer committed
        * @retval ERR_NOT_FOUND Required node not found.
        */
        virtual a_util::result::Result visit(DDLPrefix* prefix) = 0;
        /**
        * Visitor for DDL reference unit objects.
        * @param[in] ref_unit - Pointer to the refunit object
        * @retval ERR_POINTER Null-pointer committed
        * @retval ERR_NOT_FOUND Required node not found.
        */
        virtual a_util::result::Result visit(DDLRefUnit* ref_unit) = 0;
        /**
        * Visitor for DDL streamstruct objects.
        * @param[in] stream_struct - Pointer to the streamstruct object
        * @retval ERR_POINTER Null-pointer committed
        * @retval ERR_NOT_FOUND Required node not found.
        */
        virtual a_util::result::Result visit(DDLStreamStruct* stream_struct) = 0;

        /**
        * Visitor for DDL enum objects.
        * @param[in] ddl_enum - Pointer to the enum object
        * @retval ERR_POINTER Null-pointer committed
        * @retval ERR_NOT_FOUND Required node not found.
        */
        virtual a_util::result::Result visit(DDLEnum* ddl_enum) = 0;

        /**
        * Visitor for DDL stream meta type objects.
        * @param[in] stream_meta_type - Pointer to the object
        * @retval ERR_POINTER Null-pointer committed
        * @retval ERR_NOT_FOUND Required node not found.
        */
        virtual a_util::result::Result visit(DDLStreamMetaType* stream_meta_type) = 0;

        /**
        * Visitor for DDL property objects.
        * @param[in] ddl_property - Pointer to the object
        * @retval ERR_POINTER Null-pointer committed
        * @retval ERR_NOT_FOUND Required node not found.
        */
        virtual a_util::result::Result visit(DDLProperty* ddl_property) = 0;
    };

}   // namespace ddl

#endif  // DDL_VISITOR_H_INCLUDED
