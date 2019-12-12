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

#ifndef DDLCOMPLEX_H_INCLUDED
#define DDLCOMPLEX_H_INCLUDED

#include "ddl_common.h"
#include "ddldatatype_intf.h"
#include "ddlvisitor_intf.h"
#include "ddl_type.h"
#include "ddlalignment.h"
#include "ddlversion.h"

namespace ddl
{

    class DDLElement;
    class DDLAlignment;

    /**
     * Container type of DDLElement objects
     */
    typedef std::vector<DDLElement*>      DDLElementVec;
    /**
     * Iterator type for DDLElementVec
     */
    typedef DDLElementVec::iterator       DDLElementIt;
    /**
     * Constant-iterator type for DDLElementVec
     */
    typedef DDLElementVec::const_iterator DDLElementItConst;

    /**
    * Representation for a complex datatype inside a DDL specification.
    */
    class DDLComplex : public IDDLDataType
    {
    public:
        /**
         * Default CTOR
         */
        DDLComplex();

        /**
         * CTOR
         * @param[in] name - Name of the data type
         * @param[in] version - Version number of the specified data type
         * @param[in] comment - Additional comments (optional)
         * @param[in] alignment - Alignment value (optional)
         * @param[in] ddl_elements - Vector of sub elements (optional)
         * @param[in] creation_level - Level at creation time (optional)
         */
        DDLComplex(const std::string& name,
                    unsigned int const version,
                    const std::string& comment = a_util::strings::empty_string,
                    DDLAlignment::AlignmentType const alignment = DDLAlignment::e1,
                    DDLElementVec ddl_elements = DDLElementVec(),
                    int const creation_level = 1,
                    DDLVersion language_version = DDLVersion::getDefaultVersion());

        /**
         * Copy CTOR
         * @param[in] other - Reference to complex datatype object to copy
         */
        DDLComplex(const DDLComplex &other);

        /**
         * Assignment operator (either copies or moves)
         * @param[in] other Complex DDL type to copy from
         * @return @c *this
         */
        DDLComplex& operator=(DDLComplex other);

        /**
         * Move CTOR
         * @param[in,out] other Complex DDL type to move from - empty but valid when finished
         */
        DDLComplex(DDLComplex&& other);

        /**
         * DTOR
         */
        virtual ~DDLComplex();

        a_util::result::Result accept(IDDLVisitor *visitor) const;
        a_util::result::Result accept(IDDLChangeVisitor *visitor);

        const std::string& getName() const;

        /**
         * Setter for the name of the complex datatype.
         * @param [in] name - Name of the complex datatype
         *
         * @return void
         */
        void setName(const std::string& name );


        bool isInitialized() const;

        bool isPredefined() const;
        bool isOverwriteable() const;

        int getCreationLevel() const;

        /**
         * Creation method to fill the object with data.
         * @param[in] name - Name of the data type
         * @param[in] version - Version number of the specified data type
         * @param[in] comment - Additional comments (optional)
         * @param[in] alignment - Alignment value (optional)
         * @param[in] ddl_elements - Vector of sub elements (optional)
         * @param[in] creation_level - Level at creation time (optional)
         * @retval ERR_INVALID_ARG Empty name committed
         */
        a_util::result::Result create(const std::string& name,
                       unsigned int const version,
                       const std::string& comment = a_util::strings::empty_string,
                       DDLAlignment::AlignmentType const alignment = DDLAlignment::e1,
                       DDLElementVec ddl_elements = DDLElementVec(),
                       int const creation_level = 1);

        /**
         * Getter for the version.
         * @return the version
         */
        unsigned int getVersion() const;

        /**
         * Setter for the version.
         * @param version the version to set
         *
         * @return void
         */
        void setVersion(unsigned int const version);

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
        a_util::result::Result setDDLVersion(const DDLVersion& language_version);

        /**
         * Setter for the comment.
         * @param[in] comment - Additional comments
         *
         * @return void
         */
        void setComment(const std::string& comment);

        /**
         * Getter for the comment.
         * @return the comment
         */
        std::string getComment() const;

        /**
         * Setter for the alignment.
         * @param[in] alignment - Alignment value
         *
         * @return void
         */
        void setAlignment(DDLAlignment::AlignmentType const alignment);

        /**
         * Setter for the element vector.
         * @param[in] ddl_elements - Vector of elements
         *
         * @return void
         */
        void cloneElements(DDLElementVec ddl_elements);

         /**
         * Setter for the element vector.
         * @param[in] ddl_elements - Vector of elements
         *
         * @return void
         */
        void setElements(DDLElementVec ddl_elements);

        /**
         * Adder for an element.
         * @param[in] element - Pointer to the element to add
         * @param[in] pos      - Position to add the element
         *
         * @return void
         */
        void addElement(DDLElement* element, int pos = -1);

        /**
         * removal for an element.
         * @param[in] element_name - name of the element to remove
         * @retval ERR_NOT_FOUND if not found
         */
        a_util::result::Result removeElement(const std::string& element_name);

        /**
         * Getter for the elements.
         * @return vector of the elements
         */
        const DDLElementVec& getElements() const;

        /**
         * Getter for the elements.
         * @return vector of the elements
         */
        DDLElementVec& getElements();

        /**
        * Getter for the boolean identifying dynamic elements.
        * *Note*: this function is not recursive, so to identify structures containing 
        * structures with dynamic elements, you have to do the recursion yourself.
        * @return true if dynamic elements were found
        */
        bool hasDynamicElements();

        /**
         * Getter for the alignment of the complex datatype.
         * @return the alignment
         */
        DDLAlignment::AlignmentType getAlignment() const;

        bool isComplex() const
        {
            return true;
        }

        /**
         * Add swap functionality, also enabling the copy-swap-idiom
         * @param[in,out] lhs Left-hand side ddl type
         * @param[in,out] rhs Right-hand side ddl type
         */
        friend void swap(DDLComplex& lhs, DDLComplex& rhs) noexcept;

    private:
        /// The DDL Version this structure was created in
        DDLVersion _language_version;
        std::string _name;
        unsigned int _version;
        std::string _comment;
        DDLAlignment::AlignmentType _alignment;
        DDLElementVec _ddl_elements;

        bool _init_flag;
        int _level;
        bool _dynamic_elements;
    };

}   // namespace ddl

#endif  // _COMPLEX_H_INCLUDED_
