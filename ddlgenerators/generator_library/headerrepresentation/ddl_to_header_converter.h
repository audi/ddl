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

#ifndef HEADER_DDL_TO_HEADER_CONVERTER_H_INCLUDED
#define HEADER_DDL_TO_HEADER_CONVERTER_H_INCLUDED

#include "header_type.h"
#include "header_factorymethod_intf.h"
#include "header_importer.h"
#include "header_header.h"

namespace ddl
{

typedef std::vector<const HeaderType*> HeaderConstTypes;

/**
 * This class generates a header from a DDL.
 * Use this class by first visiting the element(s) of the DDL you want to be part of your DDL,
 * then call createNew() to create the header from the previously parsed header elements.
 */
class DDLToHeaderConverter : public IDDLVisitor, public IHeaderFactory
{
public:
    /**
     * CTOR
     */
    DDLToHeaderConverter();

    /**
     * DTOR
     */
    virtual ~DDLToHeaderConverter();

    /**
     * The method setKnownTypes adds a list of already known types.
     * These types can be referenced by any member inside the new header.
     * Please be aware, that neither the importer object nor the
     * resulting header object will own the types. The resulting
     * header object will only reference them. So the resulting header
     * must be deleted before the types are deleted.
     * 
     * @param [in] types  A list of types already know to the system.
     * @returns  Standard result code.
     * @retval ERR_NOERROR  Everything went fine
     */
    a_util::result::Result setKnownTypes(const HeaderTypesVec* types);
    
    /**
     * The method getError returns the error messages that occurred during parsing.
     * 
     * @returns  The error messages.
     */
    const std::string& getError();

    void setDisplaceableString(std::string displace);
    

public: // implements IDDLVisitor
    a_util::result::Result visitDDL(const DDLDescription *description);
    a_util::result::Result visit(const DDLHeader *header);
    a_util::result::Result visit(const DDLDataType *data_type);
    a_util::result::Result visit(const DDLComplex *ddl_struct);
    a_util::result::Result visit(const DDLStream *stream);
    a_util::result::Result visit(const DDLUnit *unit);
    a_util::result::Result visit(const DDLBaseunit *base_unit);
    a_util::result::Result visit(const DDLExtDeclaration *ext_declaration);
    a_util::result::Result visit(const DDLElement *element);
    a_util::result::Result visit(const DDLPrefix *prefix);
    a_util::result::Result visit(const DDLRefUnit *ref_unit);
    a_util::result::Result visit(const DDLStreamStruct *stream_struct);
    a_util::result::Result visit(const DDLEnum *ddl_enum);
    a_util::result::Result visit(const DDLProperty* ddl_property);
    a_util::result::Result visit(const DDLStreamMetaType* stream_meta_type);

public: // implements IHeaderFactory
    Header* getHeader();
    a_util::result::Result createNew();
    a_util::result::Result DestroyHeader();
    a_util::result::Result BuildTypedefs();
    a_util::result::Result BuildConstants();
    a_util::result::Result buildStructs();
    a_util::result::Result buildEnums();

private:
    /// The header to be created
    Header* _header;
    /// The known types
    const HeaderTypesVec* _known_types;
    /// The Typedefs found during parsing
    HeaderTypedefs _typedefs;
    /// The structs found during parsing
    HeaderStructs _structs;
    /// The constants found during parsing
    HeaderConstants _constants;
    /// The enums found during parsing
    HeaderEnums _enums;

    /// Collecting all error messages here.
    std::string _error_string;

    /// String that should be removed at the beginning of element names
    std::string _displace;

    /**
     * The method findTypeByName searches m_vecTypes for a type by the given name.
     * 
     * @param [in] name  The name of the type to be found.
     * @returns  A pointer to the type if found, NULL otherwise.
     */
    const HeaderType* findTypeByName(const std::string &name);

    /**
     * The method findTypeByName helps finding a type in a vector containin pointer to
     * header types.
     * 
     * @param [in] vec  The vector to be looked through.
     * @param [in] name  The name of the type to be found
     * @returns  A pointer to the type if found, NULL otherwise.
     */
    template <class C>
    const HeaderType* findTypeByName(const std::vector<C> &vec, const std::string &name);

    /**
     * The method fromAlignment calculates the packing from an alignment value.
     * 
     * @param [in] alignment  The alignment value
     * @returns  The packing value, 0 if no matching packing or e0 was found.
     */
    size_t fromAlignment(DDLAlignment::AlignmentType alignment);

    /**
     * The method cleanUpName replaces all non letter/number/underscore characters from
     * the passed string with underscores and returns the result as a new string.
     * 
     * @param [in] name  The string to be cleaned up.
     * @returns A string only containing letters, numbers and underscores.
     */
    std::string cleanUpName(const std::string &name);

    bool isEnumIntegral(const DDLEnum *ddl_enum) const;

};

}

#endif // HEADER_DDL_TO_HEADER_CONVERTER_H_INCLUDED
