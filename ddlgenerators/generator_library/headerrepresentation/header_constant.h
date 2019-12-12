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

#ifndef HEADER_CONSTANT_H_INCLUDED
#define HEADER_CONSTANT_H_INCLUDED

#include "header_base_intf.h"
#include "header_type.h"

namespace ddl
{

    /**
     * Representation of a constant in a header.
     * @remark
     * This class is intended to represent a primitive type constant.
     * Although the current architecture supports setting the type
     * of the constant to a struct, it would not make much sense.
     */
    class HeaderConstant : public IHeaderBase, public a_util::variant::Variant
    {
    public:
        /**
         * Default CTOR
         */
        HeaderConstant();

        /**
         * Copy CTOR
         * @param [in] other  The other constant to copy from.
         */
        HeaderConstant(const HeaderConstant& other);

        /**
         * DTOR
         */
        virtual ~HeaderConstant();

        /**
         * This method gives access to the name of the type.
         * @return The name of the element.
         */
        const std::string& getName() const;

        /**
         * This method gives access to the name of the type.
         * @param [in] name  The name of the type.
         * @return Standard result code. Will always return ERR_NOERROR.
         */
        a_util::result::Result setName(const std::string& name);

        /**
         * The method getType gives access to the type of the constant.
         * 
         * @returns  Standard result code.
         * @retval ERR_NOERROR  Everything went fine
         */
        const HeaderType* getType() const;


        /**
         * The method setType sets the type of the constant
         * 
         * @param [in] type  A pointer to the type of the constant. This class will not own the pointer.
         * @returns  Standard result code.
         * @retval ERR_NOERROR  Will always be returned.
         */
        a_util::result::Result setType(const HeaderType* type);

    public: // implements IHeaderBase
        a_util::result::Result accept(IHeaderVisitor *visitor) const;

    private:
        /// The name of the constant.
        std::string _name;
        /// The type of the constant
        const HeaderType* _type;
    };
}   // namespace ddl

#endif  // HEADER_CONSTANT_H_INCLUDED
