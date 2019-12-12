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


#ifndef DDL_PRINTER_H_INCLUDED
#define DDL_PRINTER_H_INCLUDED

#include "ddlvisitor_intf.h"
#include "ddldescription.h"
#include "ddlversion.h"

namespace ddl
{

    /**
     * Concrete Visitor to collect the data of a DDL in object-representation
     * and convert it back to XML (string or file).
     */
    class DDLPrinter : public IDDLVisitor
    {
    public:
        /**
        * Constructor
        */
        DDLPrinter(const bool& full_out=true);

    public:
        a_util::result::Result visitDDL(const DDLDescription* description);
        a_util::result::Result visit(const DDLHeader* header);
        a_util::result::Result visit(const DDLDataType* data_type);
        a_util::result::Result visit(const DDLComplex* complex);
        a_util::result::Result visit(const DDLStream* stream);
        a_util::result::Result visit(const DDLUnit* unit);
        a_util::result::Result visit(const DDLBaseunit* baseunit);
        a_util::result::Result visit(const DDLExtDeclaration* ext_declaration);
        a_util::result::Result visit(const DDLElement* element);
        a_util::result::Result visit(const DDLPrefix* prefix);
        a_util::result::Result visit(const DDLRefUnit* ref_unit);
        a_util::result::Result visit(const DDLStreamStruct* stream_struct);
        a_util::result::Result visit(const DDLEnum* ddl_enum);
        a_util::result::Result visit(const DDLStreamMetaType* stream_meta_type);
        a_util::result::Result visit(const DDLProperty* property);

        /**
         * Getter for the XML string of the collected DDL information.
         * @return the XML string containing the DDL description
         */
        std::string getXML() const;

        /**
         * Saves the collected DDL information as XML to the specified file.
         * @param[in] filename - Path to file to use
         * @retval ERR_FAILED Saving process failed
         */
        a_util::result::Result toFile(const std::string& filename) const;

        /**
         * The method forceVersion makes the printer ignore the DDL language version set in the
         * header of the description to be printed. To take effect, this method must be called
         * before calling visitDDL()
         * 
         * @param [in] forced_version  The version the printer use for output. 
         * @returns  Standard result code.
         * @retval a_util::result::SUCCESS  Everything went fine
         * @retval ERR_INVALID_VERSION  Unknown version passed.
         */
        a_util::result::Result forceVersion(const DDLVersion& forced_version);

    private:    // members
        a_util::xml::DOM     _dom;
        DDLVersion           _version;
        bool                 _full_out;
        std::string          _last_path;
        DDLVersion           _forced_version;
    };
}   // namespace ddl

#endif  // DDL_PRINTER_H_INCLUDED
