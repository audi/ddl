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

#ifndef STREAM_STRUCT_H_INCLUDED
#define STREAM_STRUCT_H_INCLUDED

#include "ddl_common.h"
#include "ddl_type.h"
#include "ddlvisitor_intf.h"

namespace ddl
{
    /**
     * Decorator class for DDLComplex to be used inside DDLStream objects.
     */
    class DDLStreamStruct : public DDL
    {
    public:
        /**
         * Default CTOR.
         */
        DDLStreamStruct() = default;

        /**
         * CTOR
         * @param[in] type - Pointer to complex datatype object
         * @param[in] byte_pos - Byte position of the complex datatype
         * @param[in] name - Name of the stream structure (optional)
         * @remarks If the name of the stream structure is not set then the
         * type name is used.
         */
        DDLStreamStruct(DDLComplex* type,
                         unsigned int const byte_pos,
                         const std::string& name = a_util::strings::empty_string);

        a_util::result::Result accept(IDDLVisitor *visitor) const;
        a_util::result::Result accept(IDDLChangeVisitor *visitor);

        bool isInitialized() const;

        /**
         * Creation method to fill the object with data.
         * @param[in] type - Pointer to complex datatype object
         * @param[in] byte_pos - Byte position of the complex datatype
         * @param[in] name - Name of the stream structure (optional)
         * @retval ERR_POINTER Null-pointer committed
         * @remarks If the name of the stream structure is not set then the
         * type name is used.
         */
        a_util::result::Result create(DDLComplex* type,
                       unsigned int const byte_pos,
                       const std::string& name = a_util::strings::empty_string);

        /**
         * Getter for the name of the complex data type.
         * @return name of the type object
         */
        const std::string& getType() const;

        /**
         * Setter for the complex data-type object.
         * @param[in] type - complex data-type object
         * @return void
         */
        void setType(DDLComplex* const type);

        /**
         * Getter for the complex data-type object.
         * @return pointer to the complex data-type object
         */
        DDLComplex * getTypeObject() const;

        /**
         * Setter for the name.
         * @param[in] name - Name of the stream structure
         * @return void
         */
        void setName(const std::string& name);

        /**
         * Getter for the name.
         * @return the name
         */
        const std::string& getName() const;

        /**
         * Getter for the byte position.
         * @return the byte position
         */
        unsigned int getBytepos() const;

        /**
         * Setter for the byte position.
         * @param[in] byte_pos - the byte position
         * @return void
         */
        void setBytepos(unsigned int const byte_pos);


    private:
        DDLComplex * _type;
        unsigned int _bytes_pos;
        std::string _name;
        bool _init_flag;
    };

} // namespace ddl

#endif  // STREAM_STRUCT_H_INCLUDED
