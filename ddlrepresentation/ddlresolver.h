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

#ifndef DDL_RESOLVER_H_INCLUDED
#define DDL_RESOLVER_H_INCLUDED

#include "ddl_common.h"
#include "ddlvisitor_intf.h"
#include "ddlversion.h"

namespace ddl
{
    /**
     * Implementation of IDDLVisitor for resolving of a specified unit,
     * datatype, struct, or stream out of an existing DDL hierarchy.
     * So a minimal DDL description for the specified target is built.
     */
    class DDLResolver : public IDDLVisitor
    {
    public:
        /**
         * Default CTOR
         */
        DDLResolver();

        /**
         * CTOR with target name.
         * @param[in] target - Name of the target to resolve
         */
        DDLResolver(const std::string& target);

        a_util::result::Result visitDDL(const DDLDescription* description);
        a_util::result::Result visit(const DDLHeader* header);
        a_util::result::Result visit(const DDLDataType* data_type);
        a_util::result::Result visit(const DDLComplex* complex);
        a_util::result::Result visit(const DDLStream* stream);
        a_util::result::Result visit(const DDLUnit* unit);
        a_util::result::Result visit(const DDLBaseunit* baseunit);
        a_util::result::Result visit(const DDLExtDeclaration* ext_declaration);
        a_util::result::Result visit(const DDLElement* element);
        a_util::result::Result visit(const DDLPrefix* prefix);
        a_util::result::Result visit(const DDLRefUnit* ref_unit);
        a_util::result::Result visit(const DDLStreamStruct* stream_struct);
        a_util::result::Result visit(const DDLEnum* ddl_enum);
        a_util::result::Result visit(const DDLStreamMetaType* stream_meta_type);
        a_util::result::Result visit(const DDLProperty* property);

        /**
         * Getter for a minimalistic DDL which only resolves the specified DDL object.
         * @return the XML string containing the minimalistic DDL description
         */
        std::string getResolvedXML() const;

        /**
         * Setter for the name of the target to resolve.
         * @param[in] name - Name of the DDL object to resolve
         * @remarks Only targets of the types "unit", "datatype", "struct", and
         * "stream" are supported.
         * @return void
         */
        void setTargetName(const std::string& name);

    protected:
        /**
         * Create the base items
         * @param [in] description    pointer to the DDL Description   
         * 
         * @return Standard result code.
         */
        a_util::result::Result createBase(const DDLDescription* description);

    protected:    // members
        a_util::xml::DOM                 _dom;                 ///< Internal DOM for XML handling
        std::string                      _target;            ///< Name of the resolving target
        DDLVersion                       _version;             ///< DDL version (needed for some conversions for compatibility with DDL1.0)
        std::stack<a_util::xml::DOMElement>  _stack_parents;    ///< stack of parents
        std::set<std::string>           _resolved_types;     ///< set of the resolved types
        a_util::xml::DOMElement         _units;               ///< DOM-Element of type Unit
        a_util::xml::DOMElement         _data_types;           ///< DOM-Element of type DataType
        a_util::xml::DOMElement         _enums;               ///< DOM-Element of type Enum
        a_util::xml::DOMElement         _structs;             ///< DOM-Element of type Struct
        a_util::xml::DOMElement         _streams;             ///< DOM-Element of type Stream
    };

}   // namespace ddl

#endif  // DDL_RESOLVER_H_INCLUDED
