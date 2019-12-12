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

#ifndef UNIT_H_INCLUDED
#define UNIT_H_INCLUDED

#include "ddl_common.h"
#include "ddlunit_intf.h"
#include "ddlvisitor_intf.h"

namespace ddl
{

    class DDLRefUnit;

    /**
     * Container type of DDLRefUnit objects
     */
    typedef std::vector<DDLRefUnit*>  DDLRefUnitVec;
    /**
     * Iterator type for DDLRefUnitVec
     */
    typedef DDLRefUnitVec::iterator   DDLRefUnitIt;
    
    /**
     * Representation object of a unit
     */
    class DDLUnit : public IDDLUnit
    {
    public:
        /**
        * Default CTOR
        */
        DDLUnit();

        /**
         * CTOR
         * @param[in] name - Name of the new unit
         * @param[in] numerator - Numerator of the new unit
         * @param[in] denominator - Denominator of the new unit
         * @param[in] offset - Offset to the base units
         * @param[in] ddl_ref_units - Vector of reference units (optional)
         * @param[in] creation_level - Level at creation time (optional)
         */
        DDLUnit(const std::string& name,
                 const std::string& numerator,
                 const std::string& denominator,
                 double const offset,
                 DDLRefUnitVec ddl_ref_units = DDLRefUnitVec(),
                 int const creation_level = 1);

        /**
         * Copy CTOR
         * @param[in] other - Reference to unit object to copy
         */
        DDLUnit(const DDLUnit& other);

        /**
         * Assignment operator (either copies or moves)
         * @param[in] other DDL unit object to copy from
         * @return @c *this
         */
        DDLUnit& operator=(DDLUnit other);

        /**
         * Move CTOR
         * @param[in,out] other DDL unit object to move from - empty but valid when finished
         */
        DDLUnit(DDLUnit&& other);

        /**
        * DTOR
        */
        virtual ~DDLUnit();

        a_util::result::Result accept(IDDLVisitor *visitor) const;
        a_util::result::Result accept(IDDLChangeVisitor *visitor);

        bool isInitialized() const;

        bool isPredefined() const;
        bool isOverwriteable() const;

        int getCreationLevel() const;

        /**
         * Creation method to fill the object with data.
         * @param[in] name - Name of the new unit
         * @param[in] numerator - Numerator of the new unit
         * @param[in] denominator - Denominator of the new unit
         * @param[in] offset - Offset to the base units
         * @param[in] ddl_ref_units - Vector of reference units (optional)
         * @param[in] creation_level - Level at creation time (optional)
         * @retval ERR_INVALID_ARG Empty name committed
         */
        a_util::result::Result create(const std::string& name,
                       const std::string& numerator,
                       const std::string& denominator,
                       double const offset,
                       DDLRefUnitVec ddl_ref_units = DDLRefUnitVec(),
                       int const creation_level = 4);

        /**
         * Getter for the name.
         * @return the name
         */
        const std::string& getName() const;

        /**
         * Setter for the Name.
         */
        void setName(const std::string& name);


        /**
        * Getter for the numerator.
        * @return the numerator
        */
        std::string getNumerator() const;
      
        /**
         * Setter for the Denominator.
         */
        void setNumerator(const std::string& numerator);

        /**
        * Getter for the denominator.
        * @return the denominator
        */
        std::string getDenominator() const;

        /**
         * Setter for the Denominator.
         */
        void setDenominator(const std::string& denominator);

        /**
        * Getter for the offset.
        * @return the offset
        */
        double getOffset() const;

        /**
         * Setter for the offset.
         */
        void setOffset(double const offset);

        /**
        * Getter for the reference unit.
        * @return the reference unit
        */
        DDLRefUnitVec& getRefUnits();

        /**
        * Getter for the reference unit.
        * @return the reference unit
        */
        const DDLRefUnitVec& getRefUnits() const;

        /**
        * Adder for a reference unit.
        * @param[in] ref_unit - Pointer to the reference unit to add
        * @param[in] pos      - Position to add the reference unit
        *
        * @return void
        */
        void addRefUnit(DDLRefUnit* ref_unit, int pos = -1);

        /**
         * Remover for a reference unit.
         * @param[in] ref_unit - name of the reference unit to remove
         * @retval ERR_NOT_FOUND Specified reference unit not found.
         */
        a_util::result::Result removeRefUnit(const std::string& ref_unit);

        /**
         * Setter for the reference units
         * @param[in] ddl_ref_units - Vector of reference units
         *
         * @return void
         */
        void cloneRefUnits(DDLRefUnitVec ddl_ref_units);

        /**
         * Setter for the reference units
         * @param[in] ddl_ref_units - Vector of reference units
         *
         * @return void
         */
        void setRefUnits(DDLRefUnitVec ddl_ref_units);

        /**
         * Add swap functionality, also enabling the copy-swap-idiom
         * @param[in,out] lhs Left-hand side ddl type
         * @param[in,out] rhs Right-hand side ddl type
         */
        friend void swap(DDLUnit& lhs, DDLUnit& rhs) noexcept;

    private:
        std::string _name;
        std::string _numerator;
        std::string _denominator;
        double  _offset;
        DDLRefUnitVec _ddl_ref_units;

        bool _init_flag;
        int  _level;
    };

}   // namespace ddl

#endif  // _UNIT_H_INCLUDED
