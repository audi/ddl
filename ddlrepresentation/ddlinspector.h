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

#ifndef DDL_INSPECTOR_H_INCLUDED
#define DDL_INSPECTOR_H_INCLUDED

#include "ddl_common.h"
#include "ddlvisitor_intf.h"
#include "ddlimporter.h"
#include "ddlversion.h"

namespace ddl
{
    enum WarningLevel
        /* see "rules for changing an enumeration" (#27200) before doing any change! */
    {
        /**
        * Inform about all warnings
        */
        verbose = 0,
        /**
        * Suppress some minor warnings
        */
        moderate = 1,
        /**
        * Suppress all warnings
        */
        silent = 2
    };

    /// Maximum depth of the hierarchy of a complex data type in a DDL File
    #define DDL_MAX_DESC_HIERARCHY 100

     /**
     * Validation and correction class for ADTF DDL for use with the 
     */
    class DDLInspector : public IDDLChangeVisitor
    {
    public:
        /**
         * CTOR.
         * @param[in] auto_correct - Flag for implicit auto-correction (optional)
         */
        DDLInspector(bool auto_correct = false);

        virtual a_util::result::Result visitDDL(DDLDescription* description);
        virtual a_util::result::Result visit(DDLHeader* header);
        virtual a_util::result::Result visit(DDLDataType* data_type);
        virtual a_util::result::Result visit(DDLComplex* complex);
        virtual a_util::result::Result visit(DDLStream* stream);
        virtual a_util::result::Result visit(DDLUnit* unit);
        virtual a_util::result::Result visit(DDLBaseunit* baseunit);
        virtual a_util::result::Result visit(DDLExtDeclaration* ext_declaration);
        virtual a_util::result::Result visit(DDLElement* element);
        virtual a_util::result::Result visit(DDLPrefix* prefix);
        virtual a_util::result::Result visit(DDLRefUnit* ref_unit);
        virtual a_util::result::Result visit(DDLStreamStruct* stream_struct);
        virtual a_util::result::Result visit(DDLEnum* ddl_enum);
        virtual a_util::result::Result visit(DDLStreamMetaType* stream_meta_type);
        virtual a_util::result::Result visit(DDLProperty* property);

        /**
         * Setter for the auto-correction flag.
         * @param[in] auto_correct - New value of the flag
         * @return void
         */
        void setAutoCorrect(bool auto_correct);

        /**
         * Getter for the auto-correction flag.
         * @returns the flag
         */
        bool getAutoCorrect() const;

        /**
        * Setter for the auto-correction flag.
        * @param[in] auto_correct - New value of the flag
        * @return void
        */
        void setWarningLevel(WarningLevel warning_level);

        /**
        * Getter for the auto-correction flag.
        * @returns the flag
        */
        WarningLevel getWarningLevel() const;

        /**
         * Getter for the list of suggestions.
         * @returns the list of suggestions
         */
        ImporterMsgList getSuggestions() const;

        /**
         * Getter for the most recent error description.
         * @return The error description or \c a_util::strings::empty_string if there was none.
         * @remarks This method indeed returns the last \b error message (message with severity \c importer_error).
         */
        std::string getLastErrorDesc() const;

        /**
        * Init a new Check, clear all list and init all variables.
        */
        void InitNewCheck();

        /**
        * Check if the description contains dynamic arrays
        *  @retval true if a dynamic array was found
        */
        bool foundDynamicArrays();

        /**
        * Returns the last corrected Bytepos position inclusive the 
        * last Element size.
        *  @param[in] struct_name - name of the Struct where you want the last element
        *  @return Last corrected Bytepos position inclusive the last Element size.
        */
        unsigned int getLastBytePosOfAStructCorrected(const std::string& struct_name);

        /**
        * Returns the actual last Bytepos inclusive the last Element size.
        *  @param[in] struct_name - name of the Struct where you want the last element
        *  @return Actual last Bytepos inclusive the last Element size.
        */
        unsigned int getLastBytePosOfAStructReal(const std::string& struct_name);

        /**
        * Check the validy of nested structs.
        *  @param[in] description - pointer of the Description within you want to check a vality of nested structs
        *  @retval Error message list
        */
        static ImporterMsgList checkValidyOfNestedStructs(const DDLDescription* description);

    protected:
        /**
         * Method for derived classes to add an own suggestion.
         * @param [in] desc Suggestion message
         * @param [in] severity Message severity
         * @retval ERR_EMPTY The suggestion message must not be empty.
         * @retval ERR_INVALID_ARG The message severity must have a valid value
         * of type \c adtf::ImporterMsgSeverity.
         */
        a_util::result::Result addSuggestion(std::string const &desc,
            ImporterMsgSeverity const severity);

        /**
         * Helper method to check if the element is alignable.
         * @param[in] byte_pos - Current byte position inside the structure
         * @param[in] element_size - Size of the element
         * @retval true Element is alignable
         * @retval false Element is not alignable
         */
        virtual bool isAlignable(size_t const byte_pos,
            size_t const element_size);

        /**
         * Helper method which returns whether the left-hand-side argument
         * compares less than the right-hand-side one (as returned by operator <)
         * based on given DDL type information.
         *
         * @param [in] lhs String of the left-hand-side
         * @param [in] rhs String of the right-hand-side
         * @param [in] type Pointer to common type information
         *
         * @retval true LHS is lower than RHS
         * @retval false LHS is not lower than RHS or type is not interpretable
         */
        static bool lessDDL (const std::string& lhs,
            const std::string& rhs,
            const DDLDataType* type);
        
    private:    // methods
        /**
        * Recursive helper function for check of the validy of nested structs within a struct.
        *  @param[in] description - pointer of the Description within you want to check a vality of a struct
        *  @param[in] struct_type_name - name of the Struct where you want to check for invalid nested structs
        *  @param[in] nested_struct_type_name - name of the nested Struct
        *  @param[out] log - Log of error message, if validy is failed
        *  @param[in] struct_depth - Depth of the nested stuct
        *  @retval true if valid
        *  @retval false otherwise
        */
        static bool checkValidyOfNestedStructs(const DDLDescription* description,
            const std::string& struct_type_name, 
            const std::string& nested_struct_type_name, 
            std::string& log, 
            int struct_depth = 1);
       
    private:    // members
        DDLDescription*               _ddl_desc;
        bool                          _auto_correct;
        /**
        * The list contains all warnings and errors.
        */
        ImporterMsgList               _suggestions;

        /**
        * The set contains all names of the structs.
        * If the list contains two equal names, it will be set
        * an error in the ImporterMsgList.
        */
        std::set<std::string>         _complex_names;
        
        /**
        * The list contains all names of the data types.
        * If the list contains two equal names, it will be set
        * an error in the ImporterMsgList.
        */
        std::set<std::string>         _data_types_names;

        /**
        * The list contains all names of the enum.
        * If the list contains two equal names, it will be set
        * an error in the ImporterMsgList.
        */
        std::set<std::string>         _enum_names;

        /**
        * The list contains all names of the base unit names.
        * If the list contains two equal names, it will be set
        * an error in the ImporterMsgList.
        */
        std::set<std::string>         _base_unit_names;

        /**
        * The list contains all names of the streams.
        * If the list contains two equal names, it will be set
        * an error in the ImporterMsgList.
        */
        std::set<std::string>         _stream_names;

        /**
        * The list contains all names of the prefixes.
        * If the list contains two equal names, it will be set
        * an error in the ImporterMsgList.
        */
        std::set<std::string>         _prefix_names;

        /**
        * The list contains all names of the unit names.
        * If the list contains two equal names, it will be set
        * an error in the ImporterMsgList.
        */
        std::set<std::string>         _unit_names;
        
        DDLVersion                    _version;

        WarningLevel                  _warning_level;

        /*
        * Boolean to know if a file contains a dynamic array
        */
        bool                          _has_dynamic_arrays;
        /*
        * Boolean to know if a structure contains a dynamic array
        */
        bool                          _has_struct_dynamic_arrays;

        struct StructInfo
        {
            unsigned int size;
            unsigned int last_byte_pos_real;
            unsigned int last_byte_pos;
            bool valid;
        };

        std::map<const DDLComplex*, StructInfo>  _struct_infos;
    };
}
#endif // DDL_INSPECTOR_H_INCLUDED

