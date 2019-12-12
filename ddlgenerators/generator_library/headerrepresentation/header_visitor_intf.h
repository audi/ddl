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

#ifndef HEADER_VISITOR_H_INCLUDED
#define HEADER_VISITOR_H_INCLUDED

namespace ddl
{
    class Header;
    class HeaderBasicType;
    class HeaderTypedef;
    class HeaderConstant;
    class HeaderStruct;
    class HeaderStructElement;
    class HeaderEnum;

    /**
     * Abstract base class/interface for Visitor design-pattern.
     */
    class IHeaderVisitor
    {
    public:
        /**
         * Visitor for a whole header.
         * @param [in] header  pointer to the header object.
         * @return Standard result code.
         * @retval ERR_NOERROR  Everything went fine.
         * @retval ERR_POINTER  Null-pointer passed.
         */
        virtual a_util::result::Result visit(const Header* header) = 0;

        /**
         * Visitor for a basic type.
         * @param [in] basic_type  pointer to the basic type object.
         * @return Standard result code.
         * @retval ERR_NOERROR  Everything went fine.
         * @retval ERR_POINTER  Null-pointer passed.
         */
        virtual a_util::result::Result visit(const HeaderBasicType* basic_type) = 0;

        /**
         * Visitor for a typedef.
         * @param [in] type_def  pointer to the basic type object.
         * @return Standard result code.
         * @retval ERR_NOERROR  Everything went fine.
         * @retval ERR_POINTER  Null-pointer passed.
         */
        virtual a_util::result::Result visit(const HeaderTypedef* type_def) = 0;

        /**
         * Visitor for a constant.
         * @param [in] constant  pointer to the constant object.
         * @return Standard result code.
         * @retval ERR_NOERROR  Everything went fine.
         * @retval ERR_POINTER  Null-pointer passed.
         */
        virtual a_util::result::Result visit(const HeaderConstant* constant) = 0;

        /**
         * Visitor for a struct.
         * @param [in] header_struct  pointer to the struct object.
         * @return Standard result code.
         * @retval ERR_NOERROR  Everything went fine.
         * @retval ERR_POINTER  Null-pointer passed.
         */
        virtual a_util::result::Result visit(const HeaderStruct* header_struct) = 0;

        /**
         * Visitor for a struct element.
         * @param [in] struct_element  pointer to the struct element object.
         * @return Standard result code.
         * @retval ERR_NOERROR  Everything went fine.
         * @retval ERR_POINTER  Null-pointer passed.
         */
        virtual a_util::result::Result visit(const HeaderStructElement* struct_element) = 0;

        /**
        * Visitor for a enum element.
        * @param [in] header_enum  pointer to the enum object.
        * @return Standard result code.
        * @retval ERR_NOERROR  Everything went fine.
        * @retval ERR_POINTER  Null-pointer passed.
        */
        virtual a_util::result::Result visit(const HeaderEnum* header_enum) = 0;
    };

}   // namespace ddl

#endif  // HEADER_VISITOR_H_INCLUDED
