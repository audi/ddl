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
#include "header_type.h"

namespace ddl_generator
{
    namespace oo
    {
        //define all needed error types and values locally
        _MAKE_RESULT(0, ERR_NOERROR);
    }
}

using namespace ddl_generator::oo;
using namespace ddl;



namespace ddl
{

HeaderType::HeaderType() : _packing(0)
{ }

HeaderType::HeaderType(const std::string &name, size_t packing) :
    _packing(packing),
    _name(name)
{ }

HeaderType::HeaderType(const HeaderType& other) :
    _packing(other._packing),
    _name(other._name)
{ }

HeaderType::~HeaderType()
{ }

const std::string &HeaderType::getName() const
{
    return _name;
}

a_util::result::Result HeaderType::setName(const std::string& name)
{
    _name = name;
    return ERR_NOERROR;
}

const std::string &HeaderType::getDescription() const
{
    return _description;
}

a_util::result::Result HeaderType::setDescription(const std::string& description)
{
    _description = description;
    return ERR_NOERROR;
}

const std::string &HeaderType::getComment() const
{
    return _comment;
}

a_util::result::Result HeaderType::setComment(const std::string& comment)
{
    _comment = comment;
    return ERR_NOERROR;
}

size_t HeaderType::getPacking() const
{
    return _packing;
}

a_util::result::Result HeaderType::setPacking( size_t packing )
{
    _packing = packing;
    return ERR_NOERROR;
}

}   // namespace ddl
