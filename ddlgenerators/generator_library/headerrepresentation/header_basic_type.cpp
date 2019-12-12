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
#include "header_basic_type.h"
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

HeaderBasicType::HeaderBasicType() : HeaderType(), _bit_size(0)
{ }

HeaderBasicType::HeaderBasicType(const std::string &name, size_t packing, size_t bit_size)
    : HeaderType(name, packing), _bit_size(bit_size)
{ }

HeaderBasicType::HeaderBasicType(const HeaderBasicType& other)
    : HeaderType(other), _bit_size(other._bit_size)
{ }

HeaderBasicType::~HeaderBasicType()
{ }

a_util::result::Result HeaderBasicType::accept(IHeaderVisitor *visitor) const
{
    if (!visitor)
    {
        return ERR_INVALID_ARG;
    }
    return (visitor->visit(this));
}

a_util::result::Result HeaderBasicType::setBitsize_t(size_t bit_size)
{
    _bit_size = bit_size;
    return ERR_NOERROR;
}

size_t HeaderBasicType::getBitsize_t() const
{
    return _bit_size;
}

}   // namespace ddl
