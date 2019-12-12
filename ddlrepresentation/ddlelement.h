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

#ifndef DDLELEMENT_H_INCLUDED
#define DDLELEMENT_H_INCLUDED

#include "ddl_common.h"
#include "ddldatatype_intf.h"
#include "ddlbyteorder.h"
#include "ddlalignment.h"
#include "ddlunit_intf.h"

namespace ddl
{

    /**
     * Representation of an element inside a struct in DDL a description.
     */
    class DDLElement
    {
    public:
        /**
         * Default CTOR
         */
        DDLElement() = default;

        /**
         * CTOR
         * @param[in] type - Pointer to the data-type object
         * @param[in] name - Name of the created element
         * @param[in] byte_pos - Byte position of the referenced data type
         * @param[in] array_size - Array size of the element
         * @param[in] byte_order - Byte order of the related data type
         * @param[in] alignment - Alignment of the related data type
         * @param[in] unit - Pointer to unit object
         * @param[in] bit_pos - Bit position of the referenced data type (optional)
         * @param[in] num_bits - The amount of bits used for the representation (optional)
         * @param[in] description - Description of the created data type (optional)
         * @param[in] comment - Additional comments (optional)
         * @param[in] array_size_source - The name of the source element for the size of a dynamic array (optional)
         * @param[in] value - The value of a constant element (optional)
         * @param[in] min_valid - Validity flag for the minimum value (optional)
         * @param[in] min_value - Minimum value of the element (optional)
         * @param[in] max_valid - Validity flag for the maximum value (optional)
         * @param[in] max_value - Maximum value of the element (optional)
         * @param[in] default_valid - Validity flag for the default value (optional)
         * @param[in] default_value - Default value of the element (optional)
         * @param[in] scale_valid - Validity flag for the scaling value (optional)
         * @param[in] scale_value - Scaling value of the element (optional)
         * @param[in] offset_valid - Validity flag for the offset value (optional)
         * @param[in] offset_value - Offset value of the element (optional)
         */
        DDLElement(IDDLDataType* type,
                    const std::string& name,
                    unsigned int const byte_pos,
                    unsigned int const array_size,
                    DDLByteorder::Byteorder const byte_order,
                    DDLAlignment::AlignmentType const alignment,
                    IDDLUnit* unit = NULL,
                    unsigned int const bit_pos = 0,
                    unsigned int const num_bits = 0,
                    const std::string& description = a_util::strings::empty_string,
                    const std::string& comment = a_util::strings::empty_string,
                    const std::string& array_size_source = a_util::strings::empty_string,
                    const std::string& value = a_util::strings::empty_string,
                    bool const min_valid = false,
                    const std::string& min_value = a_util::strings::empty_string,
                    bool const max_valid = false,
                    const std::string& max_value = a_util::strings::empty_string,
                    bool const default_valid = false,
                    const std::string& default_value = a_util::strings::empty_string,
                    bool const scale_valid = false,
                    const std::string& scale_value = a_util::strings::empty_string,
                    bool const offset_valid = false,
                    const std::string& offset_value = a_util::strings::empty_string);

        /**
         * @copydoc IDDL::accept()
         */
        a_util::result::Result accept(IDDLVisitor *visitor) const;
        a_util::result::Result accept(IDDLChangeVisitor *visitor);

        /**
         * @copydoc IDDL::isInitialized()
         */
        bool isInitialized() const;

   /*     a_util::result::Result Serialize(void const *pvDeSerData,
                          size_t const &szSizeDeSerInByte,
                          size_t const &szAlignedOffsetInByte,
                          void *pvSerData,
                          size_t const &szSizeSerInByte,
                          size_t const &szOffsetInBit);

        a_util::result::Result Deserialize(void *pvDeSerData,
            size_t const &szSizeDeSerInByte,
            size_t const &szAlignedOffsetInByte,
            void const *pvSerData,
            size_t const &szSizeSerInByte,
            size_t const &szOffsetInBit);

        size_t getSize() const;*/

        /**
         * @copydoc IDDL::isPredefined()
         */
        bool isPredefined() const;

        /**
         * @copydoc IDDL::getCreationLevel()
         */
        int getCreationLevel() const;

        /**
         * Creation method to fill the obejct with data.
         * @param[in] type - Pointer to the datatype object
         * @param[in] name - Name of the created element
         * @param[in] byte_pos - Byte position of the referenced data type
         * @param[in] array_size - Arraysize of the element
         * @param[in] byte_order - Byteorder of the related data type
         * @param[in] alignment - Alignment of the related data type
         * @param[in] unit - Pointer to unit object
         * @param[in] bit_pos - Bit position of the referenced data type (optional)
         * @param[in] num_bits - The amount of bits used for the representation (optional)
         * @param[in] description - Description of the created data type (optional)
         * @param[in] comment - Additional comments (optional)
         * @param[in] array_size_source - The name of the source element for the size of a dynamic array
         * @param[in] value - The value of a constant element (optional)
         * @param[in] min_valid - Validity flag for the minimum value (optional)
         * @param[in] min_value - Minimum value of the element (optional)
         * @param[in] max_valid - Validity flag for the maximum value (optional)
         * @param[in] max_value - Maximum value of the element (optional)
         * @param[in] default_valid - Validity flag for the default value (optional)
         * @param[in] default_value - Default value of the element (optional)
         * @param[in] scale_valid - Validity flag for the scaling value (optional)
         * @param[in] scale_value - Scaling value of the element (optional)
         * @param[in] offset_valid - Validity flag for the offset value (optional)
         * @param[in] offset_value - Offset value of the element (optional)
         * @retval ERR_POINTER Null-pointer committed
         */
        a_util::result::Result create(IDDLDataType* type,
            const std::string& name,
            unsigned int const byte_pos,
            unsigned int const array_size,
            DDLByteorder::Byteorder const byte_order,
            DDLAlignment::AlignmentType const alignment,
            IDDLUnit* unit = NULL,
            unsigned int const bit_pos = 0,
            unsigned int const num_bits = 0,
            const std::string& description = a_util::strings::empty_string,
            const std::string& comment = a_util::strings::empty_string,
            const std::string& array_size_source = a_util::strings::empty_string,
            const std::string& value = a_util::strings::empty_string,
            bool const min_valid = false,
            const std::string& min_value = a_util::strings::empty_string,
            bool const max_valid = false,
            const std::string& max_value = a_util::strings::empty_string,
            bool const default_valid = false,
            const std::string& default_value = a_util::strings::empty_string,
            bool const scale_valid = false,
            const std::string& scale_value = a_util::strings::empty_string,
            bool const offset_valid = false,
            const std::string& offset_value = a_util::strings::empty_string);

        /**
         * Getter for the data-type name.
         * @return name of the data type
         */
        const std::string& getType() const;

        /**
         * Setter for the type of the element.
         * @param [in] type Pointer to the type object
         *
         * @return void
         */
        void setType(IDDLDataType* const type);

        /**
         * Getter for the data-type object.
         * @return pointer to the data-type object
         */
        IDDLDataType * getTypeObject() const;

        /**
         * Getter for the name.
         * @return the name
         */
        const std::string& getName() const;

        /**
         * Setter for the name of the element.
         * @param [in] name Name of the element
         *
         * @return void
         */
        void setName(const std::string& name);

        /**
         * Getter for the b byte position.
         * @return the byte position
         */
        unsigned int getBytepos() const;

        /**
         * Setter for the byte position of the element.
         * @param [in] byte_pos Byte position of the element
         *
         * @return void
         */
        void setBytepos(unsigned int const byte_pos);


        /**
         * Getter for the array size.
         * @return the array size
         */
        unsigned int getArraysize() const;

        /**
         * Setter for the array size of the element.
         * @param [in] array_size Array size of the element
         *
         * @return void
         */
        void setArraysize(unsigned int const array_size);

        /**
         * Getter for the byteorder.
         * @return the byteorder
         */
        DDLByteorder::Byteorder getByteorder() const;

        /**
         * Setter for the byte order of the element.
         * @param [in] byte_order Byte order of the element
         *
         * @return void
         */
        void setByteorder(DDLByteorder::Byteorder const byte_order);

        /**
         * Getter for the alignment.
         * @return the alignment
         */
        DDLAlignment::AlignmentType getAlignment() const;

        /**
         * Setter for the alignment of the element.
         * @param [in] alignment Alignment of the element
         *
         * @return void
         */
        void setAlignment(DDLAlignment::AlignmentType const alignment);

        /**
         * Setter for the bit position.
         * @param[in] bit_pos - Bit position of the referenced data type
         *
         * @return void
         */
        void setBitpos(unsigned int const bit_pos);

        /**
         * Getter for the bit position.
         * @return the bit position
         */
        unsigned int getBitpos() const;

        /**
         * Setter for the amount of bits.
         * @param[in] num_bits - Amount of bits used for the representation
         *
         * @return void
         */
        void setNumBits(unsigned int const num_bits);

        /**
         * Getter for the amount of bits.
         * @return the amount of bits
         */
        unsigned int getNumBits() const;

        /**
         * Setter for the description.
         * @param[in] description - the description
         *
         * @return void
         */
        void setDescription(const std::string& description);

        /**
         * Getter for the description.
         * @return the description
         */
        const std::string& getDescription() const;

        /**
         * Setter for the unit reference.
         * @param[in] unit - Pointer to the unit object of the element
         *
         * @return void
         */
        void setUnit(IDDLUnit* unit);

        /**
         * Getter for the unit name.
         * @return the unit name
         */
        const std::string& getUnit() const;

        /**
         * Getter for the unit object.
         * @return pointer to the unit object
         */
        IDDLUnit * getUnitObject() const;

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
        const std::string& getComment() const;

          /**
         * Getter for the array size source.
         * @return the array size source
         */
        const std::string& getArraySizeSource() const;

         /**
         * Setter for the array size source.
         * @param[in] array_size_source - the array size source element
         *
         * @return void
         */
        void setArraySizeSource(const std::string& array_size_source);

        
          /**
         * Getter for the value of a constant element.
         * @return the value of a constant element
         */
        const std::string& getConstantValue() const;

         /**
         * Setter value of a constant element.
         * @param[in] constant_value - constant value
         *
         * @return void
         */
        void setConstantValue(const std::string& constant_value);

        /**
         * Checks if element is dynamic
         * @return true if it is a dynamic array
         */
        bool isDynamic() const;

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
        const std::string& getMinValue() const;

        /**
         * Setter for the minimum value of the data type.
         *
         * @param [in] min_value The new minimum value
         *
         * @retval a_util::result::SUCCESS Everything went fine.
         */
        a_util::result::Result setMinValue (const std::string& min_value);

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
        const std::string& getMaxValue () const;

        /**
         * Setter for the maximum value of the data type.
         *
         * @param [in] max_value The new maximum value
         *
         * @retval a_util::result::SUCCESS Everything went fine.
         */
        a_util::result::Result setMaxValue (const std::string& max_value);

        /**
         * Setter for the validity flag for the default value.
         *
         * @param [in] default_valid Validity flag for the default value
         *
         * @return void
         */
        void setDefaultValidity (bool const default_valid);
        
        /**
         * Getter for the validity flag for the default value.
         *
         * @return the validity flag for the default value
         */
        bool isDefaultValid () const;

        /**
         * Getter for the default value.
         *
         * @return the default value
         */
        const std::string& getDefaultValue () const;

        /**
         * Setter for the default value.
         *
         * @param [in] default_value The new default value
         *
         * @retval a_util::result::SUCCESS Everything went fine.
         */
        a_util::result::Result setDefaultValue (const std::string& default_value);

        /**
         * Setter for the validity flag for the scaling value.
         *
         * @param [in] scale_valid Validity flag for the scaling value
         *
         * @return void
         */
        void setScaleValidity (bool const scale_valid);

        /**
         * Getter for the validity flag for the scaling value.
         *
         * @return the validity flag for the scaling value.
         */
        bool isScaleValid () const;

        /**
         * Getter for the scaling value.
         *
         * @return the scaling value
         */
        const std::string& getScaleValue () const;

        /**
         * Setter for the scaling value.
         *
         * @param [in] scale_value The new scaling value
         *
         * @retval a_util::result::SUCCESS Everything went fine.
         */
        a_util::result::Result setScaleValue (const std::string& scale_value);

        /**
         * Setter for the validity flag for the offset value.
         *
         * @param [in] offset_valid Validity flag for the offset value
         *
         * @return void
         */
        void setOffsetValidity (bool const offset_valid);

        /**
         * Getter for the validity flag for the offset value.
         *
         * @return the validity flag for the offset value.
         */
        bool isOffsetValid () const;

        /**
         * Getter for the offset value.
         *
         * @return the offset value.
         */
        const std::string& getOffsetValue () const;

        /**
         * Setter for the offset value.
         *
         * @param [in] offset_value The new offset value
         *
         * @retval a_util::result::SUCCESS Everything went fine.
         */
        a_util::result::Result setOffsetValue (const std::string& offset_value);

    private:
        IDDLDataType * _type;
        std::string _name;
        unsigned int _bytes_pos;
        unsigned int _bit_pos;
        unsigned int _num_bits;
        std::string _description;
        unsigned int _array_size;
        std::string _array_size_source;
        std::string _constant_value;
        IDDLUnit * _unit;
        DDLByteorder::Byteorder _byteorder;
        DDLAlignment::AlignmentType _alignment;
        std::string _comment;
        bool _init_flag;

        bool               _min_valid;
        std::string  _min_value;
        bool               _max_valid;
        std::string  _max_value;
        bool               _default_valid;
        std::string  _default_value;
        bool               _scale_valid;
        std::string  _scale_value;
        bool               _offset_valid;
        std::string  _offset_value;
    };
}   // namespace ddl

#endif  // DDLELEMENT_H_INCLUDED
