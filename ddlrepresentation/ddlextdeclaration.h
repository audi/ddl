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

#ifndef DDLEXT_DECLARATION_H_INCLUDED
#define DDLEXT_DECLARATION_H_INCLUDED

#include "ddl_common.h"
#include "ddl_type.h"
#include "ddlvisitor_intf.h"

namespace ddl
{

    /**
     * Representation of an external declaration inside the header of a DDL
     * description.
     */
    class DDLExtDeclaration : public DDL
    {
    public:
        /// Default CTOR
        DDLExtDeclaration() = default;

        /**
         * CTOR
         * @param[in] key - Name of the additional information
         * @param[in] value - Value of the additional information
         */
        DDLExtDeclaration(const std::string& key, const std::string& value);

        a_util::result::Result accept(IDDLVisitor *visitor) const;
        a_util::result::Result accept(IDDLChangeVisitor *visitor);

        const std::string& getName() const;

        bool isInitialized() const;

        /**
         * Creation method to fill the object with data.
         * @param[in] key - Name of the additional information
         * @param[in] value - Value of the additional information
         * @retval ERR_INVALID_ARG Empty key committed
         */
        a_util::result::Result create(const std::string& key, const std::string& value);

        /**
         * Getter for the key
         * @return the key
         */
        std::string getKey() const;

        /**
        * Setter for the key
        */
        void setKey(const std::string& key);

        /**
         * Getter for the value
         * @return the value
         */
        std::string getValue() const;

        /**
        * Setter for the Value
        */
        void setValue(const std::string& value);

    private:
        std::string _key;
        std::string _value;

        bool _init_flag;
    };

}   // namespace ddl

#endif  // DDLEXT_DECLARATION_H_INCLUDED
