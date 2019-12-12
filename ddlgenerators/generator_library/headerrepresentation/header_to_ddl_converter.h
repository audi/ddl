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

#ifndef HEADER_TO_DDL_CONVERTER_H_INCLUDED
#define HEADER_TO_DDL_CONVERTER_H_INCLUDED

#include "header_basic_type.h"
#include "header_typedef.h"
#include "header_constant.h"
#include "header_struct.h"
#include "header_enum.h"
#include "header_visitor_intf.h"

namespace ddl
{
/**
 * A Set of basic types
 */
typedef std::set<const HeaderBasicType*> HeaderConstBasicTypes;

/**
    * Container type of basic type objects
    */
typedef std::vector<const HeaderTypedef*> HeaderConstTypedefs;

/**
    * Container type of constants objects
    */
typedef std::vector<const HeaderConstant*> HeaderConstConstants;

/**
    * Container type of struct objects
    */
typedef std::vector<const HeaderStruct*> HeaderConstStructs;

/**
 * Container type of enum objects
 */
typedef std::vector<const HeaderEnum*> HeaderConstEnums;


/**
 * This class generates a DDL from a header.
 * Use this class by first visiting the element(s) of the header you want to be part of your DDL,
 * then call createNew() to create the DDL from the previously parsed header elements.
 * The generator tries to extract all needed datatypes from the header and stores them in a local
 * header as a copy (not the basic types). This copy then will then be converted to DDL. 
 */
class HeaderToDDLConverter : public IHeaderVisitor, public IDDLFactoryMethod
{
public:
    
    /**
     * CTOR
     */
    HeaderToDDLConverter();

    /**
     * DTOR
     */
    virtual ~HeaderToDDLConverter();

public: // implements IHeaderVisitor

    virtual a_util::result::Result visit(const Header* header);

    virtual a_util::result::Result visit(const HeaderBasicType* basic_type);

    virtual a_util::result::Result visit(const HeaderTypedef* type_def);

    virtual a_util::result::Result visit(const HeaderConstant* constant);

    virtual a_util::result::Result visit(const HeaderStruct* header_struct);

    virtual a_util::result::Result visit(const HeaderStructElement* struct_element);

    virtual a_util::result::Result visit(const HeaderEnum* header_enum);

public: // implements IDDLFactoryMethod
    virtual DDLDescription * getDDL() const;

    virtual a_util::result::Result createNew(const DDLVersion& version = DDLVersion::ddl_version_invalid);

    virtual void destroyDDL();

    virtual a_util::result::Result buildHeader();

    virtual a_util::result::Result buildUnits();

    virtual a_util::result::Result buildDatatypes();

    virtual a_util::result::Result buildStructs();

    virtual a_util::result::Result buildStreams();

    virtual a_util::result::Result buildEnums();
    virtual a_util::result::Result buildStreamMetaTypes();

private:

    /**
     * The method toAlignment calculates the alignment from an packing value.
     * 
     * @param [in] packing  The packing value
     * @returns  The alignment value, e0 if no matching packing or 0 was found.
     */
    ddl::DDLAlignment::AlignmentType toAlignment(size_t packing);

    /// The header that was passed.
    DDLDescription* _description;

    /// The header to be converted
    const Header* _header;

    /// The basic types found during parsing
    HeaderConstBasicTypes _basic_types;
    /// The constants found during parsing
    HeaderConstConstants _constants;
    /// The structs found during parsing
    HeaderConstStructs _structs;
    /// The typedefs found during parsing
    HeaderConstTypedefs _typedefs;
    /// The enums found during parsing
    HeaderConstEnums _enums;
};
}

#endif // HEADER_TO_DDL_CONVERTER_H_INCLUDED
