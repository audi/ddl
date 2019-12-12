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

#ifndef DDL_HEADER_H_INCLUDED
#define DDL_HEADER_H_INCLUDED

#include "ddl_common.h"
#include "ddl_type.h"
#include "ddlvisitor_intf.h"
#include "ddlversion.h"

namespace ddl
{

    class DDLExtDeclaration;

    /**
     * Container type of DDLExtDeclaration objects
     */
    typedef std::vector<DDLExtDeclaration*>   DDLExtDeclarationVec;
    /**
     * Iterator type for DDLExtDeclarationVec
     */
    typedef DDLExtDeclarationVec::iterator    DDLExtDeclarationIt;

    /**
     * Representation of the header of a DDL description.
     */
    class DDLHeader : public DDL
    {
    public:
        /**
         * Default CTOR
         */
        DDLHeader() = default;

        /**
         * CTOR
         * @param[in] language_version - Version number of the file where this description is based on
         * @param[in] author - Author
         * @param[in] date_creation - Creation date of file
         * @param[in] date_change - Date of last changes
         * @param[in] description - Short description of the file
         * @param[in] ext_decls - Vector of external declarations (optional)
         */
        DDLHeader(const DDLVersion& language_version,
                   const std::string& author,
                   a_util::datetime::Date const date_creation,
                   a_util::datetime::Date const date_change,
                   const std::string& description,
                   DDLExtDeclarationVec ext_decls = DDLExtDeclarationVec());

        /**
         * Copy CTOR
         * @param[in] other - Reference to header object that should be copied
         */
        DDLHeader(const DDLHeader&) = default;

        /**
         * Assignment operator (either copies or moves)
         * @param[in] other DDL header type to copy from
         * @return @c *this
         */
        DDLHeader& operator=(DDLHeader other);

        /**
         * Move CTOR
         * @param[in,out] other DDL header type to move from - empty but valid when finished
         */
        DDLHeader(DDLHeader&& other);


        /**
         * DTOR
         */
        virtual ~DDLHeader();

        a_util::result::Result accept(IDDLVisitor *visitor) const;
        a_util::result::Result accept(IDDLChangeVisitor *visitor);

        const std::string& getName() const;

        bool isInitialized() const;

        /**
         * Creation method to fill the object with data.
         * @param[in] language_version - Version number of the file where this description is based on
         * @param[in] author - Author
         * @param[in] date_creation - Creation date of file
         * @param[in] date_change - Date of last changes
         * @param[in] description - Short description of the file
         * @param[in] ext_decls - Vector of external declarations (optional)
         *
         * @return Standard result.
         */
        a_util::result::Result create(DDLVersion& language_version, const std::string& author,
            a_util::datetime::Date const date_creation, a_util::datetime::Date const date_change,
            const std::string& description,
            DDLExtDeclarationVec ext_decls = DDLExtDeclarationVec());

        /**
         * Getter for the language version.
         * @return the language version
         */
        DDLVersion getLanguageVersion() const;

        /**
         * Setter for the language version.
         * @param[in] language_version - the language version
         * @return void
         */
        void setLanguageVersion(const DDLVersion& language_version);

        /**
         * Getter for the author.
         * @return the author
         */
        std::string getAuthor() const;

        /**
         * Setter for the author.
         * @param[in] author - Author
         *
         * @return void
         */
        void setAuthor(const std::string& author);

        /**
         * Getter for the creation date.
         * @return the creation date
         */
        a_util::datetime::Date getDateCreation() const;

        /**
         * Setter for the creation date.
         * @param[in] date_creation - Creation date to set
         *
         * @return void
         */
        void setDateCreation(a_util::datetime::Date const date_creation);

        /**
         * Getter for the change date.
         * @return the change date
         */
        a_util::datetime::Date getDateChange() const;

        /**
         * Setter for the change date.
         * @param[in] date_change - Change date to set
         *
         * @return void
         */
        void setDateChange(a_util::datetime::Date const date_change);

        /**
         * Getter for the description.
         * @return the description
         */
        std::string getDescription() const;

        /**
        * Setter for the description.
        */
        void setDescription(const std::string& description);

        /**
         * Setter for the external declarations
         * @param[in] ext_declarations - Vector of external declarations
         *
         * @return void
         */
        void setExtDeclarations(DDLExtDeclarationVec ext_declarations);

        /**
         * Adder for an external declaration
         * @param[in] ext_declarations - Pointer to the external declaration
         * @param[in] pos             - Position to add the external declaration
         *
         * @return void
         */
        void addExtDeclaration(DDLExtDeclaration * ext_declarations, int pos = -1);

        /**
         * Removes the external declaration with the given key.
         * @param [in] key - Key of the external declaration which should be removed
         * @retval ERR_NOT_FOUND The specified key was not found
         */
        a_util::result::Result removeExtDeclaration(const std::string& key);

        /**
         * Getter for the external declarations
         * @return the external declarations
         */
        const DDLExtDeclarationVec& getExtDeclarations() const;

        /**
         * Getter for the external declarations
         * @return the external declarations
         */
        DDLExtDeclarationVec& getExtDeclarations();

        /**
         * Helper method to extract a date out of a string.
         * Following date formats are supported (complying with DDL spec):<br><ul>
         * <li> yyyymmdd </li>
         * <li> dd-mm-yyyy </li>
         * <li> yyyy-mm-dd </li>
         * <li> dd.mm.yyyy </li></ul>
         * 
         * @param[in] date - String containing the date
         * @return the date structure
         */
        static a_util::datetime::Date dateFromString(const std::string& date);

        /**
         * Add swap functionality, also enabling the copy-swap-idiom
         * @param[in,out] lhs Left-hand side ddl header
         * @param[in,out] rhs Right-hand side ddl header
         */
        friend void swap(DDLHeader& lhs, DDLHeader& rhs) noexcept;


    private:
        DDLVersion _language_version;
        std::string _author;
        a_util::datetime::Date _date_creation;
        a_util::datetime::Date _date_change;
        std::string _description;
        DDLExtDeclarationVec _ddl_ext_declarations;

        bool _init_flag;
    };

}   // namespace ddl

#endif  // DDL_HEADER_H_INCLUDED
