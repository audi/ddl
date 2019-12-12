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

#include "map_header.h"

#include "map_configuration.h"

namespace mapping
{
namespace oo
{
    //define all needed error types and values locally
    _MAKE_RESULT(-5, ERR_INVALID_ARG)
}
}

using namespace mapping::oo;

MapHeader::MapHeader()
{
    reset();
}

const std::string& MapHeader::getDescription() const
{
    return _desc;
}

const std::string& MapHeader::getDllPaths() const
{
    return _ddl;
}

const std::string& MapHeader::getLanguageVersion() const
{
    return _lang_version;
}

const std::string& MapHeader::getAuthor() const
{
    return _author;
}

const std::string& MapHeader::getCreationDate() const
{
    return _creation_date;
}

const std::string& MapHeader::getModificationDate() const
{
    return _mod_date;
}

void MapHeader::reset()
{
    _desc = "Auto-generated with Signal Mapping Editor";
    _ddl.clear();
    _lang_version = "1.00";
    _author = a_util::system::getCurrentUserName();
    _creation_date = a_util::datetime::getCurrentLocalDateTime().format("%c");
    _mod_date = _creation_date;
}

a_util::result::Result MapHeader::setDescription(const std::string& strDescription)
{
    _desc = strDescription;
    return a_util::result::SUCCESS;
}

a_util::result::Result MapHeader::setDdlPaths(const std::string& strDdlPaths)
{
    _ddl = strDdlPaths;
    return a_util::result::SUCCESS;
}

a_util::result::Result MapHeader::loadFromDOM(const a_util::xml::DOMElement& oHeader, MapErrorList& lstErrors)
{
    const a_util::xml::DOMElement oLang = oHeader.getChild("language_version");
    const a_util::xml::DOMElement oAuthor = oHeader.getChild("author");
    const a_util::xml::DOMElement oCreated = oHeader.getChild("date_creation");
    const a_util::xml::DOMElement oModified = oHeader.getChild("date_change");
    const a_util::xml::DOMElement oDescription = oHeader.getChild("description");

    if (oLang.isNull() || oAuthor.isNull() || oCreated.isNull() ||
        oModified.isNull() || oDescription.isNull())
    {
        lstErrors.push_back("Missing <language_version>, <author>, <date_creation>, "
            "<date_change> or <description> element in header");
        return ERR_INVALID_ARG;
    }

    if(!a_util::strings::isDouble(oLang.getData()) || a_util::strings::toDouble(oLang.getData()) > 1)
    {
        lstErrors.push_back("<language_version> is not 1.00. No other language version exists.");
        return ERR_INVALID_ARG;
    }

    _lang_version = oLang.getData();
    _author = oAuthor.getData();
    _creation_date = oCreated.getData();
    _mod_date = oModified.getData();
    setDescription(oDescription.getData());

    // optional
    const a_util::xml::DOMElement oDdlPaths = oHeader.getChild("ddl");
    if (!oDdlPaths.isNull())
    {
        setDdlPaths(oDdlPaths.getData());
    }
    else
    {
        _ddl.clear();
    }

    return a_util::result::SUCCESS;
}

a_util::result::Result MapHeader::writeToDOM(a_util::xml::DOMElement oDOMElement)
{
    oDOMElement.setName("header");

    _mod_date = a_util::datetime::getCurrentLocalDateTime().format("%c");

    a_util::xml::DOMElement oLang = oDOMElement.createChild("language_version");
    a_util::xml::DOMElement oAuthor = oDOMElement.createChild("author");
    a_util::xml::DOMElement oCreated = oDOMElement.createChild("date_creation");
    a_util::xml::DOMElement oModified = oDOMElement.createChild("date_change");
    a_util::xml::DOMElement oDescription = oDOMElement.createChild("description");   

    oLang.setData(_lang_version);
    oAuthor.setData(_author);
    oCreated.setData(_creation_date);
    oDescription.setData(_desc);
    oModified.setData(_mod_date);

    if(!_ddl.empty())
    {
        a_util::xml::DOMElement oDdlPaths = oDOMElement.createChild("ddl");
        oDdlPaths.setData(_ddl);
    }

    return a_util::result::SUCCESS;
}
