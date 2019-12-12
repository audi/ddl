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

#ifndef DDLCOMPARE_H_INCLUDED
#define DDLCOMPARE_H_INCLUDED

#include "ddl_common.h"
#include "ddldescription.h"
#include "ddlunit_intf.h"
#include "ddlelement.h"

namespace ddl
{

/**
 * Utility class to compare media descriptions
 */
class DDLCompare
{
    public:
        /**
         * Flags for the description based isEqual methods
         */
        enum DescriptionCheckFlags
        {
            dcf_data_types = 0x01, ///< Compare datatypes
            dcf_units = 0x02, ///< Compare units
            dcf_enums = 0x04,  ///< Compare enums
            dcf_structs = 0x08,  ///< Compare structs
            dcf_streams = 0x10,  ///< Compare streams
            dcf_base_units = 0x20,  ///< Compare baseunits
            dcf_prefixes = 0x40,  ///< Compare prefixes
            dcf_all = 0xFF,  ///< Compare all elements (see above)
            dcf_subset = 0x0100, ///< Checks whether the first description is a subset of the second
            dcf_versions = 0x0200, ///< Compares the version attributes as well
            dcf_comments = 0x0400, ///< Compares the comment attributes as well
            dcf_descriptions = 0x0800, ///< Compares the description attributes as well
            dcf_header = 0x1000, ///< Compare the header as well
            dcf_visualization_attributes = 0x2000, ///< Check attributes relevant for visualization (min/max/default/scale/offset)
            dcf_no_enum_values_check = 0x010000, ///< Do not compare enum values.
            dcf_no_recursion = 0x020000, ///< Do not compare sub-entities (elements, ref units, stream structs, ...)
            dcf_no_header_dates = 0x040000, ///< Do not compare header dates (date_creation, date_change)

            dcf_everything = 0xFFFF  ///< Check everything (all flags except the "No" variants)
        };

        /**
         * Flags for the item based isEqual methods
         */
        enum ItemCheckFlags
        {
            icf_none = 0x00,
            icf_memory = 0x01, ///< Compare the in-memory representation
            icf_serialized = 0x02, ///< Compare the serialized representation
            icf_names = 0x04, ///< Compare the names of structs and their elements
            icf_versions = 0x08, ///< Compare the versions of all elements and structs
            icf_units = 0x10, ///< Compare the units as well
            icf_comments = 0x20, ///< Compare the comments as well
            icf_descriptions = 0x40, ///< Compare the comments as well
            icf_subset = 0x80, ///< Check if the first item is a subset of the second.
            icf_visualizations_attributes = 0x0100, ///< Check attributes relevant for visualization (min/max/default/scale/offset)
            icf_no_enum_values_check = 0x0200, ///< Do not compare enum values.
            icf_no_recursion = 0x0400 ///< Do not compare sub-entities (elements, ref units, stream structs, ...)
        };

    public:

        /**
         * @brief isBinaryEqual checks whether two type descriptions describe the same binary data layout
         * @param [in] type1 The name of the first type.
         * @param [in] desc1 The description that has type1.
         * @param [in] type2 The name of the second type.
         * @param [in] desc2 The description that has type2.
         * @param [in] is_subset If true then the method checks if the first type is a subset of the second (starting at offset 0, possibly with smaller size).
         * @retval a_util::result::SUCCESS The description describe the same layout
         * @retval ERR_FAILED The description do NOT describe the same layout
         * @retval Standard result (other DDL parsing errors etc.)
         */
        static a_util::result::Result isBinaryEqual(const std::string& type1, const std::string& desc1,
            const std::string& type2, const std::string& desc2,
                                     bool is_subset = true);

        /**
         * @brief isBinaryEqual checks whether two type descriptions describe the same binary data layout
         * @param [in] type1 The name of the first type.
         * @param [in] desc1 The description that has type1.
         * @param [in] type2 The name of the second type.
         * @param [in] desc2 The description that has type2.
         * @param [in] is_subset If true then the method checks if the first type is a subset of the second (starting at offset 0, possibly with smaller size).
         * @retval a_util::result::SUCCESS The description describe the same layout
         * @retval ERR_FAILED The description do NOT describe the same layout
         * @retval Standard result (other DDL parsing errors etc.)
         */
        static a_util::result::Result isBinaryEqual(const std::string& type1, const DDLDescription* desc1,
                                     const std::string& type2, const DDLDescription* desc2,
                                     bool is_subset = true);

        // main ddl entities comparisons

        /**
         * @brief isEqualPrefix checks whether two prefix descriptions are equal.
         * @param [in] prefix1 The name of the first prefix.
         * @param [in] desc1 The description that has prefix1.
         * @param [in] prefix2 The name of the second prefix.
         * @param [in] desc2 The description that has prefix2.
         * @param [in] flags Flags that specifiy what should be checked, see @ref ItemCheckFlags.
         * @retval a_util::result::SUCCESS The descriptions are equal.
         * @retval ERR_FAILED The description are not equal.
         * @retval Standard result (other DDL parsing errors etc.)
         */
        static a_util::result::Result isEqualPrefix(const std::string& prefix1, const std::string& desc1,
                                     const std::string& prefix2, const std::string& desc2,
                                     uint32_t flags = icf_none);

        /**
         * @brief isEqual checks whether two prefix descriptions are equal.
         * @param [in] prefix1 The first prefix.
         * @param [in] prefix2 The second prefix.
         * @param [in] flags Flags that specifiy what should be checked, see @ref ItemCheckFlags.
         * @retval a_util::result::SUCCESS The descriptions are equal.
         * @retval ERR_FAILED The description are not equal.
         * @retval Standard result (other DDL parsing errors etc.)
         */
        static a_util::result::Result isEqual(const DDLPrefix* prefix1, const DDLPrefix* prefix2,
                               uint32_t flags = icf_none);

        /**
         * @brief isEqualUnit checks whether two unit descriptions are equal.
         * @param [in] unit1 The name of the first type.
         * @param [in] desc1 The description that has unit1.
         * @param [in] unit2 The name of the second type.
         * @param [in] desc2 The description that has unit2.
         * @param [in] flags Flags that specifiy what should be checked, see @ref ItemCheckFlags.
         * @retval a_util::result::SUCCESS The descriptions are equal.
         * @retval ERR_FAILED The description are not equal.
         * @retval Standard result (other DDL parsing errors etc.)
         */
        static a_util::result::Result isEqualUnit(const std::string& unit1, const std::string& desc1,
                                   const std::string& unit2, const std::string& desc2,
                                   uint32_t flags = icf_none);

        /**
         * @brief isEqual checks whether two unit descriptions are equal.
         * @param [in] unit1 The first unit.
         * @param [in] unit2 The second unit.
         * @param [in] flags Flags that specifiy what should be checked, see @ref ItemCheckFlags.
         * @retval a_util::result::SUCCESS The descriptions are equal.
         * @retval ERR_FAILED The description are not equal.
         * @retval Standard result (other DDL parsing errors etc.)
         */
        static a_util::result::Result isEqual(const IDDLUnit* unit1, const IDDLUnit* unit2,
                               uint32_t flags = icf_none);

        /**
         * @brief isEqual checks whether two type (POD/enum/struct) descriptions are equal.
         *        When this is true for a struct then it also implies @ref isBinaryEqual is true
         * @param [in] type1 The name of the first type.
         * @param [in] desc1 The description that has type1.
         * @param [in] type2 The name of the second type.
         * @param [in] desc2 The description that has type2.
         * @param [in] flags Flags that specifiy what should be checked, see @ref ItemCheckFlags.
         * @retval a_util::result::SUCCESS The descriptions are equal.
         * @retval ERR_FAILED The description are not equal.
         * @retval Standard result (other DDL parsing errors etc.)
         */
        static a_util::result::Result isEqualType(const std::string& type1, const std::string& desc1,
                                   const std::string& type2, const std::string& desc2,
                                   uint32_t flags = icf_memory);

        /**
         * @brief isEqual checks whether two type (POD/enum/struct) descriptions are equal
         *        When this is true for a struct then it also implies @ref isBinaryEqual is true
         * @param [in] type1 The first type.
         * @param [in] type2 The second type.
         * @param [in] flags Flags that specifiy what should be checked, see @ref ItemCheckFlags.
         * @retval a_util::result::SUCCESS The descriptions are equal.
         * @retval ERR_FAILED The description are not equal.
         * @retval Standard result (other DDL parsing errors etc.)
         */
        static a_util::result::Result isEqual(const IDDLDataType* type1, const IDDLDataType* type2,
                               uint32_t flags = icf_memory);

        /**
         * @brief isEqual checks whether two stream descriptions are equal.
         * @param [in] stream1 The name of the first stream.
         * @param [in] desc1 The description that has stream1.
         * @param [in] stream2 The name of the second stream.
         * @param [in] desc2 The description that has stream2.
         * @param [in] flags Flags that specifiy what should be checked, see @ref ItemCheckFlags.
         * @retval a_util::result::SUCCESS The descriptions are equal.
         * @retval ERR_FAILED The description are not equal.
         * @retval Standard result (other DDL parsing errors etc.)
         */
        static a_util::result::Result isEqualStream(const std::string& stream1, const std::string& desc1,
                                     const std::string& stream2, const std::string& desc2,
                                     uint32_t flags = icf_memory);

        /**
         * @brief isEqual checks whether two stream descriptions are equal.
         * @param [in] stream1 The first stream.
         * @param [in] stream2 The second stream.
         * @param [in] flags Flags that specifiy what should be checked, see @ref ItemCheckFlags.
         * @retval a_util::result::SUCCESS The descriptions are equal.
         * @retval ERR_FAILED The description are not equal.
         * @retval Standard result (other DDL parsing errors etc.)
         */
        static a_util::result::Result isEqual(const DDLStream* stream1, const DDLStream* stream2,
                               uint32_t flags = icf_memory);

        // sub-item comparison

        /**
         * @brief isEqual checks whether two external declarations are equal.
         * @param [in] ext1 The first declaration.
         * @param [in] ext2 The second declaration.
         * @param [in] flags Flags that specifiy what should be checked, see @ref ItemCheckFlags.
         * @retval a_util::result::SUCCESS The descriptions are equal.
         * @retval ERR_FAILED The description are not equal.
         * @retval Standard result (other DDL parsing errors etc.)
         */
        static a_util::result::Result isEqual(const DDLExtDeclaration* ext1, const DDLExtDeclaration* ext2,
                               uint32_t flags = icf_none);

        /**
         * @brief isEqual checks whether two reference units are equal.
         * @param [in] ref_unit1 The first unit.
         * @param [in] ref_unit2 The second unit.
         * @param [in] flags Flags that specifiy what should be checked, see @ref ItemCheckFlags.
         * @retval a_util::result::SUCCESS The descriptions are equal.
         * @retval ERR_FAILED The description are not equal.
         * @retval Standard result (other DDL parsing errors etc.)
         */
        static a_util::result::Result isEqual(const DDLRefUnit* ref_unit1, const DDLRefUnit* ref_unit2,
                               uint32_t flags = icf_none);

        /**
         * @brief isEqual checks whether two struct elements are equal.
         * Note that in case of dynamic elements only the name of the array size specifier can be checked.
         * Compare the whole struct if you need a more tourough comparison.
         * @param [in] elem1 The first element.
         * @param [in] elem2 The second element.
         * @param [in] flags Flags that specifiy what should be checked, see @ref ItemCheckFlags.
         * @retval a_util::result::SUCCESS The descriptions are equal.
         * @retval ERR_FAILED The description are not equal.
         * @retval Standard result (other DDL parsing errors etc.)
         */
        static a_util::result::Result isEqual(const DDLElement* elem1, const DDLElement* elem2,
                               uint32_t flags = icf_none);

        /**
         * @brief isEqual checks whether two stream structs equal.
         * @param [in] stream_struct1 The first struct.
         * @param [in] stream_struct2 The second struct.
         * @param [in] flags Flags that specifiy what should be checked, see @ref ItemCheckFlags.
         * @retval a_util::result::SUCCESS The descriptions are equal.
         * @retval ERR_FAILED The description are not equal.
         * @retval Standard result (other DDL parsing errors etc.)
         */
        static a_util::result::Result isEqual(const DDLStreamStruct* stream_struct1, const DDLStreamStruct* stream_struct2,
                               uint32_t flags = icf_none);

        // description based methods

        /**
         * @brief isEqual checks whether two descriptions are equal.
         * @param [in] desc1 The first description.
         * @param [in] desc2 The second description.
         * @param [in] flags Flags that specifiy what should be checked, see @ref DescriptionCheckFlags.
         * @retval a_util::result::SUCCESS The descriptions are equal.
         * @retval ERR_FAILED The description are not equal.
         * @retval Standard result (other DDL parsing errors etc.)
         */
        static a_util::result::Result isEqual(const std::string& desc1,
                               const std::string& desc2,
                               uint32_t flags = dcf_all | dcf_subset);

        /**
         * @brief isEqual checks whether two descriptions are equal.
         * @param [in] desc1 The first description.
         * @param [in] desc2 The second description.
         * @param [in] flags Flags that specifiy what should be checked, see @ref DescriptionCheckFlags.
         * @retval a_util::result::SUCCESS The descriptions are equal.
         * @retval ERR_FAILED The description are not equal.
         * @retval Standard result (other DDL parsing errors etc.)
         */
        static a_util::result::Result isEqual(const DDLDescription* desc1,
                               const DDLDescription* desc2,
                               uint32_t flags = dcf_all | dcf_subset);
};

}

#endif
