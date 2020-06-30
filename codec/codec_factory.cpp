/**
 * @file
 * Implementation of the ADTF default media description.
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

#include "struct_layout.h"
#include "codec_factory.h"

#include "ddlrepresentation/ddl_error.h"
#include "ddlrepresentation/ddldescription.h"
#include "ddlrepresentation/ddlimporter.h"
#include "ddlrepresentation/ddlversion.h"

namespace ddl
{
//define all needed error types and values locally
_MAKE_RESULT(-10, ERR_INVALID_INDEX);
_MAKE_RESULT(-20, ERR_NOT_FOUND);
_MAKE_RESULT(-37, ERR_NOT_INITIALIZED);

static a_util::result::Result getDDL(const char* strMediaDescription,
                      a_util::memory::unique_ptr<DDLDescription>& pDDL)
{
    DDLVersion version_helper = DDLVersion::ddl_version_current;

    ddl::DDLImporter oImporter;
    a_util::memory::unique_ptr<ddl::DDLDescription> pDefaultDescription(
                ddl::DDLDescription::createDefault(version_helper, 4));

    RETURN_DDLERROR_IF_FAILED_DESC(oImporter.setXML(strMediaDescription), oImporter.getErrorDesc());
    RETURN_DDLERROR_IF_FAILED_DESC(oImporter.createPartial(pDefaultDescription.get(),
        version_helper),
        oImporter.getErrorDesc());

    pDDL.reset(oImporter.getDDL());
    return a_util::result::SUCCESS;
}

CodecFactory::CodecFactory():
    _layout(new StructLayout()),
    _constructor_result(ERR_NOT_INITIALIZED)
{
}

CodecFactory::CodecFactory(const char* strStructName, const char* strMediaDescription)
{
    a_util::memory::unique_ptr<DDLDescription> pDDL;
    _constructor_result = getDDL(strMediaDescription, pDDL);
    if(isOk(_constructor_result))
    {
        DDLComplex* pStruct = pDDL->getStructByName(strStructName);
        if (pStruct)
        {
            _layout.reset(new StructLayout(pStruct));
            _constructor_result = _layout->isValid();
        }
        else
        {
            _constructor_result = ERR_NOT_FOUND;
        }
    }

    if (!_layout)
    {
        _layout.reset(new StructLayout());
    }
}

CodecFactory::CodecFactory(const DDLComplex* pStruct):
    _layout(new StructLayout(pStruct))
{
    _constructor_result = _layout->isValid();
}

a_util::result::Result CodecFactory::isValid() const
{
    return _constructor_result;
}

size_t CodecFactory::getStaticElementCount() const
{
    return _layout->getStaticElements().size();
}

a_util::result::Result CodecFactory::getStaticElement(size_t nIndex, const StructElement*& pElement) const
{
    if (nIndex >= getStaticElementCount())
    {
        return ERR_INVALID_INDEX;
    }
    pElement = &_layout->getStaticElements()[nIndex];

    return a_util::result::SUCCESS;
}

size_t CodecFactory::getStaticBufferSize(DataRepresentation eRep) const
{
    return _layout->getStaticBufferSize(eRep);
}


}
