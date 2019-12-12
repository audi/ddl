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

#ifndef HEADER_IMPORTER_H_INCLUDED
#define HEADER_IMPORTER_H_INCLUDED

#include "header_factorymethod_intf.h"
#include "header_type.h"
#include "header_struct.h"
#include "header_enum.h"
#include "header_header.h"

namespace ddl
{

/**
 * Container type for header types.
 */
typedef std::vector<HeaderType*> HeaderTypesVec;

/**
 * The class HeaderImporter imports headers from file or string.
 * The following settings can be performed regarding the import itself
 * Default integer type: see setDefaultIntegerType()
 */

class HeaderImporter : IHeaderFactory
{
public:
    
    HeaderImporter();
    virtual ~HeaderImporter();

    /**
     * The method SetSourceString sets the text that will be parsed.
     * Any filename previously set with setFileName will be ignored.
     * 
     * @param [in] source  The header string that will be parsed.
     * @returns  Standard result code.
     * @retval ERR_NOERROR  Everything went fine.
     * @retval ERR_UNKNOWN_FORMAT  Could not parse the string because it is not a valid header file.
     */
    a_util::result::Result setHeaderString(const std::string &source);

    /**
     * The method setFileName sets the path to the file that will be parsed.
     * Any string previously set with setHeaderString will be ignored.
     * 
     * @param [in] filename  The path to the file.
     * @returns  Standard result code.
     * @retval ERR_NOERROR  Everything went fine
     */
    a_util::result::Result setFileName(const a_util::filesystem::Path &filename);

    /**
     * The method setKnownTypes adds a list of already known types.
     * These types can be referenced by any member inside the header.
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
     * The method GetErrors gives access to the last error that occurred.
     * 
     * @returns  The last error
     */
    const std::string& getLastError() const;

    /**
     * The method setDefaultIntegerType sets the type to be used for constants defined by
     * \#define CONST_NAME 123.
     * The class does not take ownership of the type.
     * If no type is set, the createNew() method will try to find the type "tUInt64" within the known
     * types and use this one.
     *
     * @remark You could pass a pointer from the known types list (setKnownTypes) for best consistency.
     * 
     * @param [in] type  The type to use for defines.
     * @returns  Standard result code.
     * @retval ERR_NOERROR  Everything went fine
     */
    a_util::result::Result setDefaultIntegerType(const HeaderType* type);

    /**
    * The method getDefaultTypes creates a list of know types. The ADTF types will be treated as
    * THE basic types (so "tFloat32" is the most basic type and not "float").
    * The list will contain the ADTF types and types from stdint.h.
    * This list can be used with setKnownTypes()
    * 
    * @returns  A list of all known basic types. The caller will be the owner of the object.
    */
    static HeaderTypesVec* getDefaultTypes();



public: // implements IHeaderFactory
    
    virtual Header * getHeader();

    virtual a_util::result::Result createNew();

    virtual a_util::result::Result DestroyHeader();

    virtual a_util::result::Result BuildTypedefs();

    virtual a_util::result::Result BuildConstants();

    virtual a_util::result::Result buildStructs();

    virtual a_util::result::Result buildEnums();

private:
     /**
      * The method findKnownType searches all lists with known types
      * (typedefs and types in _header and other types from _types)
      * for a type with the passed name
      * 
      * @param [in] name  The name of the type to be found.
      * @returns  A pointer to the found type. If no type was found, the function returns NULL.
      */
     const HeaderType* findKnownType(const std::string &name) const;

     /**
      * The function extractStructElements parses a block that defines a struct
      * for elements.
      * The function expects the passed pointer to point to the next character
      * after the { character. After execution, the \a pos will point to the first
      * character after the closing } character.
      * The function does not keep the ownership of the elements inside the resulting vector.
      * The caller is responsible to delete these elements.
      * The order of the elements corresponds the order in the struct.
      * 
      * @param [in,out] pos A pointer to the first character of the
                             opening "{" character of the struct block.
                             The pointer will point to the first character
                             after the closing "}" of the block.
      * @returns  A vector of all found struct elements.
      */
     HeaderStructElementVec extractStructElements(const char* &pos);

     /**
      * The method addErrorDescription adds a description string to the internal error string.
      * It will try to extract the line number from the position of the source string.
      * 
      * @param [in] description  The error description to be added.
      * @param [in] pos  A pointer to the position inside the source string where the error occurred.
      * @returns  Standard result code.
      * @retval ERR_NOERROR  Everything went fine
      */
     a_util::result::Result addErrorDescription(const std::string &description, const char* pos);

     /**
      * The method parseTypedef parses a typedef that does redefine an existing type.
      * 
      * @param [inout] pos  A pointer to the first character after the word "typedef".
                          After invocation of this method, this pointer will point to
                          the first character after the final ";" of the typedef.
                          If no typedef could be parsed successfully, the pos will not be altered.
      * @param [out] new_type_name  The name of new type
      * @param [out] originalTypeName  The name of the original type
      * @param [out] token  The token if available (like struct or enum)
      * @returns  Standard result code.
      * @retval ERR_NOERROR  Everything went fine
      * @retval ERR_UNKNOWN_FORMAT If a typedef could not be identified.
      */
     a_util::result::Result parseTypedef(const char* &pos, std::string &token, std::string &original_type_name, std::string& new_type_name);

     /**
     * The function extractEnumValues parses a block that defines a enum
     * for it's values.
     * The function expects the passed pointer to point to the next character
     * after the { character. After execution, the \a pos will point to the first
     * character after the closing } character.
     *
     * @param [in,out] pos A pointer to the first character of the
     opening "{" character of the struct block.
     The pointer will point to the first character
     after the closing "}" of the block.
     * @returns  A map of all found enum values.
     */
     HeaderEnumValueMap extractEnumValues(const char* &pos);
    
private:
    /// The internal header being build up.
    Header* _header;
    /// The file to be parsed
    a_util::filesystem::Path _input_file;
    /// The string to be parsed
    std::string _header_source;    
    /// A list of already know types
    const HeaderTypesVec* _types;
    /// The last error that occurred
    std::string _last_error;
    /// The default integer type that will be used for defines, see also setDefaultIntegerType()
    const HeaderType* _default_type;
};
}

#endif //HEADER_IMPORTER_H_INCLUDED
