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

#ifndef DDL_IMPORTER_H_INCLUDED
#define DDL_IMPORTER_H_INCLUDED

#include "ddl_common.h"
#include "ddlfactorymethod_intf.h"
#include "ddlvisitor_intf.h"
#include "ddlcontainer.h"

namespace ddl
{
    class DDLDescription;

    /**
     * \enum ImporterMsgSeverity
     * enumeration type for message severity
     */
    typedef enum 
    {
        importer_info,
        importer_warning,
        importer_error
    } ImporterMsgSeverity;

    /**
     * \struct ImporterMsg
     * error description structure type
     */
    typedef struct
    {
        std::string      desc;    //!< string of the message
        ImporterMsgSeverity    severity;  //!< enumeration type for message severity
    } ImporterMsg;

    /**
     * Container for error descriptions
     */
    typedef std::list<ImporterMsg>  ImporterMsgList;


    /**
     * Concrete Factory Method for building up a DDL from a XML file.
     */
    class DDLImporter : public IDDLFactoryMethod
    {
    public:
        /**
         * Default CTOR
         * @param[in] basic_check Perform basic checks during Create* (unique element names etc.).
         *            For a full check use DDLInspector.
         * @param[in] sorted Sorts all DDL items by name for better performance.
         */
        DDLImporter(bool basic_check = true, bool sorted = true);

        /**
        * CTOR with file name/path
        * @param[in] file - Path to file to use
        * @param[in] basic_check Perform basic checks during Create* (unique element names etc.).
        *            For a full check use DDLInspector.
        * @param[in] sorted Sorts all DDL items by name for better performance.
        */
        DDLImporter(const a_util::filesystem::Path& file,
            bool basic_check = true,
            bool sorted = true);

        /**
        * CTOR with file name/path
        * @param[in] file - Path to file to use
        * @param[in] basic_check Perform basic checks during Create* (unique element names etc.).
        *            For a full check use DDLInspector.
        * @param[in] sorted Sorts all DDL items by name for better performance.
        */
        DDLImporter(const char* file, bool basic_check = true, bool sorted = true);

        /**
         * CTOR with creation level.
         * @param[in] creation_level - Creation level
         * @param[in] basic_check Perform basic checks during Create* (unique element names etc.).
         *            For a full check use DDLInspector.
         * @param[in] sorted Sorts all DDL items by name for better performance.
         */
        DDLImporter(int creation_level, bool basic_check = true, bool sorted = true);

        /// Default copy constructor
        DDLImporter(const DDLImporter&) = default;

        /**
         * Assignment operator (either copies or moves)
         * @param[in] other DDLImporter object to copy from
         * @return @c *this
         */
        DDLImporter& operator=(DDLImporter other);

        /**
         * Move CTOR
         * @param[in,out] other DDLImporter object to move from - empty but valid when finished
         */
        DDLImporter(DDLImporter&& other);

        /**
         * Add swap functionality, also enabling the copy-swap-idiom
         * @param[in,out] lhs Left-hand side ddl type
         * @param[in,out] rhs Right-hand side ddl type
         */
        friend void swap(DDLImporter& lhs, DDLImporter& rhs) noexcept;


    public:  // implements IDDLFactoryMethod

        virtual ~DDLImporter();

        DDLDescription * getDDL() const;

        /**
         * @copydoc IDDLFactoryMethod::createNew(double)
         * @remarks If an error occurs, a list of detailed error descriptions
         * can be obtained using \c GetAllErrors().
         */
        a_util::result::Result createNew(const DDLVersion& version = DDLVersion::ddl_version_invalid);

        void destroyDDL();

        a_util::result::Result buildHeader();

        a_util::result::Result buildUnits();

        a_util::result::Result buildDatatypes();

        a_util::result::Result buildStructs();

        a_util::result::Result buildStreams();

        a_util::result::Result buildEnums();

        a_util::result::Result buildStreamMetaTypes();

    public:
        /**
         * Static method to destroy the given DDL hierarchy.
         * @param [in] ddl - Pointer to the DDL hierarchy which should be destroyed.
         *
         * @return void
         */
        static void destroyDDL(DDLDescription *ddl);

        /**
         * Setter for the creation level.
         * @param[in] level - Creation level to use
         *
         * @return void
         */
        void setCreationLevel(int const level = 1);

        /**
         * Setter for the validation mode.
         * @param[in] full_check - Validation mode to use.
         * Set true for full description check.
         * Set false for description check until first error.
         *
         * @return void
         */
        void setFullCheckDescriptionMode(bool full_check = false);

        /**
         * Setter to enable disable the merge of DDL defaults (units, datattypes, etc.)
         * @param [in] merge_defaults See description
         * @return void
         */
        void setMergeDefaults(bool merge_defaults = true);

        /**
         * Setter to switch from prefereing entities from the reference DDL, to using the reference
         * enties only when there is no "local" aquivalent.
         * Mind that reference entities are not cloned but only referenced.
         * They are also not included into the containers.
         * @param [in] prefere_reference See description
         * @return void
         */
        void setPreferReferenceEntities(bool prefere_reference = true);

        /**
         * Method to create a partial DDL (e.g. streams only).
         * @param[in] ref_ddl - Reference DDL object to be used for resolving
         * matters
         * @param[in] version  The version the newly created description will have. Set 0 for
         *                      newst version.
         * @retval ERR_POINTER Null-pointer committed
         * @retval ERR_NOT_INITIALIZED Not yet initialized (see setter methods
         * in concrete classes, e.g. \c DDLImporter::setFile())
         * @retval ERR_UNKNOWN_FORMAT Expected XML hierarchy not found
         * @retval ERR_NO_CLASS Cross reference not resolvable (e.g. refUnit)
         * @retval ERR_UNKNOWN Cross reference has not been resolved
         */
        a_util::result::Result createPartial(const DDLDescription* ref_ddl, const DDLVersion& version);

        /**
        * Setter for the source file.
        * @param[in] file - Path to file to use
        * @retval ERR_OPEN_FAILED Reading process failed
        * @retval ERR_UNEXPECTED Invalid XML tag
        */
        a_util::result::Result setFile(const a_util::filesystem::Path& file);

        /**
         * Setter for a XML string containing the DDL to import.
         * @param[in] xml - String containing the DDL
         * @retval ERR_UNEXPECTED Invalid XML tag
         */
        a_util::result::Result setXML(const std::string& xml);

        /**
         * Getter for the description of the last error.
         * @return the error description
         */
        std::string getErrorDesc() const;

        /**
         * Getter for the most recent error description.
         * @return The error description or \c a_util::strings::empty_string if there was none.
         * @remarks This method indeed returns the last \b error message
         *          (message with severity \c importer_error).
         */
        std::string getLastErrorDesc() const;

        /**
         * Getter for all detected errors.
         * @return A list of all detected errors.
         */
        ImporterMsgList getAllMessages() const;

        /**
         * Method to log all messages which have at least the given severity
         * to console.
         * @param[in] least_severity - Minimum severity level (optional)
         *
         * @return void
         */
        void printAllMessages(ImporterMsgSeverity least_severity = importer_info);

    private:    // methods
        /**
         * Creates a baseunit object from the given DOMElement.
         * @param[out] new_bu - Pointer to pointer to the newly created
         * baseunit object
         * @param[in] bu_element - The DOMElement with the information
         * @retval ERR_UNKNOWN_FORMAT Expected XML hierarchy not found
         * @retval ERR_NO_CLASS Cross reference not resolvable
         */
        a_util::result::Result buildSingleBaseunit(DDLBaseunit ** new_bu,
                                    const a_util::xml::DOMElement& bu_element);

        /**
         * Creates a prefix object from the given DOMElement.
         * @param[out] new_prefix - Pointer to pointer to the newly created
         * prefix object
         * @param[in] prefix_element - The DOMElement with the information
         * @retval ERR_UNKNOWN_FORMAT Expected XML hierarchy not found
         * @retval ERR_NO_CLASS Cross reference not resolvable
         */
        a_util::result::Result buildSinglePrefix(DDLPrefix ** new_prefix,
                                  const a_util::xml::DOMElement& prefix_element);

        /**
         * Creates a unit object from the given DOMElement.
         * @param[out] new_unit - Pointer to pointer to the newly created
         * unit object
         * @param[in] unit_element - The DOMElement with the information
         * @retval ERR_UNKNOWN_FORMAT Expected XML hierarchy not found
         * @retval ERR_NO_CLASS Cross reference not resolvable
         */
        a_util::result::Result buildSingleUnit(DDLUnit ** new_unit,
                                    const a_util::xml::DOMElement& unit_element);

        /**
         * Creates a datatype object from the given DOMElement.
         * @param[out] new_datatype - Pointer to pointer to the newly
         * created datatype object
         * @param[in] dt_element -T he DOMElement with the information
         * @retval ERR_UNKNOWN_FORMAT Expected XML hierarchy not found
         * @retval ERR_NO_CLASS Cross reference not resolvable (e.g. unit)
         */
        a_util::result::Result buildSingleDatatype(DDLDataType ** new_datatype,
                                    const a_util::xml::DOMElement& dt_element);

        /**
         * Creates an enum object from the given DOMElement.
         * @param[out] new_enum - Pointer to pointer to the newly
         * created enum object
         * @param[in] oEnumElement - The DOMElement with the information
         * @retval ERR_UNKNOWN_FORMAT Expected XML hierarchy not found
         * @retval ERR_NO_CLASS Cross reference not resolvable (e.g. unit)
         */
        a_util::result::Result buildSingleEnum(DDLEnum** new_enum,
                                const a_util::xml::DOMElement& enum_element);

        /**
         * Creates a struct object from the given DOMElement.
         * @param[out] new_struct - Pointer to pointer to the newly created
         * struct object
         * @param[in] struct_element - The DOMElement with the information
         * @retval ERR_UNKNOWN_FORMAT Expected XML hierarchy not found
         * @retval ERR_NO_CLASS Cross reference not resolvable (e.g. datatype)
         * @retval ERR_UNKNOWN Not all firstly unknown structs have been
         * resolved
         */
        a_util::result::Result buildSingleStruct(DDLComplex ** new_struct,
                                  const a_util::xml::DOMElement& struct_element);

        /**
         * Creates a DDL stream object from the given DOMElement.
         * @param[out] new_stream - Pointer to pointer to the newly created
         * stream object
         * @param[in] stream_element - The DOMElement with the information
         * @retval ERR_UNKNOWN_FORMAT Expected XML hierarchy not found
         * @retval ERR_NO_CLASS Cross reference not resolvable (e.g. struct)
         */
        a_util::result::Result buildSingleStream(DDLStream ** new_stream,
                                  a_util::xml::DOMElement stream_element);

        /**
         * Creates a DDL stream object from the given DOMElement.
         * @param[out] stream_meta_type_element
         * @param[in] stream_meta_types
         * @retval ERR_UNKNOWN_FORMAT Expected XML hierarchy not found
         * @retval ERR_NO_CLASS Cross reference not resolvable (e.g. struct)
         */
        a_util::result::Result buildSingleStreamMetaType(const a_util::xml::DOMElement& stream_meta_type_element,
                                          DDLStreamMetaTypeVec& stream_meta_types);

        /**
         * Helper method to create a message and save it in the appropriate
         * container.
         * @param[in] msg - Message text
         * @param[in] severity - Message severity
         * @return void
         */
        void pushMessage(const std::string& msg, ImporterMsgSeverity severity);

        /**
         * Helper method to find specific elements inside the given first_data by Name.
         * If not found there have a look in second_data.
         * @tparam T - Representation object type (e.g. DDLDataType)
         * @param[in] name - Name of the element to find
         * @param[in] first_data - Vector of representation objects to
         * search through
         * @param[in] second_data - second Vector of representation objects to
         * search through
         * @return pointer to the found element or \c NULL if not found
         */
        template<typename T>
        T* tryToFind(const std::string& name,
                     DDLContainer<T>& first_data,
                     DDLContainer<T>& second_data);

    private:    // members
        a_util::xml::DOM _dom;
        DDLDescription *_ddl_desc;
        const DDLDescription *_current_ref_ddl;

        DDLUnitVec _unknown_units;
        DDLComplexVec _unknown_structs;
        DDLStreamMetaTypeVec _unknown_stream_meta_types;
        bool _init_flag;
        int _creation_level;

        bool _full_check;
        ImporterMsgList  _errors;
        bool _basic_check;
        bool _merge_defaults;
        bool _sorted;
        bool _prefere_reference;
    };

}   // namespace ddl

#endif  // DDL_IMPORTER_H_INCLUDED
