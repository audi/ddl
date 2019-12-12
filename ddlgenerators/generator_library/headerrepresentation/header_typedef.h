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

#ifndef HEADER_TYPEDEF_H_INCLUDED
#define HEADER_TYPEDEF_H_INCLUDED

#include "header_type.h"

namespace ddl
{

    /**
     * Representation of a typedef in a header.
     *
     * @remark This class does not support unnamed structs/unions or anonymous unions.
     */
    class HeaderTypedef : public HeaderType
    {
    public:
        /**
         * Default CTOR
         */
        HeaderTypedef();

        /**
         * CTOR
         * @param[in] name  The name of the type.
         * @param[in] type  The original type this type represents. This class is not taking ownership of the type.
         */
        HeaderTypedef(const std::string &name, const HeaderType* type);

        /**
         * Copy CTOR
         * @param [in] other  The other type this instance will be a copy of.
         */
        HeaderTypedef(const HeaderTypedef& other);

        /**
         * DTOR
         */
        virtual ~HeaderTypedef();

        /**
         * The method getOriginalType gives access to the original type this type is representing.
         * 
         * @returns The original type.
         */
        const HeaderType* getOriginalType() const;

        /**
         * The method setOriginalType sets the original type this type represents.
         * 
         * @param [in] type  The original type. This class is not taking ownership of the type.
         * @returns  Standard result code.
         * @retval ERR_NOERROR  Everything went fine.
         */
        a_util::result::Result setOriginalType(const HeaderType *type);

    public: //implements HeaderType

        virtual a_util::result::Result accept(IHeaderVisitor *visitor) const;

    public: //overrides HeaderType
        /**
         * @copydoc HeaderType::getPacking()
         * @remark The packing information of the mapped type will be inherited.
         * If no mapped type is defined, 0 will be returned.
         */
        size_t getPacking() const;

    private:
        /// The original type of this typedef
        const HeaderType* _type;
    };
}   // namespace ddl

#endif  // HEADER_TYPEDEF_H_INCLUDED
