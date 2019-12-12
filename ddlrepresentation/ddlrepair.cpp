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

#include "ddlrepair.h"
#include "a_util/result/error_def.h"
#include "legacy_error_macros.h"

#include "ddlcontainer.h"
#include "ddldescription.h"
#include "ddlunit.h"
#include "ddlbaseunit.h"
#include "ddlprefix.h"
#include "ddldatatype.h"
#include "ddlenum.h"
#include "ddlcomplex.h"
#include "ddlstream.h"
#include "ddlstreammetatype.h"
#include "ddlheader.h"
#include "ddlextdeclaration.h"
#include "ddlrefunit.h"
#include "ddlelement.h"
#include "ddlstreamstruct.h"


namespace ddl
{
    //define all needed error types and values locally
    _MAKE_RESULT(-4, ERR_POINTER)
    _MAKE_RESULT(-8, ERR_INVALID_HANDLE)
    _MAKE_RESULT(-16, ERR_NOT_IMPL)

    a_util::result::Result DDLRepair::visitDDL(DDLDescription* poDescription)
    {
        if (!poDescription) { return ERR_POINTER; }

        _ref_ddl = poDescription;
        
        DDLStreamVec vecStreams = poDescription->getStreams();
        for (DDLStreamIt itStream = vecStreams.begin();
            vecStreams.end() != itStream; )
        {
            if (isFailed((*itStream)->accept(this)))
            {
                _ref_ddl->removeStream((*itStream)->getName());
                vecStreams = poDescription->getStreams();
                itStream = vecStreams.begin();
            }
            else
            {
                ++itStream;
            }
        }

        DDLUnitVec vecDDLUnits = poDescription->getUnits();
        for (DDLUnitIt itUnit = vecDDLUnits.begin(); vecDDLUnits.end() != itUnit;
            ++itUnit)
        {
            RETURN_IF_FAILED((*itUnit)->accept(this));
        }

        DDLComplexVec vecStructs = poDescription->getStructs();
        for (DDLComplexIt itStruct = vecStructs.begin();
            vecStructs.end() != itStruct; ++itStruct)
        {
            RETURN_IF_FAILED((*itStruct)->accept(this));
        }

        DDLEnumVec vecEnums = poDescription->getEnums();
        for (DDLEnumIt itEnum = vecEnums.begin(); vecEnums.end() != itEnum; ++itEnum)
        {
            RETURN_IF_FAILED((*itEnum)->accept(this));
        }


        DDLDTVec vecDTs = poDescription->getDatatypes();
        for (DDLDTIt itDT = vecDTs.begin(); vecDTs.end() != itDT; ++itDT)
        {
            RETURN_IF_FAILED((*itDT)->accept(this));
        }

       

        // All representation objects are valid.
        _ref_ddl = NULL;

        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLRepair::visit(DDLHeader* poHeader)
    {
        // Nothing to resolve/validate
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLRepair::visit(DDLExtDeclaration* poExtDeclaration)
    {
        // Nothing to resolve/validate
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLRepair::visit(DDLBaseunit* poBaseunit)
    {
        // Nothing to resolve/validate
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLRepair::visit(DDLPrefix* poPrefix)
    {
        // Nothing to resolve/validate
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLRepair::visit(DDLUnit* poUnit)
    {
        if (!poUnit ||! _ref_ddl) { return ERR_POINTER; }
        DDLRefUnitVec vecRUs = poUnit->getRefUnits();
        for (DDLRefUnitIt itRU = vecRUs.begin(); vecRUs.end() != itRU; )
        {
            if (isFailed((*itRU)->accept(this)))
            {
                poUnit->removeRefUnit((*itRU)->getName());
                vecRUs = poUnit->getRefUnits();
                itRU = vecRUs.begin();
            }
            else
            {
                ++itRU;
            }
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLRepair::visit(DDLRefUnit* poRefUnit)
    {
        if (!poRefUnit || !_ref_ddl) { return ERR_POINTER; }
        IDDLUnit * poUnit = _ref_ddl->getBaseunitByName(poRefUnit->getName());
        if (NULL == poUnit)
        {
            poUnit = _ref_ddl->getUnitByName(poRefUnit->getName());
            if (NULL == poUnit)
            {
                return ERR_INVALID_HANDLE;
            }
            else
            {
                poRefUnit->setUnitObject(poUnit);
            }
        }
        else
        {
            poRefUnit->setUnitObject(poUnit);
        }
        DDLPrefix * poPrefix = _ref_ddl->getPrefixByName(poRefUnit->getPrefix());
        if (NULL == poPrefix)
        {
            return ERR_INVALID_HANDLE;
        }
        else
        {
            poRefUnit->setPrefixObject(poPrefix);
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLRepair::visit(DDLDataType* poDataType)
    {
        if (!poDataType || !_ref_ddl) { return ERR_POINTER; }
        std::string strUnit = poDataType->getUnit();
        if (!strUnit.empty())
        {
            IDDLUnit * poUnit = _ref_ddl->getBaseunitByName(strUnit);
            if (NULL == poUnit)
            {
                poUnit = _ref_ddl->getUnitByName(strUnit);
                if (NULL == poUnit)
                {
                    poDataType->setUnit(NULL);  
                }
                else
                {
                    poDataType->setUnit(poUnit);
                }
            }
            else
            {
                poDataType->setUnit(poUnit);
            }
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLRepair::visit(DDLComplex* poComplex)
    {
        if (!poComplex) { return ERR_POINTER; }
        if (!_ref_ddl) { return ERR_POINTER; }
        DDLElementVec vecDDLElements = poComplex->getElements();
        for (DDLElementIt itElem = vecDDLElements.begin();
            vecDDLElements.end() != itElem; )
        {
            if (isFailed(((*itElem)->accept(this))))
            {
                poComplex->removeElement((*itElem)->getName());
                vecDDLElements = poComplex->getElements();
                itElem = vecDDLElements.begin();
            }
            else
            {
                ++itElem;
            }
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLRepair::visit(DDLElement* poElement)
    {
        if (!poElement || !_ref_ddl) { return ERR_POINTER; }
        IDDLDataType * poDT = _ref_ddl->getDataTypeByName(poElement->getType());
        if (NULL == poDT)
        {
            poDT = _ref_ddl->getStructByName(poElement->getType());
            if (NULL == poDT)
            {
                poDT = _ref_ddl->getEnumByName(poElement->getType());
                if (NULL == poDT)
                {
                    return ERR_INVALID_HANDLE;
                }
                else
                {
                    poElement->setType(poDT);
                }
            }
            else
            {
                poElement->setType(poDT);
            }
        }
        else
        {
            poElement->setType(poDT);
        }
        std::string strUnit = poElement->getUnit();
        if (!strUnit.empty())
        {
            IDDLUnit * poUnit = _ref_ddl->getBaseunitByName(strUnit);
            if (NULL == poUnit)
            {
                poUnit = _ref_ddl->getUnitByName(strUnit);
                if (NULL == poUnit)
                {
                    poElement->setUnit(NULL);
                }
                else
                {
                    poElement->setUnit(poUnit);
                }
            }
            else
            {
                poElement->setUnit(poUnit);
            }
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLRepair::visit(DDLStream* poStream)
    {
        if (!poStream || !_ref_ddl) { return ERR_POINTER; }
        IDDLDataType * poDT = _ref_ddl->getDataTypeByName(poStream->getType());
        if (NULL == poDT)
        {
            DDLComplex* poDTComp = _ref_ddl->getStructByName(poStream->getType());
            if (NULL == poDTComp)
            {
                return ERR_INVALID_HANDLE;
            }
            else
            {
                poStream->setType(poDTComp);
            }
        }
        DDLStreamStructVec vecStrmStructs = poStream->getStructs();
        for (DDLStreamStructIt itStrmStruct = vecStrmStructs.begin();
            vecStrmStructs.end() != itStrmStruct; )
        {
            if (isFailed((*itStrmStruct)->accept(this)))
            {
                poStream->removeStruct((*itStrmStruct)->getName());
                vecStrmStructs = poStream->getStructs();
                itStrmStruct = vecStrmStructs.begin();
            }
            else
            {
                ++itStrmStruct;
            }
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLRepair::visit(DDLStreamStruct* poStreamStruct)
    {
        if (!poStreamStruct || !_ref_ddl) { return ERR_POINTER; }
        IDDLDataType * poDT = _ref_ddl->getDataTypeByName(poStreamStruct->getType());
        if (NULL == poDT)
        {
            DDLComplex* poDTComp = _ref_ddl->getStructByName(poStreamStruct->getType());
            if (NULL == poDTComp)
            {
                return ERR_INVALID_HANDLE;
            }
            else
            {
                poStreamStruct->setType(poDTComp);
            }
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLRepair::visit(DDLEnum* poEnum )
    {
        if (!poEnum || !_ref_ddl) { return ERR_POINTER; }
        IDDLDataType * poDT = _ref_ddl->getDataTypeByName(poEnum->getType());
        if (NULL == poDT)
        {
            poEnum->setType(NULL);
        }
        else
        {
            poEnum->setType(poDT);
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLRepair::visit(DDLStreamMetaType* poStreamMetaType)
    {
        return ERR_NOT_IMPL;
    }

    a_util::result::Result DDLRepair::visit(DDLProperty* poProperty)
    {
        return ERR_NOT_IMPL;
    }
}   // namespace ddl
