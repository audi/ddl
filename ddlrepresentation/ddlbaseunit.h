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

#ifndef DDLBASE_UNIT_H_INCLUDED
#define DDLBASE_UNIT_H_INCLUDED

#include "ddl_common.h"
#include "ddlunit_intf.h"

namespace ddl
{
    /**
     * Representation of a base unit inside a DDL description.
     */
    class DDLBaseunit : public IDDLUnit
    {
    public:
        /**
         * Default CTOR
         */
        DDLBaseunit();

        /**
         * CTOR
         * @param[in] name - Name of the base unit (e.g. "metre")
         * @param[in] symbol - Symbol of the base unit (e.g. "m")
         * @param[in] description - Description of the represented base unit
         * @param[in] creation_level - Level at creation time (optional)
         */
        DDLBaseunit(const std::string& name,
                     const std::string& symbol,
                     const std::string& description,
                     int const creation_level = 1);

        /**
         * Creation method to fill the object with data.
         * @param[in] name - Name of the base unit (e.g. "metre")
         * @param[in] symbol - Symbol of the base unit (e.g. "m")
         * @param[in] description - Description of the represented base unit
         * @param[in] creation_level - Level at creation time (optional)
         * @retval ERR_INVALID_ARG Empty name committed
         */
        a_util::result::Result create(const std::string& name,
                       const std::string& symbol,
                       const std::string& description,
                       int const creation_level = 1);

        a_util::result::Result accept(IDDLVisitor *visitor) const;
        a_util::result::Result accept(IDDLChangeVisitor *visitor);

        bool isPredefined() const;
         bool isOverwriteable() const;

        bool isInitialized() const;

        int getCreationLevel() const;

        /**
         * Getter for the name.
         * @return the name
         */
        const std::string& getName() const;

        /**
        * Setter for the name.
        */
        void setName(const std::string& name);

        /**
         * Getter for the symbol.
         * @return the symbol
         */
        const std::string& getSymbol() const;
        /**
        * Setter for the description.
        */
        void setSymbol(const std::string& symbol);


        /**
         * Getter for the description.
         * @return the description
         */
        const std::string& getDescription() const;

        /**
        * Setter for the description.
        */
        void setDescription(const std::string& desc);

    private:
        std::string _name;
        std::string _symbol;
        std::string _description;

        bool _init_flag;
        int _level;
    };

}   // namespace ddl

#endif  // DDLBASE_UNIT_H_INCLUDED
