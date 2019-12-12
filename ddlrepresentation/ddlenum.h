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

#ifndef DDLCONSTANT_H_INCLUDED
#define DDLCONSTANT_H_INCLUDED

#include "ddl_common.h"
#include "ddldatatype_intf.h"
#include "ddlunit_intf.h"

namespace ddl
{
    /**
     * Container type of DDL enum elements
     */
    typedef std::vector<std::pair<std::string, std::string> > EnumNameValueVec;
    /**
    * Representation for an enum inside a DDL description.
    */
    class DDLEnum : public IDDLDataType
    {
    public:
        /**
        * Default CTOR
        */
        DDLEnum();

         /**
         * CTOR
         * @param[in] type - Pointer to the data-type object
         * @param[in] name - Name of the created enum
         * @param[in] name_values - Values of the enum
         * @param[in] unit - Pointer to the unit of the datatype (optional)
         * @param[in] creation_level - Level at creation time (optional)
         */
        DDLEnum(IDDLDataType* type,
                     const std::string& name,
                     const EnumNameValueVec& name_values,
                     IDDLUnit* unit = NULL,
                     int creation_level = 1);

        a_util::result::Result accept(IDDLVisitor *visitor) const;
        a_util::result::Result accept(IDDLChangeVisitor *visitor);

        bool isPredefined() const;
        bool isOverwriteable() const;

        const std::string& getName() const;

        /**
         * Setter for the name of the element.
         * @param [in] name Name of the element
         *
         * @return void
         */
        void setName(const std::string& name);

        /**
         * Getter for the Container type of DDL enum elements.
         *
         * @return Container type of DDL enum elements.
         */
        const EnumNameValueVec& getValues() const;

        /**
         * Setter for the DDL enum elements.
         *
         * @param [in] values    Container type of DDL enum elements
         *
         * @return void.
         */
        void setValues(const EnumNameValueVec& values);

        /**
         * Getter for the value of the element.
         *
         * @param [in]  element  Name of the element
         * @param [out] value    Value of the element
         *
         * @return Standard result code.
         */
        a_util::result::Result getValue(const std::string& element, std::string& value) const;

        /**
         * Setter for the value of the element.
         *
         * @param [in] element  Name of the element
         * @param [in] value    Value of the element
         *
         * @return Standard result code.
         */
        a_util::result::Result setValue(const std::string& element, const std::string& value);

        /**
         * Getter for the name of the element.
         *
         * @param [in]  value     Value of the element
         * @param [out] element   Name of the element
         *
         * @return Standard result code.
         */
        a_util::result::Result getNameForValue(const std::string& value, std::string& element);


         /**
         * Getter for the data-type name.
         * @return name of the data type
         */
        const std::string& getType() const;

        /**
         * Setter for the data-type object.
         *
         * @param [in] type    pointer to the data-type object 
         *
         * @return void
         */
        void setType(IDDLDataType* const type);

        /**
         * Getter for the data-type object.
         * @return pointer to the data-type object
         */
        IDDLDataType * getTypeObject() const;

        int getCreationLevel() const;

        bool isInitialized() const;

         /**
         * Creation method to fill the object with data.
         * @param[in] type - Pointer to the data-type object
         * @param[in] name - Name of the created enum
         * @param[in] name_values - Values of the enum
         * @param[in] unit - Pointer to the unit of the datatype (optional)
         * @param[in] creation_level - Level at creation time (optional)
         * @return Standard result.
         * @retval ERR_POINTER Null-pointer committed
         */
        a_util::result::Result create(IDDLDataType* type,
                       const std::string& name,
                       const EnumNameValueVec& name_values,
                       IDDLUnit* unit = NULL,
                       int creation_level = 1);



        /**
         * Setter for the unit.
         * @param[in] unit - Pointer to the unit object of the data type
         *
         * @return void
         */
        void setUnit(IDDLUnit* unit);

        /**
         * Getter for the unit name.
         * @return name of the referenced unit (\c a_util::strings::empty_string if there is none)
         */
        std::string getUnit() const;

        /**
         * Getter for the unit object.
         * @return pointer to the unit object (\c NULL if there is none)
         */
        IDDLUnit * getUnitObject() const;

         /**
         * Remove the element with the provided name.
         * @param[in] element - name of the element to be removed
         * @return Standard result.
         * @retval ERR_NOT_FOUND element not found
         */
        a_util::result::Result removeElement(const std::string& element);

    private:
        std::string _name;
        EnumNameValueVec _name_values;
        IDDLUnit *_unit;
        IDDLDataType * _type;
     

        bool _init_flag;
        int _level;

    };

}   // namespace ddl

#endif  // DDLCONSTANT_H_INCLUDED
