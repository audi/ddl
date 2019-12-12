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

#include "header_type.h"
#include "header_visitor_intf.h"

namespace ddl
{
    /**
     * Vector type for type objects
     */
    typedef std::vector<const HeaderType*> HeaderConstTypes;

    /**
     * This class creates the header file string.
     * @remark The class will not take ownership of any pointers passed.
     */
    class HeaderPrinter : public IHeaderVisitor
    {
    public:

        /**
         * Constructor
         */
        HeaderPrinter();

        /**
         * Destructor
         */
        virtual ~HeaderPrinter();

        /**
         * The method writeToFile writes the header string to the specified file.
         * Existing files will be overwritten.
         * 
         * @param [in] filename  The path of the file to write to.
         * @returns  Standard result code.
         * @retval ERR_NOERROR  Everything went fine.
         * @retval ERR_OPEN_FAILED  The file could not be opened.
         * @retval ERR_PATH_NOT_FOUND  The directory containing the file does not exist.
         */
        a_util::result::Result writeToFile(const a_util::filesystem::Path &filename);

        /**
         * The method getHeader returns the textual representation of the header.
         * If no header has been parsed yet, the string will be empty.
         * 
         * @returns  The header as a string.
         */
        const std::string& getHeader();

    public: // implements IHeaderVisitor
        a_util::result::Result visit(const Header* header);

        a_util::result::Result visit(const HeaderBasicType* basic_type);

        a_util::result::Result visit(const HeaderTypedef* type_def);

        a_util::result::Result visit(const HeaderConstant* constant);

        a_util::result::Result visit(const HeaderStruct* header_struct);

        a_util::result::Result visit(const HeaderStructElement* struct_element);

        a_util::result::Result visit(const HeaderEnum* header_enum);

        void SetNamespace(const std::string name_space);

    private:

        /**
         * The method appendType looks for a type identified by its name.
         * If the type is found, it is visited for printing.
         * 
         * @param [in] strName  The type.
         * @returns  Standard result code.
         * @retval ERR_NOERROR  The type was found and appended
         * @retval ERR_NOT_FOUND  The type could not be found
         */
        a_util::result::Result appendType(const HeaderType* type);

        /**
         * The method CollectType can be used for collecting types that occur during printing.
         * This method is used to fill the list of known and unknown types.
         * A type is unknown, if its just used, and known, if its defined.
         * 
         * @param [in] type  The type to be collected. The class does not take ownership of the type.
         * @param [in] is_known  Whether this type is known or not.
         * @returns  Standard result code.
         * @retval ERR_NOERROR  Everything went fine
         */
        a_util::result::Result CollectType(const HeaderType* type, bool is_known);

        /**
         * The method printUnknownTypes prints the names of all types that occured during parsing
         * and are not defined inside the header.
         * 
         * @returns  Standard result code.
         * @retval ERR_NOERROR  Everything went fine
         */
        a_util::result::Result printUnknownTypes();

        /** 
         * Helper method to print the description and/or comment of a type to the header.
         * Only prints anything if either description or comment are non empty
         */
        a_util::result::Result printDescription(const std::string& description, const std::string& comment, bool indent = false);

        /**
        * Helper method to print the description and/or comment of a type to the header.
        * Only prints anything if either description or comment are non empty
        */
        a_util::result::Result printDescription(const HeaderType* type);

        /// The header to be parsed
        const Header* _header;
        /// The string containing the output of the printer
        std::string _header_output;
        /// The vector of known types
        HeaderConstTypes _known_types;
        /// The vector of unknown types
        HeaderConstTypes _unknown_types;

        std::string _name_space;

        static std::string addHeaderGuards(const a_util::filesystem::Path &filename, const std::string &ungarded_header_content);
    };
}
