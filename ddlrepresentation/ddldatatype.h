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

#ifndef DDLDATATYPE_H_INCLUDED
#define DDLDATATYPE_H_INCLUDED

#include "ddl_common.h"
#include "ddldatatype_intf.h"
#include "ddlvisitor_intf.h"
#include "ddlunit_intf.h"
#include "ddlalignment.h"
#include "ddlversion.h"

namespace ddl
{

    /**
    * Representation for a (simple) data type inside a DDL description.
    */
    class DDLDataType : public IDDLDataType
    {
    public:
        /**
        * Default CTOR
        */
        DDLDataType();

        /**
         * CTOR
         * @param[in] name - Name of the primitive data type
         * @param[in] size - Size in bit
         * @param[in] unit - Pointer to the unit of the data type (optional)
         * @param[in] description - Description of the primitive data type (optional)
         * @param[in] alignment - Alignment of the primitive data type (optional)
         * @param[in] array_size - Array size of the primitive data type (optional)
         * @param[in] creation_level - Level at creation time (optional)
         * @param[in] array_size_source - Name of the element that represents
         * the arrays size (optional)
         * @param [in] min_valid - Validity flag for the minimum value (optional)
         * @param[in] min_val - Minimum value of the data type (optional)
         * @param [in] max_valid - Validity flag for the maximum value (optional)
         * @param[in] max_val - Maximum value of the data type (optional)
         */
        DDLDataType(const std::string& name,
                     unsigned int const size,
                     IDDLUnit* unit = NULL,
                     const std::string& description = a_util::strings::empty_string,
                     DDLAlignment::AlignmentType const alignment = DDLAlignment::e1,
                     unsigned int const array_size = 1,
                     int const creation_level = 1,
                     const std::string& array_size_source = a_util::strings::empty_string,
                     bool const min_valid = false,
                     const std::string& min_val = a_util::strings::empty_string,
                     bool const max_valid = false,
                     const std::string& max_val = a_util::strings::empty_string);

        a_util::result::Result accept(IDDLVisitor *visitor) const;
        a_util::result::Result accept(IDDLChangeVisitor *visitor);

        bool isPredefined() const;
        bool isOverwriteable() const;

        const std::string& getName() const;

        /**
         * Setter for the name of the data type.
         * @param [in] name Name of the data type
         *
         * @return void
         */
        void setName(const std::string& name);
        int getCreationLevel() const;

        bool isInitialized() const;

        /**
         * Creation method to fill the object with data.
         * @param[in] name - Name of the primitive data type
         * @param[in] size - Size in bit
         * @param[in] unit - Pointer to the unit of the data type
         * @param[in] description - Description of the primitive data type (optional)
         * @param[in] alignment - Alignment of the primitive data type (optional)
         * @param[in] array_size - Array size of the primitive data type (optional)
         * @param[in] creation_level - Level at creation time (optional)
         * @param[in] array_size_source - Name of the element that represents the arrays size
         * @param [in] min_valid - Validity flag for the minimum value (optional)
         * @param[in] min_val - Minimum value of the data type (optional)
         * @param [in] max_valid - Validity flag for the maximum value (optional)
         * @param[in] max_val - Maximum value of the data type (optional)
         * @retval ERR_POINTER Null-pointer committed
         */
        a_util::result::Result create(const std::string& name,
            unsigned int const size,
            IDDLUnit* unit = NULL,
            const std::string& description = a_util::strings::empty_string,
            DDLAlignment::AlignmentType const alignment = DDLAlignment::e1,
            unsigned int const array_size = 1,
            int const creation_level = 1,
            const std::string& array_size_source = a_util::strings::empty_string,
            bool const min_valid = false,
            const std::string& min_val = a_util::strings::empty_string,
            bool const max_valid = false,
            const std::string& max_val = a_util::strings::empty_string);


        /**
         * Setter for the description.
         * @param[in] description - Description of the primitive data type
         *
         * @return void
         */
        void setDescription(const std::string& description);

        /**
         * Getter for the description.
         * @return the description
         */
        std::string getDescription() const;

        /**
         * Setter for the array size.
         * @param[in] array_size - Array size of the data type<br>
         * = 1 -> primitve presentation<br>\> 1 -> array with the amount of elements
         *
         * @return void
         */
        void setArraysize(unsigned int const array_size);

        /**
         * Getter for the array size.
         * @return the array size
         */
        unsigned int getArraysize() const;

        /**
         * Setter for the unit.
         * @param[in] unit - Pointer to the unit object of the data type
         *
         * @return void
         */
        void setUnit(IDDLUnit* unit);

        /**
         * Getter for the unit name.
         * @return name of the referenced unit (\c a_util::strings::empty_string if there is none)
         */
        std::string getUnit() const;

        /**
         * Getter for the unit object.
         * @return pointer to the unit object (\c NULL if there is none)
         */
        const IDDLUnit * getUnitObject() const;

        /**
         * Getter for the unit object.
         * @return pointer to the unit object (\c NULL if there is none)
         */
        IDDLUnit * getUnitObject();

        /**
         * Setter for the alignment.
         * @param[in] alignment - Alignment value
         *
         * @return void
         */
        void setAlignment(DDLAlignment::AlignmentType const alignment);

        /**
         * Getter for the alignment of the primitive data type.
         * @return the alignment
         */
        DDLAlignment::AlignmentType getAlignment() const;

        /**
         * Getter for the size of the primitive data type.
         * @return the size in [bit]
         */
        unsigned int getNumBits() const;

        /**
         * Setter for the size of the primitive data type.
         * @param [in] num_bits - Size in [bit]
         *
         * @return void
         */
        void setNumBits(unsigned int const num_bits);

         /**
         * Getter for the array size source.
         * @return the array size source
         */
        std::string getArraySizeSource() const;

         /**
         * Setter for the array size source.
         * @param[in] array_size_source - the array size source element
         *
         * @return void
         */
        void setArraySizeSource(const std::string& array_size_source);

          /**
         * Checks if element is dynamic
         * @return true if it is a dynamic array
         */
        bool isDynamic() const;

        /**
         * Getter for the DDL version.
         * @return the DDL version
         */
        DDLVersion getDDLVersion() const;

        /**
         * Setter for the DDL version.
         * @param language_version the DDL version to set
         * @return Standard result code.
         * @retval a_util::result::SUCCESS  Everything is fine.
         */
        a_util::result::Result setDDLVersion(DDLVersion& language_version);

        /**
         * Setter for the validity flag for the minimum value.
         *
         * @param [in] min_valid Validity flag for the minimum value.
         *
         * @return void
         */
        void setMinValidity (bool const min_valid);

        /**
         * Getter for the validity flag for the minimum value.
         *
         * @return the validity flag for the minimum value
         */
        bool isMinValid () const;

        /**
         * Getter for the minimum value of the data type.
         *
         * @return the minimum value
         */
        const std::string &getMinValue () const;

        /**
         * Setter for the minimum value of the data type.
         *
         * @param [in] min_val The new minimum value
         *
         * @retval a_util::result::SUCCESS Everything went fine.
         */
        a_util::result::Result setMinValue (const std::string& min_val);

        /**
         * Setter for the validity flag for the maximum value.
         *
         * @param [in] max_valid Validity flag for the maximum value.
         *
         * @return void
         */
        void setMaxValidity (bool const max_valid);

        /**
         * Getter for the validity flag for the maximum value.
         *
         * @return the validity flag for the maximum value
         */
        bool isMaxValid () const;

        /**
         * Getter for the maximum value of the data type.
         *
         * @return the maximum value
         */
        const std::string &getMaxValue () const;

        /**
         * Setter for the maximum value of the data type.
         *
         * @param [in] max_val The new maximum value
         *
         * @retval a_util::result::SUCCESS Everything went fine.
         */
        a_util::result::Result setMaxValue (const std::string& max_val);


    private:    // members
        /// The DDL Version this structure was created in
        DDLVersion _language_version;
        std::string _name;
        unsigned int _num_bits;
        size_t _byte_size;
        size_t _aligned_size;
        IDDLUnit* _unit;
        std::string _description;
        unsigned int _array_size;
        std::string _array_size_source;
        DDLAlignment::AlignmentType _alignment;

        bool _init_flag;
        int _level;

        bool               _min_valid;
        std::string  _min_val;
        bool               _max_valid;
        std::string  _max_val;
        bool               _default_valid;
        std::string  _default_val;
    };

}   // namespace ddl

#endif  // DDLDATATYPE_H_INCLUDED
