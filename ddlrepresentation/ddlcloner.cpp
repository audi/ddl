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

#include "ddlcloner.h"
#include "a_util/result/error_def.h"
#include "legacy_error_macros.h"

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

#include "ddlimporter.h"


namespace ddl
{
    //define all needed error types and values locally
    _MAKE_RESULT(-4, ERR_POINTER)
    _MAKE_RESULT(-16, ERR_NOT_IMPL)
    _MAKE_RESULT(-18, ERR_NO_CLASS)
    _MAKE_RESULT(-37, ERR_NOT_INITIALIZED)

    DDLDescription * DDLCloner::getDDL() const
    {
        return _ddl_desc;
    }

    void DDLCloner::destroyDDL()
    {
        DDLImporter::destroyDDL(_ddl_desc);
        _ddl_desc = NULL;
    }

    a_util::result::Result DDLCloner::createNew(const DDLVersion& version /* = 0 */)
    {
        if (NULL == _original_desc)
        {
            return ERR_NOT_INITIALIZED;
        }
        // The DDL object does not get deleted because the caller/user of this
        // object is responsible for it.
        _ddl_desc = NULL;
        // clone original DDL
        RETURN_IF_FAILED(buildHeader());
        RETURN_IF_FAILED(buildUnits());
        RETURN_IF_FAILED(buildDatatypes());
        RETURN_IF_FAILED(buildEnums());
        RETURN_IF_FAILED(buildStructs());
        RETURN_IF_FAILED(buildStreams());
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLCloner::buildHeader()
    {
        if (NULL == _original_desc)
        {
            return ERR_NOT_INITIALIZED;
        }
        // copy header object
        DDLHeader *header = new DDLHeader(*_original_desc->getHeader());
        // external declarations have to be cloned separately
        DDLExtDeclarationVec ext_declarations = header->getExtDeclarations();
        std::transform(ext_declarations.begin(), ext_declarations.end(),
            ext_declarations.begin(), DDL::clone<DDLExtDeclaration>);
        _ddl_desc = new DDLDescription(header);
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLCloner::buildUnits()
    {
        if (NULL == _original_desc)
        {
            return ERR_NOT_INITIALIZED;
        }

        // clone baseunit objects
        DDLBaseunitVec baseunits = _original_desc->getBaseunits();
        std::transform(baseunits.begin(), baseunits.end(), baseunits.begin(),
            DDL::clone<DDLBaseunit>);
        _ddl_desc->refBaseunits(baseunits);

        // clone prefix objects
        DDLPrefixVec ddlprefixes = _original_desc->getPrefixes();
        std::transform(ddlprefixes.begin(), ddlprefixes.end(),
            ddlprefixes.begin(), DDL::clone<DDLPrefix>);
        _ddl_desc->refPrefixes(ddlprefixes);

        // clone unit objects
        DDLUnitVec units = _original_desc->getUnits();
        std::transform(units.begin(), units.end(),
            units.begin(), DDL::clone<DDLUnit>);
        // refunit objects have to be cloned separately
        for (DDLUnitIt it_unit = units.begin(); units.end() != it_unit;
            ++it_unit)
        {
            DDLRefUnitVec refunits = (*it_unit)->getRefUnits();
            for (DDLRefUnitIt it_ru = refunits.begin(); refunits.end() != it_ru;
                ++it_ru)
            {
                // find prefix object
                DDLPrefixIt it_prefix = std::find_if(ddlprefixes.begin(),
                                                     ddlprefixes.end(), DDLCompareFunctor<DDLPrefix>((*it_ru)->getPrefix()));
                if (ddlprefixes.end() == it_prefix)
                {
                    return ERR_NO_CLASS;
                }
                // find unit and create refunit object with appropriate CTOR
                DDLBaseunitIt it_bu_found = std::find_if(baseunits.begin(),
                                                        baseunits.end(), DDLCompareFunctor<DDLBaseunit>((*it_ru)->getName()));
                if (baseunits.end() == it_bu_found)
                {
                    // not found in baseunit vector
                    DDLUnitIt it_u_found = std::find_if(units.begin(),
                                                       units.end(), DDLCompareFunctor<DDLUnit>((*it_ru)->getName()));
                    if (units.end() == it_u_found)
                    {
                        // not found in unit vector
                        // => not defined at all
                        return ERR_NO_CLASS;
                    }
                    *it_ru = new DDLRefUnit(*it_u_found, (*it_ru)->getPower(),
                        *it_prefix);
                }
                else
                {
                    *it_ru = new DDLRefUnit(*it_bu_found, (*it_ru)->getPower(),
                        *it_prefix);
                }
            }
            (*it_unit)->setRefUnits(refunits);
        }
        _ddl_desc->refUnits(units);
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLCloner::buildDatatypes()
    {
        if (NULL == _original_desc)
        {
            return ERR_NOT_INITIALIZED;
        }
        DDLDTVec datatypes = _original_desc->getDatatypes();
        for (DDLDTIt it_dt = datatypes.begin(); datatypes.end() != it_dt; ++it_dt)
        {
            DDLVersion version = (*it_dt)->getDDLVersion();
            if ((*it_dt)->getUnit().empty())
            {
                // datatype without defined unit
                (*it_dt) = new DDLDataType((*it_dt)->getName(),
                                            (*it_dt)->getNumBits(),
                                            NULL,
                                            (*it_dt)->getDescription(),
                                            (*it_dt)->getAlignment(),
                                            (*it_dt)->getArraysize(),
                                            (*it_dt)->getCreationLevel(),
                                            (*it_dt)->getArraySizeSource(),
                                            (*it_dt)->isMinValid(),
                                            (*it_dt)->getMinValue(),
                                            (*it_dt)->isMaxValid(),
                                            (*it_dt)->getMaxValue());
            }
            else
            {
                // find unit and clone datatype object with appropriate CTOR
                DDLBaseunitVec baseunits = _ddl_desc->getBaseunits();
                DDLBaseunitIt it_bu_found = std::find_if(baseunits.begin(),
                                                        baseunits.end(), DDLCompareFunctor<DDLBaseunit>((*it_dt)->getUnit()));
                if (baseunits.end() == it_bu_found)
                {
                    // not found in baseunit vector
                    DDLUnitVec units = _ddl_desc->getUnits();
                    DDLUnitIt it_u_found = std::find_if(units.begin(),
                                                       units.end(), DDLCompareFunctor<DDLUnit>((*it_dt)->getUnit()));
                    if (units.end() == it_u_found)
                    {
                        // not found in unit vector
                        // => not defined at all
                        return ERR_NO_CLASS;
                    }
                    (*it_dt) = new DDLDataType((*it_dt)->getName(),
                        (*it_dt)->getNumBits(),
                        *it_u_found,
                        (*it_dt)->getDescription(),
                        (*it_dt)->getAlignment(),
                        (*it_dt)->getArraysize(),
                        (*it_dt)->getCreationLevel(),
                        (*it_dt)->getArraySizeSource(),
                        (*it_dt)->isMinValid(),
                        (*it_dt)->getMinValue(),
                        (*it_dt)->isMaxValid(),
                        (*it_dt)->getMaxValue());
                }
                else
                {
                    (*it_dt) = new DDLDataType((*it_dt)->getName(),
                        (*it_dt)->getNumBits(),
                        *it_bu_found,
                        (*it_dt)->getDescription(),
                        (*it_dt)->getAlignment(),
                        (*it_dt)->getArraysize(),
                        (*it_dt)->getCreationLevel(),
                        (*it_dt)->getArraySizeSource(),
                        (*it_dt)->isMinValid(),
                        (*it_dt)->getMinValue(),
                        (*it_dt)->isMaxValid(),
                        (*it_dt)->getMaxValue());
                }
            }
            (*it_dt)->setDDLVersion(version);
        }
        _ddl_desc->refDatatypes(datatypes);
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLCloner::buildEnums()
    {
        if (NULL == _original_desc)
        {
            return ERR_NOT_INITIALIZED;
        }
        DDLEnumVec enums = _original_desc->getEnums();
        // clone enums
        std::transform(enums.begin(), enums.end(),
            enums.begin(), DDL::clone<DDLEnum>);
        _ddl_desc->refEnums(enums);
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLCloner::buildStructs()
    {
        if (NULL == _original_desc)
        {
            return ERR_NOT_INITIALIZED;
        }
        // clone structs
        DDLComplexVec structs;
        structs.cloneFrom(_original_desc->getStructs());

        // elements have to be cloned seperately
        DDLDTVec datatypes = _ddl_desc->getDatatypes();
        DDLUnitVec units = _ddl_desc->getUnits();
        DDLBaseunitVec baseunits = _ddl_desc->getBaseunits();
        DDLEnumVec ddl_enums = _ddl_desc->getEnums();
        for (DDLComplexVec::iterator it_struct = structs.begin();
            structs.end() != it_struct; ++it_struct)
        {
            DDLElementVec elements = (*it_struct)->getElements();
            for (DDLElementIt it_element = elements.begin();
                elements.end() != it_element; ++it_element)
            {
                DDLComplex* ddl_struct = NULL;
                DDLEnum* ddl_enum = NULL;
                DDLDataType* type = datatypes.find((*it_element)->getType());
                if (!type)
                {
                    // not found in datatypes vector
                    ddl_struct = structs.find((*it_element)->getType());
                    if (!ddl_struct)
                    {
                        // not found in structs vector
                        ddl_enum = ddl_enums.find((*it_element)->getType());
                        if (!ddl_enum)
                        {
                            // not found in enums vector
                            // => not defined at all
                            return ERR_NO_CLASS;
                        }
                    }
                }

                if ((*it_element)->getUnit().empty())
                {
                    // no unit defined
                    if (ddl_enum)
                    {
                        *it_element = new DDLElement(ddl_enum,
                            (*it_element)->getName(),
                            (*it_element)->getBytepos(),
                            (*it_element)->getArraysize(),
                            (*it_element)->getByteorder(),
                            (*it_element)->getAlignment(),
                            NULL,
                            (*it_element)->getBitpos(),
                            (*it_element)->getNumBits(),
                            (*it_element)->getDescription(),
                            (*it_element)->getComment(),
                            (*it_element)->getArraySizeSource(),
                            (*it_element)->getConstantValue(),
                            (*it_element)->isMinValid(),
                            (*it_element)->getMinValue(),
                            (*it_element)->isMaxValid(),
                            (*it_element)->getMaxValue(),
                            (*it_element)->isDefaultValid(),
                            (*it_element)->getDefaultValue(),
                            (*it_element)->isScaleValid(),
                            (*it_element)->getScaleValue(),
                            (*it_element)->isOffsetValid(),
                            (*it_element)->getOffsetValue());
                    }
                    else if(ddl_struct)
                    {
                        *it_element = new DDLElement(ddl_struct,
                            (*it_element)->getName(),
                            (*it_element)->getBytepos(),
                            (*it_element)->getArraysize(),
                            (*it_element)->getByteorder(),
                            (*it_element)->getAlignment(),
                            NULL,
                            (*it_element)->getBitpos(),
                            (*it_element)->getNumBits(),
                            (*it_element)->getDescription(),
                            (*it_element)->getComment(),
                            (*it_element)->getArraySizeSource(),
                            (*it_element)->getConstantValue(),
                            (*it_element)->isMinValid(),
                            (*it_element)->getMinValue(),
                            (*it_element)->isMaxValid(),
                            (*it_element)->getMaxValue(),
                            (*it_element)->isDefaultValid(),
                            (*it_element)->getDefaultValue(),
                            (*it_element)->isScaleValid(),
                            (*it_element)->getScaleValue(),
                            (*it_element)->isOffsetValid(),
                            (*it_element)->getOffsetValue());
                    }
                    else
                    {
                        *it_element = new DDLElement(type,
                            (*it_element)->getName(),
                            (*it_element)->getBytepos(),
                            (*it_element)->getArraysize(),
                            (*it_element)->getByteorder(),
                            (*it_element)->getAlignment(),
                            NULL,
                            (*it_element)->getBitpos(),
                            (*it_element)->getNumBits(),
                            (*it_element)->getDescription(),
                            (*it_element)->getComment(),
                            (*it_element)->getArraySizeSource(),
                            (*it_element)->getConstantValue(),
                            (*it_element)->isMinValid(),
                            (*it_element)->getMinValue(),
                            (*it_element)->isMaxValid(),
                            (*it_element)->getMaxValue(),
                            (*it_element)->isDefaultValid(),
                            (*it_element)->getDefaultValue(),
                            (*it_element)->isScaleValid(),
                            (*it_element)->getScaleValue(),
                            (*it_element)->isOffsetValid(),
                            (*it_element)->getOffsetValue());
                    }
                }
                else
                {
                    // find unit
                    DDLBaseunitIt it_bu_found = std::find_if(baseunits.begin(),
                                                            baseunits.end(), DDLCompareFunctor<DDLBaseunit>((*it_element)->getUnit()));
                    if (baseunits.end() == it_bu_found)
                    {
                        // not found in baseunit vector
                        DDLUnitIt it_u_found = std::find_if(units.begin(),
                            units.end(), DDLCompareFunctor<DDLUnit>((*it_element)->getUnit()));
                        if (units.end() == it_u_found)
                        {
                            // not found in units vector
                            // => not defined at all
                            return ERR_NO_CLASS;
                        }
                        if (ddl_enum)
                        {
                            *it_element = new DDLElement(ddl_enum,
                                (*it_element)->getName(),
                                (*it_element)->getBytepos(),
                                (*it_element)->getArraysize(),
                                (*it_element)->getByteorder(),
                                (*it_element)->getAlignment(),
                                *it_u_found,
                                (*it_element)->getBitpos(),
                                (*it_element)->getNumBits(),
                                (*it_element)->getDescription(),
                                (*it_element)->getComment(),
                                (*it_element)->getArraySizeSource(),
                                (*it_element)->getConstantValue(),
                                (*it_element)->isMinValid(),
                                (*it_element)->getMinValue(),
                                (*it_element)->isMaxValid(),
                                (*it_element)->getMaxValue(),
                                (*it_element)->isDefaultValid(),
                                (*it_element)->getDefaultValue(),
                                (*it_element)->isScaleValid(),
                                (*it_element)->getScaleValue(),
                                (*it_element)->isOffsetValid(),
                                (*it_element)->getOffsetValue());
                        }
                        else if (ddl_struct)
                        {
                            *it_element = new DDLElement(ddl_struct,
                                (*it_element)->getName(),
                                (*it_element)->getBytepos(),
                                (*it_element)->getArraysize(),
                                (*it_element)->getByteorder(),
                                (*it_element)->getAlignment(),
                                *it_u_found,
                                (*it_element)->getBitpos(),
                                (*it_element)->getNumBits(),
                                (*it_element)->getDescription(),
                                (*it_element)->getComment(),
                                (*it_element)->getArraySizeSource(),
                                (*it_element)->getConstantValue(),
                                (*it_element)->isMinValid(),
                                (*it_element)->getMinValue(),
                                (*it_element)->isMaxValid(),
                                (*it_element)->getMaxValue(),
                                (*it_element)->isDefaultValid(),
                                (*it_element)->getDefaultValue(),
                                (*it_element)->isScaleValid(),
                                (*it_element)->getScaleValue(),
                                (*it_element)->isOffsetValid(),
                                (*it_element)->getOffsetValue());
                        }
                        else
                        {
                            *it_element = new DDLElement(type,
                                (*it_element)->getName(),
                                (*it_element)->getBytepos(),
                                (*it_element)->getArraysize(),
                                (*it_element)->getByteorder(),
                                (*it_element)->getAlignment(),
                                *it_u_found,
                                (*it_element)->getBitpos(),
                                (*it_element)->getNumBits(),
                                (*it_element)->getDescription(),
                                (*it_element)->getComment(),
                                (*it_element)->getArraySizeSource(),
                                (*it_element)->getConstantValue(),
                                (*it_element)->isMinValid(),
                                (*it_element)->getMinValue(),
                                (*it_element)->isMaxValid(),
                                (*it_element)->getMaxValue(),
                                (*it_element)->isDefaultValid(),
                                (*it_element)->getDefaultValue(),
                                (*it_element)->isScaleValid(),
                                (*it_element)->getScaleValue(),
                                (*it_element)->isOffsetValid(),
                                (*it_element)->getOffsetValue());
                        }
                    }
                    else
                    {
                        if (ddl_enum)
                        {
                            *it_element = new DDLElement(ddl_enum,
                                (*it_element)->getName(),
                                (*it_element)->getBytepos(),
                                (*it_element)->getArraysize(),
                                (*it_element)->getByteorder(),
                                (*it_element)->getAlignment(),
                                *it_bu_found,
                                (*it_element)->getBitpos(),
                                (*it_element)->getNumBits(),
                                (*it_element)->getDescription(),
                                (*it_element)->getComment(),
                                (*it_element)->getArraySizeSource(),
                                (*it_element)->getConstantValue(),
                                (*it_element)->isMinValid(),
                                (*it_element)->getMinValue(),
                                (*it_element)->isMaxValid(),
                                (*it_element)->getMaxValue(),
                                (*it_element)->isDefaultValid(),
                                (*it_element)->getDefaultValue(),
                                (*it_element)->isScaleValid(),
                                (*it_element)->getScaleValue(),
                                (*it_element)->isOffsetValid(),
                                (*it_element)->getOffsetValue());
                        }
                        else if (ddl_struct)
                        {
                            *it_element = new DDLElement(ddl_struct,
                                (*it_element)->getName(),
                                (*it_element)->getBytepos(),
                                (*it_element)->getArraysize(),
                                (*it_element)->getByteorder(),
                                (*it_element)->getAlignment(),
                                *it_bu_found,
                                (*it_element)->getBitpos(),
                                (*it_element)->getNumBits(),
                                (*it_element)->getDescription(),
                                (*it_element)->getComment(),
                                (*it_element)->getArraySizeSource(),
                                (*it_element)->getConstantValue(),
                                (*it_element)->isMinValid(),
                                (*it_element)->getMinValue(),
                                (*it_element)->isMaxValid(),
                                (*it_element)->getMaxValue(),
                                (*it_element)->isDefaultValid(),
                                (*it_element)->getDefaultValue(),
                                (*it_element)->isScaleValid(),
                                (*it_element)->getScaleValue(),
                                (*it_element)->isOffsetValid(),
                                (*it_element)->getOffsetValue());
                        }
                        else
                        {
                            *it_element = new DDLElement(type,
                                (*it_element)->getName(),
                                (*it_element)->getBytepos(),
                                (*it_element)->getArraysize(),
                                (*it_element)->getByteorder(),
                                (*it_element)->getAlignment(),
                                *it_bu_found,
                                (*it_element)->getBitpos(),
                                (*it_element)->getNumBits(),
                                (*it_element)->getDescription(),
                                (*it_element)->getComment(),
                                (*it_element)->getArraySizeSource(),
                                (*it_element)->getConstantValue(),
                                (*it_element)->isMinValid(),
                                (*it_element)->getMinValue(),
                                (*it_element)->isMaxValid(),
                                (*it_element)->getMaxValue(),
                                (*it_element)->isDefaultValid(),
                                (*it_element)->getDefaultValue(),
                                (*it_element)->isScaleValid(),
                                (*it_element)->getScaleValue(),
                                (*it_element)->isOffsetValid(),
                                (*it_element)->getOffsetValue());
                        }
                    }
                }
            }
            (*it_struct)->setElements(elements);
        }
        _ddl_desc->refStructs(structs);
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLCloner::buildStreams()
    {
        if (NULL == _original_desc)
        {
            return ERR_NOT_INITIALIZED;
        }
        // clone the streams
        DDLStreamVec streams = _original_desc->getStreams();
        DDLComplexVec structs = _ddl_desc->getStructs();
        for (DDLStreamIt it_stream = streams.begin();
            streams.end() != it_stream; ++it_stream)
        {
            // find type
            DDLComplexIt it_found = std::find_if(structs.begin(),
                                                 structs.end(), DDLCompareFunctor<DDLComplex>((*it_stream)->getType()));
            if (structs.end() == it_found)
            {
                // type not found
                return ERR_NO_CLASS;
            }

            // clone the contained stream structs
            DDLStreamStructVec strm_structs = (*it_stream)->getStructs();
            DDLVersion version = (*it_stream)->getDDLVersion();
            *it_stream = new DDLStream(*it_found, (*it_stream)->getName(),
                (*it_stream)->getDescription(), strm_structs,
                (*it_stream)->getCreationLevel());
            (*it_stream)->setDDLVersion(version);

            for (DDLStreamStructIt it_struct = strm_structs.begin();
                strm_structs.end() != it_struct; ++it_struct)
            {
                // find type
                it_found = std::find_if(structs.begin(), structs.end(),
                                       DDLCompareFunctor<DDLComplex>((*it_struct)->getType()));
                if (structs.end() == it_found)
                {
                    // type not found
                    return ERR_NO_CLASS;
                }
                *it_struct = new DDLStreamStruct(*it_found,
                    (*it_struct)->getBytepos(), (*it_struct)->getName());
            }
            (*it_stream)->refStructs(strm_structs);
        }
        _ddl_desc->refStreams(streams);
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLCloner::setOriginal(const DDLDescription* original)
    {
        if (!original) { return ERR_POINTER; }
        _original_desc = original;
        return a_util::result::SUCCESS;
    }

    a_util::result::Result ddl::DDLCloner::buildStreamMetaTypes()
    {
        return ERR_NOT_IMPL; ///@todo
    }

}   // namespace ddl
