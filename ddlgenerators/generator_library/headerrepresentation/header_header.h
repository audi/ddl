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

#ifndef HEADER_HEADER_H_INCLUDED
#define HEADER_HEADER_H_INCLUDED

#include "header_base_intf.h"
#include "header_typedef.h"
#include "header_constant.h"
#include "header_struct.h"
#include "header_enum.h"
#include "header_visitor_intf.h"

namespace ddl
{

    /**
     * Container type of basic type objects
     */
    typedef std::vector<HeaderTypedef*> HeaderTypedefs;

    /**
     * Container type of constants objects
     */
    typedef std::vector<HeaderConstant*> HeaderConstants;

    /**
     * Container type of struct objects
     */
    typedef std::vector<HeaderStruct*> HeaderStructs;

    /**
    * Container type of enum objects
    */
    typedef std::vector<HeaderEnum*> HeaderEnums;

    /**
     * Main class representing a whole header file
     */
    class Header : public IHeaderBase
    {
    public:
        /**
         * CTOR
         */
        Header();

        /**
         * Copy CTOR. This will make a full copy of the other header.
         * @param [in] other  The other header this instance will be a copy of.
         */
        Header(const Header &other);

        /**
         * DTOR
         */
        virtual ~Header();

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
         * This method gives access to the list of typedefs from the header.
         * @returns The list of typedefs
         */
        const HeaderTypedefs& getTypedefs() const;

        /**
         * This method adds a typedef to the internal list of typedefs.
         * @param [in] type_def  The typedef to be added. The class takes ownership of the object.
         * @return Standard result code.
         * @retval ERR_NOERROR  Everything is ok.
         */
         a_util::result::Result addTypedef(HeaderTypedef* type_def);

        /**
         * This method removes a typedef from the internal list of typedefs.
         * @param [in] name  The name of the typedef to be removed.
         * @return Standard result code.
         * @retval ERR_NOERROR  Everything is ok.
         * @retval ERR_NOT_FOUND  No typedef with this name was found.
         */
        a_util::result::Result removeTypedef(const std::string &name);

        /**
         * This method gives access to the list of constants from the header.
         * @returns The list of constants.
         */
        const HeaderConstants& getConstants() const;

        /**
         * This method adds a constant to the internal list of constants.
         * @param [in] constant  The constant to be added. The class takes ownership of the object.
         * @return Standard result code.
         * @retval ERR_NOERROR  Everything is ok.
         */
        a_util::result::Result addConstant(HeaderConstant* constant);

        /**
         * This method removes a constant from the internal list of constants.
         * @param [in] strName  The name of the constant to be removed.
         * @return Standard result code.
         * @retval ERR_NOERROR  Everything is ok.
         * @retval ERR_NOT_FOUND  No constant with this name was found.
         */
        a_util::result::Result removeConstant(const std::string &name);

        /**
         * This method gives access to the list of structs from the header.
         * @returns The list of structs
         */
        const HeaderStructs& getStructs() const;

        /**
         * This method adds a struct to the internal list of structs.
         * @param [in] pStruct  The struct to be added. The class takes ownership of the object.
         * @return Standard result code.
         * @retval ERR_NOERROR  Everything is ok.
         */
        a_util::result::Result addStruct(HeaderStruct* header_struct);

        /**
         * This method removes a struct from the internal list of structs.
         * @param [in] strName  The name of the struct to be removed.
         * @return Standard result code.
         * @retval ERR_NOERROR  Everything is ok.
         * @retval ERR_NOT_FOUND  No struct with this name was found.
         */
        a_util::result::Result removeStruct(const std::string &name);

        /**
        * This method gives access to the list of enums from the header.
        * @returns The list of enums
        */
        const HeaderEnums& getEnums() const;

        /**
        * This method adds an enum to the internal list of enums.
        * @param [in] p_enum The enum to be added. The class takes ownership of the object.
        * @return Standard result code.
        * @retval ERR_NOERROR  Everything is ok.
        */
        a_util::result::Result addEnum(HeaderEnum* header_enum);


    public: // implementation of IHeaderBase
        virtual a_util::result::Result accept(IHeaderVisitor * visitor) const;

    private:
        /// The name of the header (i.e. file name)
        std::string _name;
        /// The list of typedefs.
        HeaderTypedefs _typedefs;
        /// The list of constants.
        HeaderConstants _constants;
        /// The list of structs.
        HeaderStructs _structs;
        /// The list of enums
        HeaderEnums _enums;

    };

}   // namespace ddl

#endif  // HEADER_HEADER_H_INCLUDED
