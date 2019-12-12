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

#ifndef DDL_STREAM_H_INCLUDED
#define DDL_STREAM_H_INCLUDED

#include "ddl_common.h"
#include "ddl_type.h"
#include "ddlversion.h"

namespace ddl
{
    class DDLStreamStruct;
    class DDLComplex;

    /**
     * Container type of DDLStreamStruct objects
     */
    typedef std::vector<DDLStreamStruct*> DDLStreamStructVec;
    /**
     * Iterator type for DDLStreamStructVec
     */
    typedef DDLStreamStructVec::iterator DDLStreamStructIt;

    /**
     * Representation of a stream inside a DDL description.
     */
    class DDLStream : public DDL
    {
    public:
        /**
         * Default CTOR
         */
        DDLStream();

        /**
         * CTOR
         * @param[in] type - Pointer to complex datatype object
         * @param[in] name - Name of the stream (optional)
         * @param[in] description - Description (optional)
         * @param[in] structs - Vector of \c DDLStreamStruct objects (optional)
         * @param[in] creation_level - Level at creation time (optional)
         */
        DDLStream(DDLComplex* type,
                   const std::string& name = a_util::strings::empty_string,
                   const std::string& description = a_util::strings::empty_string,
                   DDLStreamStructVec structs = DDLStreamStructVec(),
                   int const creation_level = 4);

         /**
         * Copy CTOR
         * @param[in] other - Reference to stream object to copy
         */
        DDLStream(const DDLStream &other);

        /**
         * Assignment operator (either copies or moves)
         * @param[in] other DDL stream object to copy from
         * @return @c *this
         */
        DDLStream& operator=(DDLStream other);

        /**
         * Move CTOR
         * @param[in,out] other DDL stream object to move from - empty but valid when finished
         */
        DDLStream(DDLStream&& other);

        /**
         * DTOR
         */
        virtual ~DDLStream();

        a_util::result::Result accept(IDDLVisitor *visitor) const;
        a_util::result::Result accept(IDDLChangeVisitor *visitor);

        bool isInitialized() const;

        int getCreationLevel() const;

        /**
         * Creation method to fill the object with data.
         * @param[in] type - Pointer to complex datatype object
         * @param[in] name - Name of the stream (optional)
         * @param[in] description - Description (optional)
         * @param[in] structs - Vector of \c DDLStreamStruct objects (optional)
         * @param[in] creation_level - Level at creation time (optional)
         * @retval ERR_POINTER Null-pointer committed
         */
        a_util::result::Result create(DDLComplex* type,
                       const std::string& name = a_util::strings::empty_string,
                       const std::string& description = a_util::strings::empty_string,
                       DDLStreamStructVec structs = DDLStreamStructVec(),
                       int const creation_level = 4);

        /**
         * Getter for the name of the complex data type.
         * @return name of the complex data type
         */
        std::string getType() const;

        /**
         * Setter for the object of the complex data type.
         * @param[in] type - the  type object referencing to
         * @return void
         */
        void setType(DDLComplex* const type);

        /**
         * Getter for the DDL version.
         * @return the DDL version
         */
        DDLVersion getDDLVersion() const;

        /**
         * Setter for the DDL version.
         * @param language_version the DDL version to set
         * @return Standard result code.
         * @retval a_util::result::SUCCESS 
         */
        a_util::result::Result setDDLVersion(DDLVersion& language_version);


        /**
         * Getter for the complex data-type object.
         * @return pointer to the complex data-type object
         */
        DDLComplex * getTypeObject() const;

        /**
         * Setter for the name.
         * @param[in] name - Name of the stream
         * @return void
         */
        void setName(const std::string& name);

        /**
         * Getter for the name.
         * @return the name
         */
        const std::string& getName() const;

        /**
         * Setter for the description.
         * @param[in] description - Description of the stream
         * @return void
         */
        void setDescription(const std::string& description);

        /**
         * Getter for the description.
         * @return the description
         */
        std::string getDescription() const;

        /**
         * Setter for the contained structs.
         * @param[in] structs - Vector of structures (complex data types)
         * @return void
         */
        void cloneStructs(DDLStreamStructVec structs);

        /**
         * Setter for the contained structs.
         * @param[in] structs - Vector of structures (complex data types)
         * @return void
         */
        void refStructs(DDLStreamStructVec structs);

        /**
         * Adder for a struct.
         * @param[in] ddl_struct - Pointer to the struct object to add
         * @return void
         */
        void addStruct(DDLStreamStruct* ddl_struct);

        /**
         * removal for a struct item.
         * @param[in] struct_name - name of struct object to remove
         * @retval ERR_NOT_FOUND if not exists 
         */
        a_util::result::Result removeStruct(const std::string& struct_name);

        /**
         * Getter for the contained structs.
         * @return the vector of structs
         */
        DDLStreamStructVec& getStructs();

        /**
         * Getter for the contained structs.
         * @return the vector of structs
         */
        const DDLStreamStructVec& getStructs() const;

        /**
         * Add swap functionality, also enabling the copy-swap-idiom
         * @param[in,out] lhs Left-hand side ddl type
         * @param[in,out] rhs Right-hand side ddl type
         */
        friend void swap(DDLStream& lhs, DDLStream& rhs) noexcept;

    private:
        std::string _name;
        DDLComplex * _type;
        std::string _description;
        DDLStreamStructVec _structs;
        bool _init_flag;
        int _level;
        DDLVersion _language_version;
    };

}   // namespace ddl

#endif  // DDL_STREAM_H_INCLUDED
