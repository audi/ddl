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

#ifndef HEADER_TYPE_H_INCLUDED
#define HEADER_TYPE_H_INCLUDED

#include "header_base_intf.h"

namespace ddl
{

    /**
     * Representation of a type in a header.
     */
    class HeaderType : public IHeaderBase
    {
    public:
        /**
         * Default CTOR
         */
        HeaderType();

        /**
         * CTOR
         * 
         * @param [in] name  The name of the type.
         * @param [in] packing  he packing of the type
         */
        HeaderType(const std::string &name, size_t packing);

        /**
         * Copy CTOR
         * @param [in] other  The other type this instance will be a copy of.
         */
        HeaderType(const HeaderType& other);

        /**
         * DTOR
         */
        virtual ~HeaderType();

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
         * This method gives access to the packing size of the type.
         * @return The packing size of the struct.
         */
        virtual size_t getPacking() const;

        /**
         * This method sets the packing size of the type.
         * @param [in] packing  The packing size of the type.
         * @return Standard result code. Will always return ERR_NOERROR.
         */
        a_util::result::Result setPacking(size_t packing);


        /**
        * This method gives access to the description of the type.
        * @return The description of the element.
        */
        const std::string& getDescription() const;

        /**
        * This method gives access to the description of the type.
        * @param [in] description  The description of the type.
        * @return Standard result code. Will always return ERR_NOERROR.
        */
        a_util::result::Result setDescription(const std::string& description);

        /**
        * This method gives access to the comment of the type.
        * @return The comment of the element.
        */
        const std::string& getComment() const;

        /**
        * This method gives access to the comment of the type.
        * @param [in] comment  The comment of the type.
        * @return Standard result code. Will always return ERR_NOERROR.
        */
        a_util::result::Result setComment(const std::string& comment);

    public: //implements IHeaderBase

        virtual a_util::result::Result accept(IHeaderVisitor *visitor) const = 0;

    protected:
        /// The packing of the type.
        size_t _packing;
        /// The name of the type.
        std::string _name;
        /// The description of the type.
        std::string _description;
        /// The comment of the type.
        std::string _comment;
    };
}   // namespace ddl

#endif  // HEADER_TYPE_H_INCLUDED
