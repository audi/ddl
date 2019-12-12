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

#ifndef DDLPREFIX_H_INCLUDED
#define DDLPREFIX_H_INCLUDED

#include "ddl_common.h"
#include "ddl_type.h"

namespace ddl
{

    /**
     * Representation of a prefix in a DDL description.
     */
    class DDLPrefix : public DDL
    {
    public:
        /**
         * Default CTOR
         */
        DDLPrefix();

        /**
         * CTOR
         * @param[in] name - Name of the prefix
         * @param[in] symbol - Represents the short symbol (e.g. "k")
         * @param[in] power - Power of the prefix
         * @param[in] creation_level - Level at creation time (optional)
         */
        DDLPrefix(const std::string& name,
                   const std::string& symbol,
                   int const power,
                   int const creation_level = 1);

        a_util::result::Result accept(IDDLVisitor *visitor) const;
        a_util::result::Result accept(IDDLChangeVisitor *visitor);

        bool isPredefined() const;

        bool isInitialized() const;

        int getCreationLevel() const;

        /**
         * Creation method to fill the object with data.
         * @param[in] name - Name of the prefix
         * @param[in] symbol - Represents the short symbol (e.g. "k")
         * @param[in] power - Power of the prefix
         * @param[in] creation_level - Level at creation time (optional)
         * @retval ERR_POINTER Null-pointer committed
         */
        a_util::result::Result create(const std::string& name,
                       const std::string& symbol,
                       int const power,
                       int const creation_level = 1);

        /**
         * Getter for the name
         * @return the name
         */
        const std::string& getName() const;

        /**
         * Setter for the name
         */
        void setName(const std::string& name);

        /**
         * Getter for the symbol
         * @return the symbol
         */
        std::string getSymbol() const;

        /**
         * Setter for the symbol
         */
        void setSymbol(const std::string& symbol);

        /**
         * Getter for the power
         * @return the power
         */
        int getPower() const;

        /**
         * Setter for the power
         */
        void setPower(int const power);


    private:
        std::string _name;
        std::string _symbol;
        int _power;

        bool _init_flag;
        int _level;
    };

}   // namespace ddl

#endif  // _PREFIX_H_INCLUDED_
