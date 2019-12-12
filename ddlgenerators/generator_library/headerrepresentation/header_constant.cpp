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
#include "header_constant.h"
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

HeaderConstant::HeaderConstant() : a_util::variant::Variant()
{ }

HeaderConstant::HeaderConstant(const HeaderConstant& other) : a_util::variant::Variant(other), _name(other._name)
{ }

HeaderConstant::~HeaderConstant()
{ }

const std::string& HeaderConstant::getName() const
{
    return _name;
}

a_util::result::Result HeaderConstant::setName(const std::string& name)
{
    _name = name;
    return ERR_NOERROR;
}

a_util::result::Result HeaderConstant::setType(const HeaderType* type)
{
    _type = type;
    return ERR_NOERROR;
}

const HeaderType* HeaderConstant::getType() const
{
    return _type;
}

a_util::result::Result HeaderConstant::accept(IHeaderVisitor *visitor) const
{
    if (!visitor)
    {
        return ERR_INVALID_ARG;
    }
    return (visitor->visit(this));
}
}   // namespace ddl
