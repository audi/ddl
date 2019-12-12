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

#ifndef HEADER_BASE_TYPE_H_INCLUDED
#define HEADER_BASE_TYPE_H_INCLUDED
#include "header_type.h"

namespace ddl
{

    /**
     * Representation of a basic type in a header.
     */
    class HeaderBasicType : public HeaderType
    {
    public:
        /**
         * Default CTOR
         */
        HeaderBasicType();

        /**
         * CTOR
         * 
         * @param [in] name  The name of the type.
         * @param [in] packing  The packing of the type
         * @param [in] bit_size  The size f the type in bit.
         */
        HeaderBasicType(const std::string &name, size_t packing, size_t bit_size);

        /**
         * Copy CTOR
         * @param [in] other  The other type this instance will be a copy of.
         */
        HeaderBasicType(const HeaderBasicType& other);

        /**
         * DTOR
         */
        virtual ~HeaderBasicType();

        /**
         * The method setBitsize_t sets the size of the type in bit.
         * 
         * @param [in] bit_size  The size in bit.
         * @returns  Standard result code. Will always return ERR_NOERROR.
         */
        a_util::result::Result setBitsize_t(size_t bit_size);

        /**
         * The method getBitsize_t returns the size of the basic type in bit.
         * 
         * @returns  The size in bit.
         */
        size_t getBitsize_t() const;


    public: //implements HeaderType

        virtual a_util::result::Result accept(IHeaderVisitor *visitor) const;
    
    private:
        /// Stores the size of the basic type in bit
        size_t _bit_size;
    };

}   // namespace ddl

#endif  // HEADER_BASE_TYPE_H_INCLUDED
