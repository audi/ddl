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
#include "header_struct.h"
#include "header_visitor_intf.h"

namespace ddl_generator
{
    namespace oo
    {
        //define all needed error types and values locally
        _MAKE_RESULT(0, ERR_NOERROR)
        _MAKE_RESULT(-5, ERR_INVALID_ARG)
        _MAKE_RESULT(-20, ERR_NOT_FOUND)
    }
}

using namespace ddl_generator::oo;
using namespace ddl;

namespace ddl
{

HeaderStruct::HeaderStruct() : HeaderType()
{ }

HeaderStruct::HeaderStruct( std::string name, size_t packing /*= 4*/, HeaderStructElementVec header_elements /*= tHeaderElementVec()*/ )
    : HeaderType(name, packing), _header_elements(header_elements)
{ }

HeaderStruct::HeaderStruct( HeaderStruct& other )
    : HeaderType(other)
{
    for (HeaderStructElementVec::iterator iter = other._header_elements.begin(); iter != other._header_elements.end(); iter++)
    {
        _header_elements.push_back(new HeaderStructElement(*(*iter)));
    }
}

HeaderStruct::~HeaderStruct()
{
    for (HeaderStructElementVec::iterator iter = _header_elements.begin(); iter != _header_elements.end(); iter++)
    {
        delete *iter;
    }
    _header_elements.clear();
}

const HeaderStructElementVec& HeaderStruct::getElements() const
{
    return _header_elements;
}

a_util::result::Result HeaderStruct::addElement( HeaderStructElement* element )
{
    _header_elements.push_back(element);
    recalculatePacking();
    return ERR_NOERROR;
}

a_util::result::Result HeaderStruct::removeElement( const std::string &name )
{
    for (HeaderStructElementVec::iterator iter = _header_elements.begin(); iter != _header_elements.end(); iter++)
    {
        if ((*iter)->getName() == name)
        {
            delete *iter;
            _header_elements.erase(iter);
            return ERR_NOERROR;
        }
    }
    return (ERR_NOT_FOUND);
}

a_util::result::Result HeaderStruct::accept(IHeaderVisitor *visitor) const
{
    if (!visitor)
    {
        return ERR_INVALID_ARG;
    }
    return (visitor->visit(this));
}

a_util::result::Result HeaderStruct::recalculatePacking()
{
    size_t packing = 0;
    for (HeaderStructElementVec::const_iterator iter = _header_elements.begin(); iter != _header_elements.end(); iter++)
    {
        if (NULL != (*iter)->getType() && packing < (*iter)->getType()->getPacking())
        {
            packing = (*iter)->getType()->getPacking();
        }
    }
    setPacking(packing);
    return ERR_NOERROR;
}

}   // namespace ddl
