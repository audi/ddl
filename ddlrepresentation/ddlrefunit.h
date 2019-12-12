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

#ifndef REF_UNIT_H_INCLUDED
#define REF_UNIT_H_INCLUDED

#include "ddl_common.h"
#include "ddl_type.h"
#include "ddlunit_intf.h"
#include "ddlvisitor_intf.h"

namespace ddl
{

    /**
     * Decorator class for IDDLUnit objects to be used inside other DDLUnit objects.
     */
    class DDLRefUnit : public DDL
    {
    public:
        /**
        * Default CTOR
        */
        DDLRefUnit() = default;

        /**
         * CTOR with unit object.
         * @param[in] unit - Pointer to the unit object
         * @param[in] power - Power of the new unit related to the base units
         * @param[in] prefix - Pointer to the prefix object
         */
        DDLRefUnit(IDDLUnit* unit, int const power, DDLPrefix* prefix);

        a_util::result::Result accept(IDDLVisitor *visitor) const;
        a_util::result::Result accept(IDDLChangeVisitor *visitor);

        bool isInitialized() const;

        /**
         * Creation method to fill the object with data.
         * @param[in] unit - Pointer to the unit object
         * @param[in] power - Power of the new unit related to the base units
         * @param[in] prefix - Pointer to the prefix object
         * @retval ERR_POINTER Null-pointer committed
         */
        a_util::result::Result create(IDDLUnit* unit, int const power, DDLPrefix* prefix);

        /**
         * Getter for the name.
         * @return the name
         */
        const std::string& getName() const;

        /**
         * Getter for the unit object.
         * @return pointer to the unit object
         */
        IDDLUnit * getUnitObject() const;

        /**
         * Setter for the unit object.
         * @param unit the unit object to set
         * @return void
         */
        void setUnitObject(IDDLUnit* const unit);

        /**
         * Getter for the power.
         * @return the power
         */
        int getPower() const;

        /**
         * Setter for the power.
         * @param power the power
         * @return void
         */
        void setPower(int const power);

        /**
         * Getter for the prefix name.
         * @return the prefix name
         */
        std::string getPrefix() const;

        /**
         * Getter for the prefix object.
         * @return pointer to the prefix object
         */
        DDLPrefix * getPrefixObject() const;

        /**
         * Setter for the prefix object.
         * @param prefix   pointer to the prefix object
         * @return void
         */
         void setPrefixObject(DDLPrefix* const prefix);

    private:
        IDDLUnit *_unit;
        int _power;
        DDLPrefix *_prefix;
        bool _init_flag;
    };

}   // namespace ddl

#endif  // REF_UNIT_H_INCLUDED
