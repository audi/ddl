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

#include <ddl.h>
#include "header_struct_element.h"
#include "header_visitor_intf.h"
namespace ddl_generator
{
    namespace oo
    {
        //define all needed error types and values locally
        _MAKE_RESULT(0, ERR_NOERROR)
        _MAKE_RESULT(-5, ERR_INVALID_ARG)
    }
}

using namespace ddl_generator::oo;
using namespace ddl;

namespace ddl
{

HeaderStructElement::HeaderStructElement()
{
    _type = NULL;
    _array_size = 0;
    _is_pointer = false;
    _is_static = false;
    _is_const = false;
}

HeaderStructElement::HeaderStructElement(const std::string &name, 
                                           const HeaderType* type,
                                           size_t array_size,
                                           bool is_pointer,
                                           bool is_static,
                                           bool is_const)
{
    _name = name;
    _type = type;
    _array_size = array_size;
    _is_pointer = is_pointer;
    _is_static = is_static;
    _is_const = is_const;
}

HeaderStructElement::HeaderStructElement( HeaderStructElement& other )
{
    _type = other._type;
    _array_size = other._array_size;
    _is_pointer = other._is_pointer;
    _is_static = other._is_static;
    _is_const = other._is_const;
}

HeaderStructElement::~HeaderStructElement()
{ }

const HeaderType* HeaderStructElement::getType() const
{
    return _type;
}

a_util::result::Result HeaderStructElement::setType( const HeaderType* type )
{
    _type = type;
    return ERR_NOERROR;
}

const std::string& HeaderStructElement::getName() const
{
    return _name;
}

a_util::result::Result HeaderStructElement::setName( const std::string& name )
{
    _name = name;
    return ERR_NOERROR;
}

size_t HeaderStructElement::getArraySize() const
{
    return _array_size;
}

a_util::result::Result HeaderStructElement::setArraySize( size_t array_size )
{
    _array_size = array_size;
    return ERR_NOERROR;
}

bool HeaderStructElement::isPointer() const
{
    return _is_pointer;
}

a_util::result::Result HeaderStructElement::setIsPointer( bool is_pointer )
{
    _is_pointer = is_pointer;
    return ERR_NOERROR;
}

bool HeaderStructElement::isStatic() const
{
    return _is_static;
}

a_util::result::Result HeaderStructElement::setIsStatic( bool is_static )
{
    _is_static = is_static;
    return ERR_NOERROR;
}

bool HeaderStructElement::isConst() const
{
    return _is_const;
}

a_util::result::Result HeaderStructElement::setIsConst(bool is_constant)
{
    _is_const = is_constant;
    return ERR_NOERROR;
}

a_util::result::Result HeaderStructElement::accept(IHeaderVisitor *visitor) const
{
    if (!visitor)
    {
        return ERR_INVALID_ARG;
    }
    return (visitor->visit(this));
}

const std::string &HeaderStructElement::getDescription() const
{
    return _description;
}

a_util::result::Result HeaderStructElement::setDescription(const std::string& description)
{
    _description = description;
    return ERR_NOERROR;
}

const std::string &HeaderStructElement::getComment() const
{
    return _comment;
}

a_util::result::Result HeaderStructElement::setComment(const std::string& comment)
{
    _comment = comment;
    return ERR_NOERROR;
}

}   // namespace ddl
