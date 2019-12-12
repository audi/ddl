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

#ifndef DDL_PROPERTY_H_INCLUDED
#define DDL_PROPERTY_H_INCLUDED

#include "ddl_common.h"
#include "ddlvisitor_intf.h"
#include "ddl_type.h"

namespace ddl
{
    /**
     * Representation of a stream inside a DDL description.
     */
    class DDLProperty : public DDL
    {
    public:
        /**
         * Default CTOR
         */
        DDLProperty() = default;

        /**
         * CTOR
         * @param[in] name - Name of the property
         * @param[in] type - Type of the property
         */
        DDLProperty(const std::string& name, const std::string& type);


        a_util::result::Result accept(IDDLVisitor *visitor) const;
        a_util::result::Result accept(IDDLChangeVisitor *visitor);

        bool isInitialized() const;

        int getCreationLevel() const;

        /**
         * Getter for the name.
         * @return the name
         */
        const std::string& getName() const;

        /**
         * Getter for the type.
         * @return the type
         */
        const std::string& getType() const;


    private:
        std::string _name;
        std::string _type;
        bool _init_flag;
    };

}   // namespace ddl

#endif
