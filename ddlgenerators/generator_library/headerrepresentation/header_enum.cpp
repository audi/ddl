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
#include "header_enum.h"
#include "header_visitor_intf.h"

namespace ddl_generator
{
    namespace oo
    {
        //define all needed error types and values locally
        _MAKE_RESULT(0, ERR_NOERROR);
        _MAKE_RESULT(-5, ERR_INVALID_ARG);
        _MAKE_RESULT(-20, ERR_NOT_FOUND);
    }
}

using namespace ddl_generator::oo;
using namespace ddl;

namespace ddl
{

    HeaderEnum::HeaderEnum() : HeaderType(), _enum_values()
    { }

    HeaderEnum::HeaderEnum(std::string name, size_t packing /*= 4*/)
        : HeaderType(name, packing), _enum_values()
    { }

    HeaderEnum::HeaderEnum(HeaderEnum& other)
        : HeaderType(other), _enum_values()
    { }

    HeaderEnum::~HeaderEnum()
    { }

    const HeaderEnumValueMap& HeaderEnum::getValues() const
    {
        return _enum_values;
    }

    a_util::result::Result HeaderEnum::addValue(int32_t value, const std::string& name)
    {
        if (_enum_values.count(value) != 0)
        {
            // non unique key
            return ERR_INVALID_ARG;
        }
        _enum_values[value] = name;
        return ERR_NOERROR;
    }


    a_util::result::Result HeaderEnum::removeValue( int32_t value )
    {
        HeaderEnumValueMap::iterator iter = _enum_values.find(value);
        if (iter == _enum_values.end())
        {
            return ERR_NOT_FOUND;
        }

        _enum_values.erase(iter);
        return ERR_NOERROR;
    }


    a_util::result::Result HeaderEnum::accept(IHeaderVisitor *visitor) const
    {
        if (!visitor)
        {
            return ERR_INVALID_ARG;
        }
        return (visitor->visit(this));
    }

}   // namespace ddl
