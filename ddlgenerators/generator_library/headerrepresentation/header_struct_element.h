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

#ifndef HEADER_STRUCT_ELEMENT_H_INCLUDED
#define HEADER_STRUCT_ELEMENT_H_INCLUDED
#include "header_base_intf.h"
#include "header_type.h"
#include "header_struct_element.h"

namespace ddl
{

    /**
     * Representation of in a struct element.
     */
    class HeaderStructElement : public IHeaderBase
    {
    public:
        /**
         * Default CTOR
         */
        HeaderStructElement();

        /**
         * CTOR
         * 
         * @param [in] name      The name of the element.
         * @param [in] type        A pointer to the type this element is of. An instance of this class will not own the type.
         * @param [in] array_size  The array size of the element
         * @param [in] is_pointer   Set true, if the element is an pointer, false otherwise.
         * @param [in] is_static    Set true, if the element is static, false otherwise.
         * @param [in] is_const    Set true, if the element is constant, false otherwise.
         */
        HeaderStructElement(const std::string &name, const HeaderType* type,
                             size_t array_size, 
                             bool is_pointer,
                             bool is_static,
                             bool is_const);

        /**
         * Copy CTOR
         * @param [in] other  The other type this instance will be a copy of.
         */
        HeaderStructElement(HeaderStructElement& other);

        /**
         * DTOR
         */
        virtual ~HeaderStructElement();

        // Setter and getter for member access

        /**
         * This method gives access to the type of the element.
         * @return The type of the element.
         */
        const HeaderType* getType() const;

        /**
         * This method sets the type of the element.
         * @param [in] type  The type of the element.
         * @return Standard result code. Will always return ERR_NOERROR.
         */
        a_util::result::Result setType(const HeaderType* type);

        /**
         * This method gives access to the name of the element.
         * @return The name of the element.
         */
        const std::string& getName() const;

        /**
         * This method gives access to the name of the element.
         * @param [in] name  The name of the element.
         * @return Standard result code. Will always return ERR_NOERROR.
         */
        a_util::result::Result setName(const std::string& name);

        /**
         * This method gives access to the array size of the element.
         * @return The array size of the element.
         */
        size_t getArraySize() const;

        /**
         * This method sets the array size of the element.
         * @param [in] array_size  The array size of the element.
         * @return Standard result code. Will always return ERR_NOERROR.
         */
        a_util::result::Result setArraySize(size_t array_size);

        /**
         * This method returns whether the element is a pointer or not.
         * @return True if the element is a pointer, false otherwise.
         */
        bool isPointer() const;

        /**
         * This method sets whether the element is a pointer or not.
         * @param [in] is_pointer  Set true if the element is a pointer, false otherwise.
         * @return Standard result code. Will always return ERR_NOERROR.
         */
        a_util::result::Result setIsPointer(bool is_pointer);

        /**
         * This method returns whether the element is static or not.
         * @return True if the element is static, false otherwise.
         */
        bool isStatic() const;

        /**
         * This method sets whether the element is static or not.
         * @param [in] is_static  Set true if the element is static, false otherwise.
         * @return Standard result code. Will always return ERR_NOERROR.
         */
        a_util::result::Result setIsStatic(bool is_static);

        /**
         * This method returns whether the element is a const.
         * @return True if the element is a const, false otherwise.
         */
        bool isConst() const;

        /**
         * This method sets whether the element is a const or not.
         * @param [in] is_constant  Set true if the element is a const, false otherwise.
         * @return Standard result code. Will always return ERR_NOERROR.
         */
        a_util::result::Result setIsConst(bool is_constant);

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

        virtual a_util::result::Result accept(IHeaderVisitor *visitor) const;

    private:
        /// The type of the element.
        const HeaderType* _type;
        /// The name of the element.
        std::string _name;
        /// The array size of the element.
        size_t _array_size;
        // Flags for the element
        /// The element is a pointer.
        bool  _is_pointer;
        /// The element is static.
        bool _is_static;
        /// The element is const.
        bool _is_const;
        /// The description of the type.
        std::string _description;
        /// The comment of the type.
        std::string _comment;
    };
}   // namespace ddl

#endif  // HEADER_STRUCT_ELEMENT_H_INCLUDED
