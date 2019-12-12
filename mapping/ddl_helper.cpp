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
#include "ddl_helper.h"

#include <utility>
#include <vector>

#include "a_util/strings.h"

using namespace mapping;

//define all needed error types and values locally
_MAKE_RESULT(-5, ERR_INVALID_ARG)

namespace mapping
{
    _MAKE_RESULT(-20, ERR_NOT_FOUND)
}

static a_util::result::Result ParseElementPath(const std::string& strPath,
    std::vector<std::pair<std::string, int> >& vecPath)
{
    vecPath.clear();

    std::vector<std::string> lstPath = a_util::strings::split(strPath, ".");

    for (std::vector<std::string>::const_iterator it = lstPath.begin(); it != lstPath.end(); ++it)
    {
        const std::string& strPathElem = *it;

        std::pair<std::string, int> oPair;
        size_t idxBegin = strPathElem.find('[');
        if (idxBegin != std::string::npos)
        {
            size_t idxEnd = strPathElem.find(']', idxBegin);
            if (idxEnd == std::string::npos || idxEnd != strPathElem.size() - 1)
            {
                return ERR_INVALID_ARG;
            }
            oPair.first = strPathElem.substr(0, idxBegin);
            std::string strSubscript = strPathElem.substr(idxBegin + 1);
            strSubscript.erase(strSubscript.size() - 1);
            int nSubscript = a_util::strings::toInt32(strSubscript);
            if (!a_util::strings::isInt64(strSubscript) || nSubscript < 0)
            {
                return ERR_INVALID_ARG;
            }
            oPair.second = nSubscript;
        }
        else
        {
            oPair.first = strPathElem;
            oPair.second = -1;
        }

        vecPath.push_back(oPair);
    }

    return a_util::result::SUCCESS;
}

static const ddl::DDLElement* GetNestedElement(const ddl::DDLElementVec& vecElements,
    std::vector<std::pair<std::string, int> > vecPath)
{
    std::pair<std::string, int> oPair = vecPath.front();
    vecPath.erase(vecPath.begin());

    for (ddl::DDLElementItConst it = vecElements.begin(); it != vecElements.end(); ++it)
    {
        if ((*it)->getName() == oPair.first)
        {
            // check array size
            if (oPair.second > -1)
            {
                if ((*it)->getArraysize() <= (unsigned int)oPair.second)
                {
                    return NULL;
                }
            }

            if (vecPath.empty())
            {
                return *it;
            }
            else
            {
                const ddl::DDLComplex* pCmplx =
                    dynamic_cast<const ddl::DDLComplex*>((*it)->getTypeObject());
                if (pCmplx)
                {
                    return GetNestedElement(pCmplx->getElements(), vecPath);
                }
            }
            break;
        }
    }

    return NULL;
}

a_util::result::Result DDLHelper::LookupElement(const ddl::DDLComplex& oStruct,
    const std::string& strPath, const ddl::DDLElement*& pElement, bool& bIsArrayElement)
{
    std::vector<std::pair<std::string, int> > vecPath;
    if (isFailed(ParseElementPath(strPath, vecPath)))
    {
        return ERR_NOT_FOUND;
    }

    const ddl::DDLElement* pElem = GetNestedElement(oStruct.getElements(), vecPath);
    if (pElem)
    {
        pElement = pElem;
        bIsArrayElement = vecPath.back().second > -1;
        return a_util::result::SUCCESS;
    }

    return ERR_NOT_FOUND;
}
