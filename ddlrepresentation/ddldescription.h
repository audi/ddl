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

#ifndef DDL_DESCRIPTION_H_INCLUDED
#define DDL_DESCRIPTION_H_INCLUDED

#include "ddl_common.h"
#include "ddl_type.h"
#include "ddlvisitor_intf.h"
#include "ddlcontainer.h"
#include "ddlversion.h"


namespace ddl
{    
    /**
     * Main class representing a whole DDL description
     */
    class DDLDescription : public DDL
    {
    public:
        /**
         * CTOR
         * @param[in] header - Pointer to Header object
         * @param[in] units - Vector of units (optional)
         * @param[in] baseunits - Vector of baseunits (optional)
         * @param[in] prefixes - Vector of prefixes (optional)
         * @param[in] dts - Vector of datatypes (optional)
         * @param[in] structs - Vector of complex datatypes (optional)
         * @param[in] streams - Vector of streams (optional)
         * @param[in] enums - Vector of enums (optional)
         * @param[in] merge_defaults Whether or not to merge DDL default elements (datatypes etc.).
         */
        DDLDescription(DDLHeader* header,
                        const DDLUnitVec& units = DDLUnitVec(),
                        const DDLBaseunitVec& baseunits = DDLBaseunitVec(),
                        const DDLPrefixVec& prefixes = DDLPrefixVec(),
                        const DDLDTVec& dts = DDLDTVec(),
                        const DDLComplexVec& structs = DDLComplexVec(),
                        const DDLStreamVec& streams = DDLStreamVec(),
                        const DDLEnumVec& enums = DDLEnumVec(),
                        bool merge_defaults = true);

        /**
         * Do not allow copying
         * @param[in] other - Reference to description object to copy
         */
        DDLDescription(const DDLDescription&) = delete;

        /**
         * Assignment operator (moves)
         * @param[in] other Description type to copy from
         * @return @c *this
         */
        DDLDescription& operator=(DDLDescription other);

        /**
         * Move CTOR
         * @param[in,out] other Complex DDL type to move from - empty but valid when finished
         */
        DDLDescription(DDLDescription&& other);

        /**
         * DTOR
         */
        virtual ~DDLDescription();

        a_util::result::Result accept(IDDLVisitor *visitor) const;
        a_util::result::Result accept(IDDLChangeVisitor *visitor);

        bool isInitialized() const;

        const std::string& getName() const;

        /**
         * Creation method to fill the object with data.
         * @param[in] header - Pointer to Header object
         * @param[in] units - Vector of units (optional)
         * @param[in] baseunits - Vector of baseunits (optional)
         * @param[in] prefixes - Vector of prefixes (optional)
         * @param[in] dts - Vector of datatypes (optional)
         * @param[in] structs - Vector of complex datatypes (optional)
         * @param[in] streams - Vector of streams (optional)
         * @param[in] enums - Vector of enums (optional)
         * @retval ERR_POINTER Null-pointer committed
         */
        a_util::result::Result create(DDLHeader* header,
                        DDLUnitVec units = DDLUnitVec(),
                        DDLBaseunitVec baseunits = DDLBaseunitVec(),
                        DDLPrefixVec prefixes = DDLPrefixVec(),
                        DDLDTVec dts = DDLDTVec(), 
                        DDLComplexVec structs = DDLComplexVec(),
                        DDLStreamVec streams = DDLStreamVec(),
                        DDLEnumVec enums = DDLEnumVec());

        /**
         * Getter for the DDL header.
         * @return the header
         */
        DDLHeader * getHeader() const;

        /**
         * Setter for the units.
         * @param[in] units - Vector of unit objects
         *
         * @return void
         */
        void cloneUnits(const DDLUnitVec& units);

        /**
         * Setter for the units.
         * @param[in] units - Vector of unit objects
         *
         * @return void
         */
        void refUnits(DDLUnitVec& units);

        /**
         * Merger for the units.
         * @param[in] units - Vector of unit objects
         * @param[in] join_flags - Additional flags for join/merge process (optional)
         * @param[in] delete_data - deletion vector for optimezed deletion
         *
         * @return void
         */
        void mergeUnits(DDLUnitVec units, uint32_t join_flags = 0, DDLVec* delete_data = NULL);

        /**
        * Adder for an unit object.
        * @param[in] unit - Pointer to the unit object to add
        * @param[in] pos   - Position to add the unit object
        *
        * @return void
        */
        void addUnit(DDLUnit* unit, int pos = -1);

        /**
        * Getter for the units.
        * @return vector of units
        */
        DDLUnitVec& getUnits();

        /**
        * Getter for the units.
        * @return vector of units
        */
        const DDLUnitVec& getUnits() const;


        /**
         * Getter for a specific unit object.
         * @param[in] name - Name of the unit to look for
         * @return pointer to the unit object or \c NULL if not found
         */
        const DDLUnit* getUnitByName(const std::string& name) const;

        /**
         * Getter for a specific unit object.
         * @param[in] name - Name of the unit to look for
         * @return pointer to the unit object or \c NULL if not found
         */
        DDLUnit* getUnitByName(const std::string& name);

        /**
         * Setter for the baseunits.
         * @param[in] baseunits - Vector of baseunit objects
         *
         * @return void
         */
        void cloneBaseunits(const DDLBaseunitVec& baseunits);

        /**
         * Setter for the baseunits.
         * @param[in] baseunits - Vector of baseunit objects
         *
         * @return void
         */
        void refBaseunits(DDLBaseunitVec& baseunits);

        /**
         * Merger for the baseunits.
         * @param[in] baseunits - Vector of baseunit objects
         * @param[in] join_flags - Additional flags for join/merge process (optional)
         * @param[in, out] delete_data - temporary vector for data that can be deleted afterwards.
         *
         * @return void
         */
        void mergeBaseunits(DDLBaseunitVec baseunits, uint32_t join_flags = 0, DDLVec* delete_data = NULL);

        /**
        * Adder for a baseunit object.
        * @param[in] baseunit - Pointer to the baseunit object to add
        * @param[in] pos       - Position to add the baseunit object
        *
        * @return void
        */
        void addBaseunit(DDLBaseunit* baseunit, int pos = -1);

        /**
        * Getter for the baseunits.
        * @return vector of baseunits
        */
        DDLBaseunitVec getBaseunits() const;

        /**
         * Getter for a specific baseunit object.
         * @param[in] name - Name of the baseunit to look for
         * @return pointer to the baseunit object or \c NULL if not found
         */
        const DDLBaseunit* getBaseunitByName(const std::string& name) const;

        /**
         * Getter for a specific baseunit object.
         * @param[in] name - Name of the baseunit to look for
         * @return pointer to the baseunit object or \c NULL if not found
         */
        DDLBaseunit* getBaseunitByName(const std::string& name);

        /**
         * Setter for the prefixes.
         * @param[in] prefixes - Vector of prefix objects
         *
         * @return void
         */
        void clonePrefixes(const DDLPrefixVec& prefixes);

        /**
         * Setter for the prefixes.
         * @param[in] prefixes - Vector of prefix objects
         *
         * @return void
         */
        void refPrefixes(DDLPrefixVec& prefixes);

        /**
         * Merger for the prefixes.
         * @param[in] prefixes - Vector of prefix objects
         * @param[in] join_flags - Additional flags for join/merge process (optional)
         * @param[in, out] delete_data - temporary vector for data that can be deleted afterwards.
         *
         * @return void
         */
        void mergePrefixes(DDLPrefixVec prefixes, uint32_t join_flags = 0, DDLVec* delete_data = NULL);

        /**
         * Adder for a prefix object
         * @param[in] prefix - Pointer to the prefix object to add
         * @param[in] pos     - Position to add the prefix object
         *
         * @return void
         */
        void addPrefix(DDLPrefix* prefix, int pos = -1);

        /**
         * Getter for the prefixes
         * @return vector of prefixes
         */
        DDLPrefixVec getPrefixes() const;

        /**
         * Getter for a specific prefix object.
         * @param[in] name - Name of the prefix to look for
         * @return pointer to the prefix object or \c NULL if not found
         */
        DDLPrefix* getPrefixByName(const std::string& name);

        /**
         * Getter for a specific prefix object.
         * @param[in] name - Name of the prefix to look for
         * @return pointer to the prefix object or \c NULL if not found
         */
        const DDLPrefix* getPrefixByName(const std::string& name) const;

        /**
         * Setter for the data types.
         * @param[in] data_types - Vector of datatypes
         *
         * @return void
         */
        void cloneDatatypes(const DDLDTVec& data_types);

        /**
         * Setter for the data types.
         * @param[in] data_types - Vector of datatypes
         *
         * @return void
         */
        void refDatatypes(DDLDTVec& data_types);

        
        /**
         * Setter for the constants.
         * @param[in] enums - Vector of enums
         *
         * @return void
         */
        void refEnums(DDLEnumVec& enums);

         /**
         * Merger for the constants.
         * @param[in] enums - Vector of constants
         * @param[in] join_flags - Additional flags for join/merge process (optional)
         * @param[in, out] delete_data - temporary vector for data that can be deleted afterwards.
         *
         * @return void
         */
        void mergeEnums(DDLEnumVec enums, uint32_t join_flags = 0, DDLVec* delete_data = NULL);

        /**
         * Setter for the enums.
         * @param[in] enumss - Vector of enums
         *
         * @return void
         */
        void cloneEnums(const DDLEnumVec& enums);

        /**
        * Adder for a enum object.
        * @param[in] ddl_enum - Pointer to the enum object to add
        * @param[in] pos   - Position to add the enum object
        *
        * @return void
        */
        void addEnum(DDLEnum* ddl_enum, int pos = -1);

        /**
         * Getter for a specific enum object.
         * @param[in] name - Name of the enum to look for
         * @return pointer to the enum object or \c NULL if not found
         */
        DDLEnum* getEnumByName(const std::string& name);

        /**
         * Getter for a specific enum object.
         * @param[in] name - Name of the enum to look for
         * @return pointer to the enum object or \c NULL if not found
         */
        const DDLEnum* getEnumByName(const std::string& name) const;

        /**
         * Merger for the data types.
         * @param[in] data_types - Vector of datatypes
         * @param[in] join_flags - Additional flags for join/merge process (optional)
         * @param[in, out] delete_data - temporary vector for data that can be deleted afterwards.
         *
         * @return void
         */
        void mergeDatatypes(DDLDTVec data_types, uint32_t join_flags = 0, DDLVec* delete_data = NULL);

        /**
        * Adder for a datatype object.
        * @param[in] datatype - Pointer to the datatype object to add
        * @param[in] pos       - Position to add the datatype object
        *
        * @return void
        */
        void addDatatype(DDLDataType* datatype, int pos = -1);

        /**
        * Getter for the datatypes.
        * @return vector of datatypes
        */
        DDLDTVec& getDatatypes();

        /**
        * Getter for the datatypes.
        * @return vector of datatypes
        */
        const DDLDTVec& getDatatypes() const;

        /**
        * Getter for the enums.
        * @return vector of enums
        */
        DDLEnumVec& getEnums();

        /**
        * Getter for the enums.
        * @return vector of enums
        */
        const DDLEnumVec& getEnums() const;

        /**
         * Getter for a specific data-type object.
         * @param[in] name - Name of the data type to look for
         * @return pointer to the data-type object or \c NULL if not found
         */
        DDLDataType* getDataTypeByName(const std::string& name);

        /**
         * Getter for a specific data-type object.
         * @param[in] name - Name of the data type to look for
         * @return pointer to the data-type object or \c NULL if not found
         */
        const DDLDataType* getDataTypeByName(const std::string& name) const;

        /**
         * Setter for the complex datatypes.
         * @param[in] structs - Vector of complex datatypes
         *
         * @return void
         */
        void cloneStructs(const DDLComplexVec& structs);

       /**
         * Setter for the complex datatypes.
         * @param[in] structs - Vector of complex datatypes
         *
         * @return void
         */
        void refStructs(DDLComplexVec& structs);

        /**
         * Merger for the complex datatypes.
         * @param[in] structs - Vector of complex datatypes
         * @param[in] join_flags - Additional flags for join/merge process (optional)
         * @param[in, out] delete_data - temporary vector for data that can be deleted afterwards.
         *
         * @return void
         */
        void mergeStructs(DDLComplexVec structs, uint32_t join_flags = 0, DDLVec* delete_data = NULL);

        /**
        * Adder for a complex datatype object.
        * @param[in] ddl_struct - Pointer to the complex datatype object to add
        * @param[in] pos     - Position to add the complex datatype object
        *
        * @return void
        */
        void addStruct(DDLComplex *ddl_struct, int pos = -1);

        /**
        * Getter for the boolean identifying dynamic elements in structures.
        * @return true if dynamic elements were found in at least one structure
        */
        bool hasDynamicStructs();

        /**
        * Getter for the complex datatypes.
        * @return vector of complex datatypes
        */
        DDLComplexVec& getStructs();

        /**
        * Getter for the complex datatypes.
        * @return vector of complex datatypes
        */
        const DDLComplexVec& getStructs() const;

        /**
         * Getter for a specific struct object.
         * @param[in] name - Name of the struct to look for
         * @return pointer to the struct object or \c NULL if not found
         */
        const DDLComplex * getStructByName(const std::string& name) const;

        /**
         * Getter for a specific struct object.
         * @param[in] name - Name of the struct to look for
         * @return pointer to the struct object or \c NULL if not found
         */
        DDLComplex* getStructByName(const std::string& name);

        /**
         * Removes the specified primitive or complex data-type.
         * @param[in] name - Name of the type to remove
         * @retval ERR_NOT_FOUND The specified data type was not found
         * @attention Be careful with the removal of data types! There could
         * be some streams which depend on the removed type! This possible
         * dependency is not checked!
         */
        a_util::result::Result removeType(const std::string& name);

        /**
         * Removes the specified primitive data-type.
         * @param[in] name - Name of the type to remove
         * @retval ERR_NOT_FOUND The specified data type was not found
         * @attention Be careful with the removal of data types! There could
         * be some streams which depend on the removed type! This possible
         * dependency is not checked!
         */
        a_util::result::Result removeDataType(const std::string& name);

        /**
         * Removes the specified complex data-type.
         * @param[in] name - Name of the type to remove
         * @retval ERR_NOT_FOUND The specified data type was not found
         * @attention Be careful with the removal of data types! There could
         * be some streams which depend on the removed type! This possible
         * dependency is not checked!
         */
        a_util::result::Result removeComplexDataType(const std::string& name);

        /**
         * Method moves the specified external header declaration.
         * @param[in] from - Current element position in the list
         * @param[in] to - New element position in the list
         * @retval ERR_OUT_OF_RANGE Input parameters are out of range.
         */
        a_util::result::Result moveHeaderExcDec(const int from, const int to);

        /**
         * Method moves the specified base unit.
         * @param[in] from - Current element position in the list
         * @param[in] to - New element position in the list
         * @retval ERR_OUT_OF_RANGE Input parameters are out of range.
         */
        a_util::result::Result moveBaseunit(const int from, const int to);

        /**
         * Method moves the specified prefix.
         * @param[in] from - Current element position in the list
         * @param[in] to - New element position in the list
         * @retval ERR_OUT_OF_RANGE Input parameters are out of range.
         */
        a_util::result::Result movePrefix(const int from, const int to);

        /**
         * Method moves the specified unit.
         * @param[in] from - Current element position in the list
         * @param[in] to - New element position in the list
         * @retval ERR_OUT_OF_RANGE Input parameters are out of range.
         */
        a_util::result::Result moveUnit(const int from, const int to);

        /**
         * Method moves the specified element within a unit.
         * @param[in] from - Current element position in the list 
         * @param[in] to - New element position in the list
         * @param[in] parent - Unit name
         * @retval ERR_OUT_OF_RANGE Input parameters are out of range.
         */
        a_util::result::Result moveRefUnit(const int from,
                            const int to,
                            const std::string& parent);

        /**
         * Method moves the specified enum-type.
         * @param[in] from - Current element position in the list
         * @param[in] to - New element position in the list
         * @retval ERR_OUT_OF_RANGE Input parameters are out of range.
         */
        a_util::result::Result moveEnum(const int from, const int to);

        /**
         * Method moves the specified enum element.
         * @param[in] from - Current element position in the list 
         * @param[in] to - New element position in the list
         * @param[in] parent - Name of parent element
         * @retval ERR_OUT_OF_RANGE Input parameters are out of range.
         */
        a_util::result::Result moveEnumElement(const int from,
                                const int to,
                                const std::string& parent);

        /**
         * Method moves the specified data-type.
         * @param[in] from - Current element position in the list
         * @param[in] to - New element position in the list
         * @retval ERR_OUT_OF_RANGE Input parameters are out of range.
         */
        a_util::result::Result moveDatatype(const int from, const int to);

        /**
         * Method moves the specified complex data-type.
         * @param[in] from - Current element position in the list 
         * @param[in] to - New element position in the list
         * @retval ERR_OUT_OF_RANGE Input parameters are out of range.
         */
        a_util::result::Result moveComplexDataType(const int from,
                                    const int to);

        /**
         * Method moves the specified element within complex data-type.
         * @param[in] from - Current element position in the list 
         * @param[in] to - New element position in the list
         * @param[in] parent - Name of parent element
         * @retval ERR_OUT_OF_RANGE Input parameters are out of range.
         */
        a_util::result::Result moveComplexDataTypeElement(const int from,
                                           const int to,
                                           const std::string& parent);

        /**
         * Method moves the specified stream.
         * @param[in] from - Current element position in the list
         * @param[in] to - New element position in the list
         * @retval ERR_OUT_OF_RANGE Input parameters are out of range.
         */
        a_util::result::Result moveStream(const int from, const int to);

        /**
         * Removes the specified BaseUnit type.
         * @param[in] name - Name of the baseunit to remove
         * @retval ERR_NOT_FOUND The specified base was not found
         * @attention Be careful with the removal of data types! There could
         * be some streams which depend on the removed unit! This possible
         * dependency is not checked!
         */
        a_util::result::Result removeBaseunit(const std::string& name);

        /**
         * Removes the specified Unit type.
         * @param[in] name - Name of the prefix to remove
         * @retval ERR_NOT_FOUND The specified base was not found
         * @attention Be careful with the removal of data types! There could
         * be some streams which depend on the removed unit! This possible
         * dependency is not checked!
         */
        a_util::result::Result removePrefix(const std::string& name);

        /**
         * Removes the specified Unit type.
         * @param[in] name - Name of the unit to remove
         * @retval ERR_NOT_FOUND The specified base was not found
         * @attention Be careful with the removal of data types! There could
         * be some streams which depend on the removed unit! This possible
         * dependency is not checked!
         */
        a_util::result::Result removeUnit(const std::string& name);

        /**
         * Removes the specified enum.
         * @param[in] name - Name of the enum to remove
         * @retval ERR_NOT_FOUND The specified enum was not found
         * @attention Be careful with the removal of enums! There could
         * be some streams which depend on the removed type! This possible
         * dependency is not checked!
         */
        a_util::result::Result removeEnum(const std::string& name);


        /**
         * Setter for the streams.
         * @param[in] streams - Vector of streams
         *
         * @return void
         */
        void cloneStreams(const DDLStreamVec& streams);

        /**
         * Setter for the streams.
         * @param[in] streams - Vector of streams
         *
         * @return void
         */
        void refStreams(DDLStreamVec& streams);

        /**
         * Merger for the streams.
         * @param[in] streams - Vector of stream objects
         * @param[in] join_flags - Additional flags for join/merge process (optional)
         * @param[in, out] delete_data - temporary vector for data that can be deleted afterwards.
         *
         * @return void
         */
        void mergeStreams(DDLStreamVec streams, uint32_t join_flags = 0, DDLVec* delete_data = NULL);
        
        /**
        * Adder for a stream object.
        * @param[in] stream - Pointer to the stream object to add
        * @param[in] pos     - Position to add the stream object
        *
        * @return void
        */
        void addStream(DDLStream* stream, int pos = -1);

        /**
        * Getter for the streams.
        * @return vector of streams
        */
        DDLStreamVec& getStreams();

        /**
        * Getter for the streams.
        * @return vector of streams
        */
        const DDLStreamVec& getStreams() const;

        /**
         * Getter for a specific stream object.
         * @param[in] name - Name of the stream to look for
         * @return pointer to the stream object or \c NULL if not found
         * @remarks As the name attribute is optional only streams with
         * originally defined name can be found.
         */
        DDLStream * getStreamByName(const std::string& name);

        /**
         * Getter for a specific stream object.
         * @param[in] name - Name of the stream to look for
         * @return pointer to the stream object or \c NULL if not found
         * @remarks As the name attribute is optional only streams with
         * originally defined name can be found.
         */
        const DDLStream * getStreamByName(const std::string& name) const;

        /**
         * Renames the specified stream.
         * @param[in] old_name - Current name of the stream to rename
         * @param[in] new_name - New name of the stream
         * @retval ERR_NOT_FOUND The specified stream was not found
         * @remarks As the name attribute is optional only streams with
         * originally defined name can be found.
         */
        a_util::result::Result renameStream(const std::string& old_name, const std::string& new_name);

        /**
         * Removes the specified stream from the description.
         * @param[in] name - Name of the stream to remove
         * @retval ERR_NOT_FOUND The specified stream was not found
         * @remarks As the name attribute is optional only streams with
         * originally defined name can be found.
         */
        a_util::result::Result removeStream(const std::string& name);

        /**
         * Setter for the streams.
         * @param[in] streams - Vector of streams
         *
         * @return void
         */
        void cloneStreamMetaTypes(const DDLStreamMetaTypeVec& streams);

        /**
         * Setter for the streams.
         * @param[in] streams - Vector of streams
         *
         * @return void
         */
        void refStreamMetaTypes(DDLStreamMetaTypeVec& streams);

        /**
         * Merger for the streams.
         * @param[in] streams - Vector of stream objects
         * @param[in] join_flags - Additional flags for join/merge process (optional)
         * @param[in, out] delete_data - temporary vector for data that can be deleted afterwards.
         *
         * @return void
         */
        void mergeStreamMetaTypes(DDLStreamMetaTypeVec streams, uint32_t join_flags = 0, DDLVec* delete_data = NULL);

        /**
        * Adder for a stream object.
        * @param[in] stream - Pointer to the stream object to add
        * @param[in] pos     - Position to add the stream object
        *
        * @return void
        */
        void addStreamMetaType(DDLStreamMetaType* stream, int pos = -1);

        /**
        * Getter for the streams.
        * @return vector of streams
        */
        DDLStreamMetaTypeVec& getStreamMetaTypes();

        /**
        * Getter for the streams.
        * @return vector of streams
        */
        const DDLStreamMetaTypeVec& getStreamMetaTypes() const;

        /**
         * Getter for a specific stream object.
         * @param[in] name - Name of the stream to look for
         * @return pointer to the stream object or \c NULL if not found
         * @remarks As the name attribute is optional only streams with
         * originally defined name can be found.
         */
        DDLStreamMetaType * getStreamMetaTypeByName(const std::string& name);

        /**
         * Getter for a specific stream object.
         * @param[in] name - Name of the stream to look for
         * @return pointer to the stream object or \c NULL if not found
         * @remarks As the name attribute is optional only streams with
         * originally defined name can be found.
         */
        const DDLStreamMetaType * getStreamMetaTypeByName(const std::string& name) const;

        /**
         * Renames the specified stream.
         * @param[in] old_name - Current name of the stream to rename
         * @param[in] new_name - New name of the stream
         * @retval ERR_NOT_FOUND The specified stream was not found
         * @remarks As the name attribute is optional only streams with
         * originally defined name can be found.
         */
        a_util::result::Result renameStreamMetaType(const std::string& old_name, const std::string& new_name);

        /**
         * Removes the specified stream from the description.
         * @param[in] name - Name of the stream to remove
         * @retval ERR_NOT_FOUND The specified stream was not found
         * @remarks As the name attribute is optional only streams with
         * originally defined name can be found.
         */
        a_util::result::Result removeStreamMetaType(const std::string& name);

        /**
         * Merges another DDL ito this one.
         * @param[in] ddl - Reference to DDL object to merge ito this object
         * @param[in] join_flags - Additional flags for join/merge process (optional)
         * @retval ERR_INVALID_VERSION Incompatible DDL descriptions detected (not the same DDL version)
         */
        a_util::result::Result merge(DDLDescription &ddl, uint32_t join_flags = 0);

        /**
         * Deprecated, see IMediaDescriptionStack for the mechanism provided by the Media
         * Description Service that superseeds this functionality.
         *
         * Method to remove all representation objects of the specified level
         * and above.
         * @param[in] level - Creation level from which all representation
         * objects should be removed
         * @retval ERR_INVALID_HANDLE Validation of remaining DDL failed
         */
        a_util::result::Result restoreLevel (int const level);

        /**
         * Method to create a default DDL description.
         * @param [in] creation_level Creation level to use (optional)
         * @param [in] sorted        Sorts all DDL items by name for better performance. (optional)
         * @return pointer to the created description
         * @attention The caller/user has the responsibility for the created
         * DDL object! Especially take this aspect ito consideration in
         * matters of the deallocation of memory!
         */
        static DDLDescription * createDefault(int const creation_level = 1, bool sorted = true);

        /**
         * Overloaded method.
         * @copydoc DDLDescription::createDefault
         * @param [in] version  The version of the description. Pass 0 to get the newest version.
         * @return pointer to the created description
         */
        static DDLDescription * createDefault(const DDLVersion& version, int const creation_level = 1, bool sorted = true);

        /**
         * Add swap functionality, also enabling the copy-swap-idiom
         * @param[in,out] lhs Left-hand side ddl description
         * @param[in,out] rhs Right-hand side ddl description
         */
        friend void swap(DDLDescription& lhs, DDLDescription& rhs) noexcept;

    private:    // methods
        /**
        * Hidden default CTOR
        */
        DDLDescription() = default;

        /**
         * Helper method to merge a vector of representation objects ito the
         * according vector inside another DDL description.
         * @tparam T - Representation object type (e.g. DDLUnit)
         * @param[in] member_data - Vector of objects to merge ito
         * @param[in] other_data - Vector of objects to merge
         * @param[in] join_flags - Additional flags for join/merge process (optional)
         * @param[in] delete_data_mem - pointer to Vector of Object that can be deleted
         * @param[in] desc
         * @param[in] refs - Add references or clone
         * @param[in] delete_non_overwritten - default value is false
         * @retval true if member_data was updated
         * @retval false otherwise
         */
        template<typename T>
        static bool mergePart(DDLContainer<T> &member_data,
                               DDLContainer<T> &other_data,
                               uint32_t join_flags,
                               DDLVec* delete_data_mem,
                               DDLDescription* desc,
                               const bool& refs,
                               const bool& delete_non_overwritten = false);
        static void copyMinMaxValues(DDLDataType* dest_container, DDLDataType* src_container);

    private:    // members
        DDLHeader * _header;
        DDLUnitVec _units;
        DDLBaseunitVec _baseunits;
        DDLPrefixVec _prefixes;
        DDLDTVec _data_types;
        DDLEnumVec _enums;
        DDLComplexVec _structs;
        DDLStreamVec _streams;
        DDLStreamMetaTypeVec _stream_meta_types;
        bool _init_flag;
        bool _merge_defaults;
    };

        /**
     * This class template provides a auto deletion for DDL vector content like DDLEnumVec, DDLUnitVec etc.
     * @param [in] VECTOR_TYPE  The Vector Type class name the auto deletion of the vector content is performed.
     */
    template < class VECTOR_TYPE > class DDLAutoVec
    {
        private:
            std::vector < VECTOR_TYPE* >* _sync_vec;
        public:
            /**
             * CTOR.
             * @param [in] sync_vec pointer to the vector is auto delete.
             */
            DDLAutoVec(std::vector< VECTOR_TYPE* >* sync_vec)
            {
                set(sync_vec);
            };

            /**
             * Copy CTOR
             * @param[in] other DDL datatype object to copy from
             */
            DDLAutoVec(const DDLAutoVec& other) = default;

            /**
             * Assignment operator (either copies or moves)
             * @param[in] other DDL datatype object to copy from
             * @return @c *this
             */
            DDLAutoVec& operator= (DDLAutoVec other)
            {
                swap(*this, other);
                return *this;
            }

            /**
             * Move CTOR
             * @param[in,out] other DDL datatype object to move from - empty but valid when finished
             */
            DDLAutoVec(DDLAutoVec&& other)
            {
                swap(*this, other);
            }

            /**
             * DTOR.
             */
            ~DDLAutoVec()
            {
                autoDelete();
            };
            /**
             * Reset the DDL vector pointer.
             * @remark no deletion will be made at this time
             *
             * @return void
             */
            void reset()
            {
                _sync_vec = NULL;
            }
            /**
             * Reset the DDL vector pointer and set the given one.
             * @remark no deletion will be made at this time.
             * @param [in] sync_vec pointer to the vector is auto delete.
             *
             * @return void
             */
            void set(std::vector< VECTOR_TYPE* >* sync_vec)
            {
                reset();
                _sync_vec = sync_vec;
            }

            /**
             * Add swap functionality, also enabling the copy-swap-idiom
             * @param[in,out] lhs Left-hand side ddl type
             * @param[in,out] rhs Right-hand side ddl type
             */
            friend void swap(DDLAutoVec& lhs, DDLAutoVec& rhs)
            {
                using std::swap;
                swap(lhs._sync_vec, rhs._sync_vec);
            }

        private:
            /**
             * Deletes the current set sync pointer to an DDL Vector
             */
            void autoDelete()
            {
                if (_sync_vec)
                {
                    std::transform(_sync_vec->begin(), _sync_vec->end(), _sync_vec->begin(), ddl::DDLDescription::deleteChild< VECTOR_TYPE > );
                }
            }
    };

}   // namespace ddl

#endif  // DDL_DESCRIPTION_H_INCLUDED
