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

#ifndef HEADER_STRUCT_H_INCLUDED
#define HEADER_STRUCT_H_INCLUDED
#include "header_struct_element.h"

namespace ddl
{

    /**
     * Container type of HeaderStructElement objects.
     */
    typedef std::vector<HeaderStructElement*> HeaderStructElementVec;

    /**
     * Representation of a struct in a header.
     * The class calculates its own packing from its children
     * every time one or more children have been added or removed.
     * To override the calculated packing, set the packing value AFTER adding
     * children.
     */
    class HeaderStruct : public HeaderType
    {
    public:
        /**
         * Default CTOR
         */
        HeaderStruct();

        /**
         * CTOR
         * @param [in] name            The name of the struct.
         * @param [in] packing          The packing of the struct.
         * @param [in] header_elements  The elements of the struct.
         *                                The struct takes ownership of the passed elements.
         */
        HeaderStruct(std::string name,
                      size_t packing = 4,
                      HeaderStructElementVec header_elements = HeaderStructElementVec());

        /**
         * Copy CTOR
         * @param [in] other  The other type this instance will be a copy of.
         */
        HeaderStruct(HeaderStruct& other);

        /**
         * DTOR
         */
        virtual ~HeaderStruct();

        /**
         * This method gives access to the elements of the struct.
         * @return The packing size of the element.
         */
        const HeaderStructElementVec& getElements() const;

        /**
         * This method adds an element to the internal list of elements of the struct.
         * @param [in] element  The element to be added. The struct takes ownership of the object.
         * @return Standard result code. Will always return ERR_NOERROR.
         */
        a_util::result::Result addElement(HeaderStructElement* element);

        /**
         * This method removes an element from the internal list of elements of the struct.
         * @param [in] name  The name of the element to be removed.
         * @return Standard result code.
         * @retval ERR_NOERROR  Everything is ok.
         * @retval ERR_NOT_FOUND  No element with this name was found.
         */
        a_util::result::Result removeElement(const std::string &name);

    public: // implements cHeaderBaseType

        a_util::result::Result accept(IHeaderVisitor *visitor) const;

    private:

        /**
         * The method recalculatePacking calculates the pack value according to its child element.
         * The biggest pack value of any child is the pack value of this struct.
         * This represents the mechanism of a C compiler to calculate the packing of a struct.
         * 
         * @returns  Standard result code.
         * @retval ERR_NOERROR  Everything went fine
         */
        a_util::result::Result recalculatePacking();

        /// The elements of the struct.
        HeaderStructElementVec _header_elements;
    };
}   // namespace ddl

#endif  // HEADER_STRUCT_H_INCLUDED
