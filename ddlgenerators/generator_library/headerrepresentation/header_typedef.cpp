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
#include "header_typedef.h"
#include "header_visitor_intf.h"

namespace ddl_generator
{
    namespace oo
    {
        //define all needed error types and values locally
        _MAKE_RESULT(0, ERR_NOERROR);
        _MAKE_RESULT(-5, ERR_INVALID_ARG);
    }
}

using namespace ddl_generator::oo;
using namespace ddl;


namespace ddl
{

HeaderTypedef::HeaderTypedef() : HeaderType()
{
    _type = NULL;
}

HeaderTypedef::HeaderTypedef(const std::string &name, const HeaderType* type)
    : HeaderType(name, 0), _type(type)
{ }

HeaderTypedef::HeaderTypedef(const HeaderTypedef& other)
    : HeaderType(other), _type(other._type)
{ }

HeaderTypedef::~HeaderTypedef()
{
    _type = NULL;
}

a_util::result::Result HeaderTypedef::accept(IHeaderVisitor *visitor) const
{
    if (!visitor)
    {
        return ERR_INVALID_ARG;
    }
    return (visitor->visit(this));
}


const HeaderType* HeaderTypedef::getOriginalType() const
{
    return _type;
}

a_util::result::Result HeaderTypedef::setOriginalType(const HeaderType *type)
{
    _type = type;
    return ERR_NOERROR;
}

size_t HeaderTypedef::getPacking() const
{
    return _type != NULL ? _type->getPacking() : 0;
}

}   // namespace ddl
