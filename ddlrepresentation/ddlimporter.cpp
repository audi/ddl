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

#include "ddlimporter.h"
#include <sstream>
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

#include "ddlversion.h"

namespace ddl
{
    //define all needed error types and values locally
    _MAKE_RESULT(-2, ERR_UNKNOWN)
    _MAKE_RESULT(-4, ERR_POINTER)
    _MAKE_RESULT(-5, ERR_INVALID_ARG)
    _MAKE_RESULT(-15, ERR_RESOURCE_IN_USE)
    _MAKE_RESULT(-18, ERR_NO_CLASS)
    _MAKE_RESULT(-20, ERR_NOT_FOUND)
    _MAKE_RESULT(-27, ERR_OPEN_FAILED)
    _MAKE_RESULT(-36, ERR_UNKNOWN_FORMAT)
    _MAKE_RESULT(-37, ERR_NOT_INITIALIZED)

    /**
    * Iterator type for cDOMElementList
    */
    typedef a_util::xml::DOMElementList::iterator tDOMElemIt;

    #define RETURN_IF_FAILED_AND_LOG_ERROR_STR_DDLIMPORTER(code, desc) \
    { \
    a_util::result::Result __err_code = code; \
    if (isFailed(__err_code)){ _fnLogOutput(( desc ), LOG_LVL_ERROR, __FILE__, (uint32_t) __LINE__, (code)); \
    ImporterMsg sError = {std::string(desc), importer_error}; \
    _errors.push_back(sError); \
    return (__err_code); }\
    }

    DDLImporter::DDLImporter(bool bBasicCheck, bool bSorted) : 
        _dom{},
        _ddl_desc{},
        _current_ref_ddl{},
        _unknown_units{},
        _unknown_structs{},
        _unknown_stream_meta_types{},
        _init_flag{},
        _creation_level{1},
        _full_check{},
        _errors{},
        _basic_check{bBasicCheck},
        _merge_defaults{true},
        _sorted{bSorted},
        _prefere_reference{true}
    {
    }

    DDLImporter::DDLImporter(const a_util::filesystem::Path& strFile, bool bBasicCheck, bool bSorted) :
        DDLImporter(bBasicCheck, bSorted)
    {
        _init_flag = isOk(setFile(strFile));
    }

    DDLImporter::DDLImporter(const char* strFile, bool bBasicCheck, bool bSorted) :
        DDLImporter(a_util::filesystem::Path(strFile), bBasicCheck, bSorted)
    {
    }

    DDLImporter::DDLImporter(int nCreationLevel, bool bBasicCheck, bool bSorted) : 
        DDLImporter(bBasicCheck, bSorted)
    {
        _creation_level = nCreationLevel;
    }

    DDLImporter& DDLImporter::operator=(DDLImporter other)
    {
        swap(*this, other);
        return *this;
    }

    DDLImporter::DDLImporter(DDLImporter&& other)
    {
        swap(*this, other);
    }

    DDLImporter::~DDLImporter()
    {
        // The DDL object does not get deleted because the caller/user of this
        // object is responsible for it.
        _ddl_desc = NULL;
        _current_ref_ddl = NULL;
        _unknown_units.deleteAll();
        _unknown_structs.deleteAll();
        _unknown_stream_meta_types.deleteAll();
    }

    void swap(DDLImporter& lhs, DDLImporter& rhs) noexcept
    {
        using std::swap;
        swap(lhs._basic_check, rhs._basic_check);
        swap(lhs._creation_level, rhs._creation_level);
        swap(lhs._current_ref_ddl, rhs._current_ref_ddl);
        swap(lhs._ddl_desc, rhs._ddl_desc);
        swap(lhs._dom, rhs._dom);
        swap(lhs._errors, rhs._errors);
        swap(lhs._full_check, rhs._full_check);
        swap(lhs._init_flag, rhs._init_flag);
        swap(lhs._merge_defaults, rhs._merge_defaults);
        swap(lhs._prefere_reference, rhs._prefere_reference);
        swap(lhs._sorted, rhs._sorted);
        swap(lhs._unknown_stream_meta_types, rhs._unknown_stream_meta_types);
        swap(lhs._unknown_structs, rhs._unknown_structs);
        swap(lhs._unknown_units, rhs._unknown_units);
    }

    std::string DDLImporter::getErrorDesc() const
    {
        // compatibility to cDDLChecker
        return getLastErrorDesc();
    }

    std::string DDLImporter::getLastErrorDesc() const
    {
        // return description of most recently added error element
        for (ImporterMsgList::const_reverse_iterator itMsgs = _errors.rbegin();
            _errors.rend() != itMsgs; ++itMsgs)
        {
            if (importer_error == itMsgs->severity)
            {
                return std::string(itMsgs->desc);
            }
        }
        return a_util::strings::empty_string;
    }

    void DDLImporter::printAllMessages(ImporterMsgSeverity eLeastSeverity /* = importer_info */)
    {
        for (ImporterMsgList::const_iterator itMsg = _errors.begin();
            _errors.end() != itMsg; ++itMsg)
        {
            switch (itMsg->severity)
            {
            case importer_error:
                {
                    LOG_ERROR(itMsg->desc.c_str());
                    break;
                }
            case importer_warning:
                {
                    if (importer_error != eLeastSeverity)
                    {
                        LOG_WARNING(itMsg->desc.c_str());
                    }
                    break;
                }
            case importer_info:
                {
                    if (importer_info == eLeastSeverity)
                    {
                        LOG_INFO(itMsg->desc.c_str());
                    }
                    break;
                }
            default:
                {
                    LOG_ERROR("Unknown message severity detected.");
                    break;
                }
            }
        }
    }

    ImporterMsgList DDLImporter::getAllMessages() const
    {
        return _errors;
    }

    DDLDescription * DDLImporter::getDDL() const
    {
        return _ddl_desc;
    }

    void DDLImporter::destroyDDL()
    {
        destroyDDL(_ddl_desc);
        _ddl_desc = NULL;
        // no DDL objects => no valid messages
        _errors.clear();
    }

    void DDLImporter::destroyDDL(DDLDescription *poDDL)
    {
        delete poDDL;
    }

    a_util::result::Result DDLImporter::setFile(const a_util::filesystem::Path& strFile)
    {
        _init_flag = _dom.load(strFile);
        if (!_init_flag)
        {
            ImporterMsg sError = {_dom.getLastError(), importer_error };
            _errors.push_back(sError);
            return ERR_OPEN_FAILED;
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLImporter::setXML(const std::string& strXML)
    {
        _init_flag = _dom.fromString(strXML);
        if (!_init_flag)
        {
            ImporterMsg sError = {_dom.getLastError(), importer_error};
            _errors.push_back(sError);
            LOG_ERROR(_dom.getLastError().c_str());
            return ERR_OPEN_FAILED;
        }
        return a_util::result::SUCCESS;
    }

    void DDLImporter::setCreationLevel(int const nLevel)
    {
        _creation_level = nLevel;
    }

    void DDLImporter::setFullCheckDescriptionMode(bool bFullCheck)
    {
        _full_check = bFullCheck;
    }

    void DDLImporter::setMergeDefaults(bool bMergeDefaults)
    {
        _merge_defaults = bMergeDefaults;
    }

    void DDLImporter::setPreferReferenceEntities(bool bPrefereReference)
    {
        _prefere_reference = bPrefereReference;
    }

    a_util::result::Result DDLImporter::createNew(const DDLVersion& requestedVersion /* = 0 */)
    {
        DDLVersion version = requestedVersion;
        if (version == DDLVersion::ddl_version_invalid)
        {
            version = DDLVersion::ddl_version_current;
        }
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        // The DDL object does not get deleted because the caller/user of this
        // object is responsible for it.
        _ddl_desc = NULL;
        // build up new DDL
        if (_full_check)
        {
            if (isFailed(buildHeader()) && _ddl_desc == NULL)
            {
                _ddl_desc = new DDLDescription(new DDLHeader(),
                                              DDLUnitVec(_sorted),
                                              DDLBaseunitVec(_sorted),
                                              DDLPrefixVec(_sorted),
                                              DDLDTVec(_sorted),
                                              DDLComplexVec(_sorted),
                                              DDLStreamVec(_sorted),
                                              DDLEnumVec(_sorted),
                                              _merge_defaults);
            }
            buildUnits();
            buildDatatypes();
            buildEnums();
            buildStructs();
            buildStreams();
            if (version >= DDLVersion::ddl_version_40) buildStreamMetaTypes();
            // check if any error occurred during build up new DDL
            for (ImporterMsgList::const_iterator itMsg = _errors.begin();
                _errors.end() != itMsg; ++itMsg)
            {
                if (itMsg->severity == importer_error)
                {
                    return ERR_UNKNOWN_FORMAT;
                }
            }
        }
        else
        {
            RETURN_IF_FAILED(buildHeader());
            RETURN_IF_FAILED(buildUnits());
            RETURN_IF_FAILED(buildDatatypes());
            RETURN_IF_FAILED(buildEnums());
            RETURN_IF_FAILED(buildStructs());
            RETURN_IF_FAILED(buildStreams());
            if (version >= DDLVersion::ddl_version_40) RETURN_IF_FAILED(buildStreamMetaTypes());
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLImporter::createPartial(const DDLDescription* poRefDDL, const DDLVersion& requestedVersion)
    {
        DDLVersion version = requestedVersion;
        if (!poRefDDL) { return ERR_POINTER; }
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        _current_ref_ddl = poRefDDL;
        _ddl_desc = NULL;
        std::string strElementType = _dom.getRoot().getName();

        if (strElementType == "adtf:ddl" ||
            strElementType == "description" ||
            strElementType == "units" ||
            strElementType == "datatypes" ||
            strElementType == "structs" ||
            strElementType == "streams" ||
            strElementType == "enums" ||
            strElementType == "streammetatypes" ||
            (strElementType != "datatype"
              && strElementType != "baseunit"
              && strElementType != "prefixes"
              && strElementType != "unit"
              && strElementType != "datatype"
              && strElementType != "struct"
              && strElementType != "stream"
              && strElementType != "enum"
              && strElementType != "streammetatype"))
        {
            a_util::xml::DOMElement oRoot = _dom.getRoot();
            a_util::xml::DOMElement oTmpElement;
            bool bValidElementFound = false;
            bool bFoundEnums = false;
            //this is to detect 2.0 MediaDescriptions
            //i.e. the device-tb etc. uses enums!! and did not raise the version number at all!!!
            if (_dom.findNode("//enums", oTmpElement))
            {
                bFoundEnums = true;
                if (version <  DDLVersion::ddl_version_20)
                {
                    version = DDLVersion::ddl_version_20;
                }
            }
            if (oRoot.findNode("header", oTmpElement))
            {
                RETURN_IF_FAILED(buildHeader());
                bValidElementFound = true;
            }
            else
            {
                _ddl_desc = DDLDescription::createDefault(version, _creation_level, _sorted);
            }
            //this is to detect 2.0 MediaDescriptions
            //i.e. the device-tb etc. uses enums!! and did not raise the version number at all!!!device-tb etc. uses enums!! and did not raise the version number at all!!!
            if (_ddl_desc && _ddl_desc->getHeader() && bFoundEnums)
            {
                if (_ddl_desc->getHeader()->getLanguageVersion() <  DDLVersion::ddl_version_20)
                {
                    _ddl_desc->getHeader()->setLanguageVersion(DDLVersion::ddl_version_20);
                }
            }
            if (_dom.findNode("//units", oTmpElement))
            {
                RETURN_IF_FAILED(buildUnits());
                bValidElementFound = true;
            }
            if (_dom.findNode("//datatypes", oTmpElement))
            {
                RETURN_IF_FAILED(buildDatatypes());
                bValidElementFound = true;
            }
            if (_dom.findNode("//enums", oTmpElement))
            {
                RETURN_IF_FAILED(buildEnums());
                bValidElementFound = true;
            }
            if (_dom.findNode("//structs", oTmpElement))
            {
                RETURN_IF_FAILED(buildStructs());
                bValidElementFound = true;
            }
            if (_dom.findNode("//streams", oTmpElement))
            {
                RETURN_IF_FAILED(buildStreams());
                bValidElementFound = true;
            }
            if (_dom.findNode("//streammetatypes", oTmpElement))
            {
                RETURN_IF_FAILED(buildStreamMetaTypes());
                bValidElementFound = true;
            }

            _current_ref_ddl = NULL;
            if (!bValidElementFound)
            {
                return ERR_UNKNOWN_FORMAT;
            }
            return a_util::result::SUCCESS;
        }
       // _current_ref_ddl = NULL;
        _ddl_desc = DDLDescription::createDefault(version, _creation_level, _sorted);
        DDLBaseunit * poBUTmp = NULL;
        if (strElementType == "baseunit")
        {
            RETURN_IF_FAILED(buildSingleBaseunit(&poBUTmp, _dom.getRoot()));
            _ddl_desc->addBaseunit(poBUTmp);
            return a_util::result::SUCCESS;
        }
        DDLPrefix * poPrefixTmp = NULL;
        if (strElementType == "prefixes")
        {
            RETURN_IF_FAILED(buildSinglePrefix(&poPrefixTmp, _dom.getRoot()));
            _ddl_desc->addPrefix(poPrefixTmp);
            return a_util::result::SUCCESS;
        }
        DDLUnit * poUnitTmp = NULL;
        if (strElementType == "unit")
        {
            RETURN_IF_FAILED(buildSingleUnit(&poUnitTmp, _dom.getRoot()));
            _ddl_desc->addUnit(poUnitTmp);
            return a_util::result::SUCCESS;
        }
        if (strElementType == "datatype")
        {
            DDLDataType * poDTTmp = NULL;
            RETURN_IF_FAILED(buildSingleDatatype(&poDTTmp, _dom.getRoot()));
            _ddl_desc->addDatatype(poDTTmp);
            return a_util::result::SUCCESS;
        }
        if (strElementType == "enum")
        {
            DDLEnum * poEnumTmp = NULL;
            RETURN_IF_FAILED(buildSingleEnum(&poEnumTmp, _dom.getRoot()));
            _ddl_desc->addEnum(poEnumTmp);
            return a_util::result::SUCCESS;
        }
        if (strElementType == "struct")
        {
            DDLComplex * poStructTmp = NULL;
            RETURN_IF_FAILED(buildSingleStruct(&poStructTmp, _dom.getRoot()));
            _ddl_desc->addStruct(poStructTmp);
            return a_util::result::SUCCESS;
        }
        if (strElementType == "stream")
        {
            DDLStream * poStreamTmp = NULL;
            RETURN_IF_FAILED(buildSingleStream(&poStreamTmp, _dom.getRoot()));
            _ddl_desc->addStream(poStreamTmp);
            return a_util::result::SUCCESS;
        }
        if (strElementType == "streammetatype")
        {
            RETURN_IF_FAILED(buildSingleStreamMetaType(_dom.getRoot(),
                                                       _ddl_desc->getStreamMetaTypes()));
            return a_util::result::SUCCESS;
        }
        return ERR_UNKNOWN_FORMAT;
    }

    a_util::result::Result DDLImporter::buildHeader()
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        a_util::xml::DOMElement oHeaderElement;
        a_util::xml::DOMElement oTmpElement;
        a_util::result::Result nResult = a_util::result::SUCCESS;
        if (!_dom.findNode("//header", oHeaderElement))
        {
            pushMessage(std::string("DDL does not contain a header."), importer_error);
            return ERR_OPEN_FAILED;
        }
        else
        {
            std::string strVersion = a_util::strings::empty_string;
            if (!oHeaderElement.findNode("language_version", oTmpElement))
            {
                pushMessage(std::string("DDL does not contain a language_version in header"), importer_error);
                nResult = ERR_NOT_FOUND;
                if (false == _full_check)
                {
                    return nResult;
                }
            }
            else
            {
                strVersion = oTmpElement.getData();
                a_util::strings::trim(strVersion);
            }
            std::string strAuthor = a_util::strings::empty_string;
            if (!oHeaderElement.findNode("author", oTmpElement))
            {
                pushMessage(std::string("DDL does not contain an author in header."), importer_error);
                nResult = ERR_NOT_FOUND;
                if (false == _full_check)
                {
                    return nResult;
                }
            }
            else
            {
                strAuthor = oTmpElement.getData();
            }
            a_util::datetime::Date sDateCreation(1900, 1, 1);
            if (!oHeaderElement.findNode("date_creation", oTmpElement))
            {
                pushMessage(std::string("DDL does not contain a creation date in header."), importer_error);
                nResult = ERR_NOT_FOUND;
                if (false == _full_check)
                {
                    return nResult;
                }
            }
            else
            {
                sDateCreation = DDLHeader::dateFromString(oTmpElement.getData());
            }
            a_util::datetime::Date sDateChange(1900, 1, 1);
            if (!oHeaderElement.findNode("date_change", oTmpElement))
            {
                pushMessage(std::string("DDL does not contain a change date in header."), importer_error);
                nResult = ERR_NOT_FOUND;
                if (false == _full_check)
                {
                    return nResult;
                }
            }
            else
            {
                 sDateChange = DDLHeader::dateFromString(oTmpElement.getData());
            }
            std::string strDescription = a_util::strings::empty_string;
            if (!oHeaderElement.findNode("description", oTmpElement))
            {
                pushMessage(std::string("DDL does not contain a description in header."), importer_error);
                nResult = ERR_NOT_FOUND;
                if (false == _full_check)
                {
                    return nResult;
                }
            }
            else
            {
               strDescription = oTmpElement.getData();
            }
            // do not continue with header, if any header property could not found (all properties are mandatory!)
            if (isFailed(nResult))
            {
                DDLExtDeclarationVec vecExtDecls;
                _ddl_desc = new DDLDescription(new DDLHeader(DDLVersion::fromString(strVersion),
                                                             strAuthor, sDateCreation, sDateChange, strDescription, vecExtDecls),
                                              DDLUnitVec(_sorted),
                                              DDLBaseunitVec(_sorted),
                                              DDLPrefixVec(_sorted),
                                              DDLDTVec(_sorted),
                                              DDLComplexVec(_sorted),
                                              DDLStreamVec(_sorted),
                                              DDLEnumVec(_sorted),
                                              _merge_defaults);
                return nResult;
            }
            a_util::xml::DOMElementList oExtDeclarations;
            oHeaderElement.findNodes("ext_declaration", oExtDeclarations);
            DDLExtDeclarationVec vecExtDecls;
            for (tDOMElemIt it = oExtDeclarations.begin(); oExtDeclarations.end() != it; ++it)
            {
                a_util::result::Result nRes;
                if (!it->hasAttribute("key"))
                {
                    pushMessage(std::string("There is an external declaration without key in header."), importer_error);
                    nRes = ERR_UNKNOWN_FORMAT;
                    if (false == _full_check)
                    {
                        return nRes;
                    }
                }
                else
                {
                    if (!it->hasAttribute("value"))
                    {
                        pushMessage(a_util::strings::format("The external declaration with key '%s' has no value.",
                            it->getAttribute("key").c_str()), importer_error);
                        nRes = ERR_UNKNOWN_FORMAT;
                        if (false == _full_check)
                        {
                            return nRes;
                        }
                    }
                }
                if (!it->hasAttribute("value"))
                {
                    pushMessage(std::string("There is an external declaration without value in header."), importer_error);
                    nRes = ERR_UNKNOWN_FORMAT;
                    if (false == _full_check)
                    {
                        return nRes;
                    }
                }
                if (isFailed(nRes))
                {
                    nResult = nRes;
                    if (false == _full_check)
                    {
                        return nResult;
                    }
                    // do not create new DDLExtDeclaration, if any key property could not found (all properties are mandatory!)
                    continue;
                }
                vecExtDecls.push_back(new DDLExtDeclaration(it->getAttribute("key"),
                    it->getAttribute("value")));
            }
            _ddl_desc = new DDLDescription(new DDLHeader(DDLVersion::fromString(strVersion),
                                                         strAuthor, sDateCreation, sDateChange, strDescription, vecExtDecls),
                                          DDLUnitVec(_sorted),
                                          DDLBaseunitVec(_sorted),
                                          DDLPrefixVec(_sorted),
                                          DDLDTVec(_sorted),
                                          DDLComplexVec(_sorted),
                                          DDLStreamVec(_sorted),
                                          DDLEnumVec(_sorted),
                                          _merge_defaults);
            if (isFailed(nResult))
            {
                return nResult;
            }
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLImporter::buildSingleBaseunit(DDLBaseunit ** ppoNewBU, const a_util::xml::DOMElement& oBUElement)
    {
        if (!ppoNewBU) { return ERR_POINTER; }
        *ppoNewBU = NULL;
        std::string name = oBUElement.getAttribute("name");
        a_util::result::Result nResult = a_util::result::SUCCESS;
        if (!oBUElement.hasAttribute("name"))
        {
            pushMessage(a_util::strings::format("Attribute 'name' is missing in 'baseunit' tag."),
                importer_error);
            nResult = ERR_UNKNOWN_FORMAT;
            if (false == _full_check)
            {
                return nResult;
            }
        }
        if (!oBUElement.hasAttribute("symbol"))
        {
            if (!name.empty())
            {
                pushMessage(a_util::strings::format("Attribute 'symbol' is missing in 'baseunit' tag '%s'.",
                    name.c_str()), importer_error);
            }
            else
            {
                pushMessage(a_util::strings::format("Attribute 'symbol' is missing in 'baseunit' tag."),
                    importer_error);
            }
            nResult = ERR_UNKNOWN_FORMAT;
            if (false == _full_check)
            {
                return nResult;
            }
        }
        if (!oBUElement.hasAttribute("description"))
        {
            if (!name.empty())
            {
                pushMessage(a_util::strings::format("Attribute 'description' is missing in 'baseunit' tag '%s'.",
                    name.c_str()), importer_error);
            }
            else
            {
                pushMessage(a_util::strings::format("Attribute 'description' is missing in 'baseunit' tag."),
                    importer_error);
            }
            nResult = ERR_UNKNOWN_FORMAT;
            if (false == _full_check)
            {
                return nResult;
            }
        }
        // do not create new DDLBaseunit, if any base unit property could not found (all properties are mandatory!)
        if (isFailed(nResult))
        {
            return nResult;
        }
        *ppoNewBU = new DDLBaseunit(oBUElement.getAttribute("name"), oBUElement.getAttribute("symbol"),
            oBUElement.getAttribute("description"), _creation_level);
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLImporter::buildSinglePrefix(DDLPrefix ** ppoNewPrefix,
                                            const a_util::xml::DOMElement& oPrefixElement)
    {
        if (!ppoNewPrefix) { return ERR_POINTER; }
        *ppoNewPrefix = NULL;
        std::string name = oPrefixElement.getAttribute("name");
        a_util::result::Result nResult = a_util::result::SUCCESS;
        if (!oPrefixElement.hasAttribute("name"))
        {
            pushMessage(a_util::strings::format("Attribute 'name' is missing in 'prefix' tag."),
                importer_error);
            nResult = ERR_UNKNOWN_FORMAT;
            if (false == _full_check)
            {
                return nResult;
            }
        }
        if (!oPrefixElement.hasAttribute("symbol"))
        {
            if (!name.empty())
            {
                pushMessage(a_util::strings::format("Attribute 'symbol' is missing in 'prefix' tag '%s'.",
                    name.c_str()), importer_error);
            }
            else
            {
                pushMessage(a_util::strings::format("Attribute 'symbol' is missing in 'prefix' tag."),
                    importer_error);
            }
            nResult = ERR_UNKNOWN_FORMAT;
            if (false == _full_check)
            {
                return nResult;
            }
        }
        if (!oPrefixElement.hasAttribute("power"))
        {
            if (!name.empty())
            {
                pushMessage(a_util::strings::format("Attribute 'power' is missing in 'prefix' tag '%s'.",
                    name.c_str()), importer_error);
            }
            else
            {
                pushMessage(a_util::strings::format("Attribute 'power' is missing in 'prefix' tag."),
                    importer_error);
            }
            nResult = ERR_UNKNOWN_FORMAT;
            if (false == _full_check)
            {
                return nResult;
            }
        }
        // do not create new DDLPrefix, if any prefix property could not found (all properties are mandatory!)
        if (isFailed(nResult))
        {
            return nResult;
        }
       *ppoNewPrefix = new DDLPrefix(oPrefixElement.getAttribute("name"),
           oPrefixElement.getAttribute("symbol"), a_util::strings::toInt32(oPrefixElement.getAttribute("power")),
           _creation_level);
       return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLImporter::buildSingleUnit(DDLUnit ** ppoNewUnit,
                                          const a_util::xml::DOMElement& oUnitElement)
    {
        if (!ppoNewUnit) { return ERR_POINTER; }
        *ppoNewUnit = NULL;

        DDLBaseunitVec vecDDLBaseunits = _ddl_desc->getBaseunits();
        DDLPrefixVec vecDDLPrefixes = _ddl_desc->getPrefixes();
        DDLUnitVec vecDDLUnits = _ddl_desc->getUnits();

        DDLBaseunitVec vecDDLBaseunitRefs;
        DDLPrefixVec vecDDLPrefixeRefs;
        DDLUnitVec vecDDLUnitRefs;

        if (_current_ref_ddl != NULL)
        {
            vecDDLUnitRefs = _current_ref_ddl->getUnits();
            vecDDLPrefixeRefs = _current_ref_ddl->getPrefixes();
            vecDDLBaseunitRefs = _current_ref_ddl->getBaseunits();
        }

        a_util::result::Result nResult = a_util::result::SUCCESS;
        std::string name = oUnitElement.getAttribute("name");
        if (name.empty())
        {
            pushMessage(a_util::strings::format("Attribute 'name' is missing in 'unit' tag."),
                importer_error);
            nResult = ERR_UNKNOWN_FORMAT;
            if (false == _full_check)
            {
                return nResult;
            }
        }
        a_util::xml::DOMElement oTmpElement;
        std::string strNumerator = a_util::strings::empty_string;
        if (!oUnitElement.findNode("numerator", oTmpElement))
        {
            if (!name.empty())
            {
                pushMessage(a_util::strings::format("Attribute 'numerator' is missing in 'unit' tag '%s'.",
                    name.c_str()), importer_error);
            }
            else
            {
                pushMessage(a_util::strings::format("Attribute 'numerator' is missing in 'unit' tag."),
                    importer_error);
            }
            nResult = ERR_UNKNOWN_FORMAT;
            if (false == _full_check)
            {
                return nResult;
            }
        }
        else
        {
            strNumerator = oTmpElement.getData();
        }

        std::string strDenominator = a_util::strings::empty_string;
        if (!oUnitElement.findNode("denominator", oTmpElement))
        {
            if (!name.empty())
            {
                pushMessage(a_util::strings::format("Attribute 'denominator' is missing in 'unit' tag '%s'.",
                    name.c_str()), importer_error);
            }
            else
            {
                pushMessage(a_util::strings::format("Attribute 'denominator' is missing in 'unit' tag."),
                    importer_error);
            }
            nResult = ERR_UNKNOWN_FORMAT;
            if (false == _full_check)
            {
                return nResult;
            }
        }
        else
        {
            strDenominator = oTmpElement.getData();
        }

        double fOffset = 0.0F;
        if (!oUnitElement.findNode("offset", oTmpElement))
        {
            if (!name.empty())
            {
                pushMessage(a_util::strings::format("Attribute 'offset' is missing in 'unit' tag '%s'.",
                    name.c_str()), importer_error);
            }
            else
            {
                pushMessage(a_util::strings::format("Attribute 'offset' is missing in 'unit' tag."),
                    importer_error);
            }
            nResult = ERR_UNKNOWN_FORMAT;
            if (false == _full_check)
            {
                return nResult;
            }
        }
        else
        {
            fOffset = static_cast<double> (a_util::strings::toDouble(oTmpElement.getData()));
        }

        a_util::xml::DOMElementList oRefUnits;
        oUnitElement.findNodes("refUnit", oRefUnits);
        DDLRefUnitVec vecDDLRefUnits;
        a_util::result::Result nRes = a_util::result::SUCCESS;
        for (tDOMElemIt itRU = oRefUnits.begin();
            oRefUnits.end() != itRU; ++itRU)
        {
            std::string strRefUnitName = a_util::strings::empty_string;
            if (itRU->hasAttribute("name"))
            {
                strRefUnitName = itRU->getAttribute("name");
            }
            nRes = a_util::result::SUCCESS;
            if (!itRU->hasAttribute("name") ||
                !itRU->hasAttribute("power") ||
                !itRU->hasAttribute("prefix"))
            {
                std::transform(vecDDLRefUnits.begin(), vecDDLRefUnits.end(), vecDDLRefUnits.begin(), DDLDescription::deleteChild<DDLRefUnit>);

                if (!itRU->hasAttribute("name"))
                {
                    pushMessage(a_util::strings::format("Attribute 'name' is missing in 'refUnit' tag."),
                        importer_error);
                    nRes = ERR_UNKNOWN_FORMAT;
                    if (false == _full_check)
                    {
                        return nRes;
                    }
                }
                if (!itRU->hasAttribute("power"))
                {
                    if (!strRefUnitName.empty())
                    {
                        pushMessage(a_util::strings::format("Attribute 'power' is missing in 'refUnit' tag '%s'.",
                            strRefUnitName.c_str()), importer_error);
                    }
                    else
                    {
                        pushMessage(a_util::strings::format("Attribute 'power' is missing in 'refUnit' tag."),
                            importer_error);
                    }
                    nRes = ERR_UNKNOWN_FORMAT;
                    if (false == _full_check)
                    {
                        return nRes;
                    }
                }
                if (!itRU->hasAttribute("prefix"))
                {
                    if (!strRefUnitName.empty())
                    {
                        pushMessage(a_util::strings::format("Attribute 'prefix' is missing in 'refUnit' tag '%s'.",
                            strRefUnitName.c_str()), importer_error);
                    }
                    else
                    {
                        pushMessage(a_util::strings::format("Attribute 'prefix' is missing in 'refUnit' tag."),
                            importer_error);
                    }
                    nRes = ERR_UNKNOWN_FORMAT;
                    if (false == _full_check)
                    {
                        return nRes;
                    }
                }
                if (isFailed(nRes))
                {
                    nResult = nRes;
                    if (false == _full_check)
                    {
                        return nRes;
                    }
                    // do not continue with this ref unit, if any property could not found (all properties are mandatory!)
                    continue;
                }
            }

            DDLPrefix* pPrefix = tryToFind(itRU->getAttribute("prefix"), vecDDLPrefixeRefs, vecDDLPrefixes );
            if (pPrefix == NULL)
            {
                // not found in prefixes vector => not defined
                if (!name.empty() && !strRefUnitName.empty())
                {
                    pushMessage(a_util::strings::format("RefUnit '%s' in unit '%s' references to unknown prefix '%s'",
                        strRefUnitName.c_str(), name.c_str(), itRU->getAttribute("prefix").c_str()), importer_error);
                }
                else if (!name.empty())
                {
                    pushMessage(a_util::strings::format("RefUnit in unit '%s' references to unknown prefix '%s'",
                        name.c_str(), itRU->getAttribute("prefix").c_str()), importer_error);
                }
                else
                {
                    pushMessage(a_util::strings::format("RefUnit in unit references to unknown prefix '%s'",
                        itRU->getAttribute("prefix").c_str()), importer_error);
                }
                std::transform(vecDDLRefUnits.begin(), vecDDLRefUnits.end(), vecDDLRefUnits.begin(), DDLDescription::deleteChild<DDLRefUnit>);
                nRes = ERR_NO_CLASS;
                if (false == _full_check)
                {
                    return nRes;
                }
            }

            std::string strAttr = itRU->getAttribute("name");
            DDLBaseunit* pBaseUnit = tryToFind(strAttr, vecDDLBaseunitRefs, vecDDLBaseunits);
            if (pBaseUnit == NULL)
            {
                DDLUnit* pUnit = tryToFind(strAttr, vecDDLUnitRefs, vecDDLUnits);
                if (pUnit == NULL)
                {
                    pUnit = tryToFind(strAttr, _unknown_units, vecDDLUnits);
                    if (pUnit == NULL)
                    {
                        // => create placeholder object, mark Unit object
                        // as unknown, and continue
                        pUnit = new DDLUnit(strAttr, "1", "1", 0.0f);
                        _unknown_units.insert(pUnit);
                        vecDDLRefUnits.push_back(new DDLRefUnit(pUnit,
                                                                 a_util::strings::toInt32(itRU->getAttribute("power")),
                                                                 pPrefix));
                    }
                    else
                    {
                        // already marked as unknown
                        vecDDLRefUnits.push_back(new DDLRefUnit(pUnit,
                                                                 a_util::strings::toInt32(itRU->getAttribute("power")),
                                                                 pPrefix));
                    }
                }
                else
                {
                    // known Unit
                    vecDDLRefUnits.push_back(new DDLRefUnit(pUnit,
                                                             a_util::strings::toInt32(itRU->getAttribute("power")),
                                                             pPrefix));
                }
            }
            else
            {
                vecDDLRefUnits.push_back(new DDLRefUnit(pBaseUnit,
                                                         a_util::strings::toInt32(itRU->getAttribute("power")),
                                                         pPrefix));
            }
            if (isFailed(nRes))
            {
                nResult = nRes;
                if (false == _full_check)
                {
                    return nResult;
                }
            }
        }
        // do not create new DDLUnit, if any unit property could not found (all properties are mandatory!)
        if (isFailed(nResult))
        {
            return nResult;
        }
        DDLUnitIt itUnknown = std::find_if(_unknown_units.begin(), _unknown_units.end(), DDLCompareFunctor<>(name));
        if (_unknown_units.end() == itUnknown)
        {
            // create new object
            *ppoNewUnit = new DDLUnit(name, strNumerator, strDenominator, fOffset, DDLRefUnitVec(), _creation_level);

        }
        else
        {
            // use placeholder object
            (*itUnknown)->create(name, strNumerator, strDenominator, fOffset, DDLRefUnitVec(), _creation_level);
            *ppoNewUnit = *itUnknown;
            // unmark object
            _unknown_units.erase(itUnknown);
        }
        (*ppoNewUnit)->setRefUnits(vecDDLRefUnits);

        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLImporter::buildUnits()
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        a_util::xml::DOMElement oUnits;
        if (!_dom.findNode("//units", oUnits))
        {
            // as units are not supported in ADTF => no error
            pushMessage(std::string("DDL does not contain the 'units' element."),
                importer_warning);
            return a_util::result::SUCCESS;
        }
        else
        {
            a_util::result::Result nResult = a_util::result::SUCCESS;

            // build up base-unit objects
            a_util::xml::DOMElementList oBaseunits;
            if (!oUnits.findNodes("baseunit", oBaseunits))
            {
                // no error
            }
            else
            {
                DDLBaseunitVec vecDDLBaseunits(_sorted);
                a_util::result::Result nRes = a_util::result::SUCCESS;
                for (tDOMElemIt itBU = oBaseunits.begin();
                    oBaseunits.end() != itBU; ++itBU)
                {
                    DDLBaseunit * poBUTmp = NULL;
                    nRes = buildSingleBaseunit(&poBUTmp, *itBU);
                    if (isFailed(nRes))
                    {
                        vecDDLBaseunits.deleteAll();
                        nResult = nRes;
                        if (false == _full_check)
                        {
                            return nResult;
                        }
                        continue;
                    }
                    if (_basic_check && vecDDLBaseunits.find(poBUTmp->getName()))
                    {
                        pushMessage(a_util::strings::format("The baseunit '%s' is specified more than once.",
                                                    poBUTmp->getName().c_str()), importer_error);
                        nResult = ERR_RESOURCE_IN_USE;
                        if (false == _full_check)
                        {
                            return nResult;
                        }
                    }
                    vecDDLBaseunits.insert(poBUTmp);
                }
                _ddl_desc->refBaseunits(vecDDLBaseunits);
            }

            // build up prefix objects
            a_util::xml::DOMElementList oPrefixes;
            if (!oUnits.findNodes("prefixes", oPrefixes))
            {
                // => no error
            }
            else
            {
                DDLPrefixVec vecDDLPrefixes(_sorted);
                a_util::result::Result nRes = a_util::result::SUCCESS;
                for (tDOMElemIt itP = oPrefixes.begin();
                    oPrefixes.end() != itP; ++itP)
                {
                    DDLPrefix * poPrefixTmp = NULL;
                    nRes = buildSinglePrefix(&poPrefixTmp, *itP);
                    if (isFailed(nRes))
                    {
                        vecDDLPrefixes.deleteAll();
                        nResult = nRes;
                        if (false == _full_check)
                        {
                            return nResult;
                        }
                        continue;
                    }
                    if (_basic_check && vecDDLPrefixes.find(poPrefixTmp->getName()))
                    {
                        pushMessage(a_util::strings::format("The prefix '%s' is specified more than once.",
                                                    poPrefixTmp->getName().c_str()), importer_error);
                        nResult = ERR_RESOURCE_IN_USE;
                        if (false == _full_check)
                        {
                            return nResult;
                        }
                    }
                    vecDDLPrefixes.insert(poPrefixTmp);
                }
                _ddl_desc->refPrefixes(vecDDLPrefixes);
            }

            // build up unit objects
            a_util::xml::DOMElementList oUnitList;
            if (!oUnits.findNodes("unit", oUnitList))
            {
                // => no error
            }
            else
            {
                DDLUnitVec vecDDLUnits(_sorted);
                a_util::result::Result nRes = a_util::result::SUCCESS;
                _ddl_desc->cloneUnits(vecDDLUnits);
                for (tDOMElemIt itU = oUnitList.begin(); oUnitList.end() != itU; ++itU)
                {
                    DDLUnit * poUnitTmp = NULL;
                    nRes = buildSingleUnit(&poUnitTmp, *itU);
                    if (isFailed(nRes))
                    {
                        nResult = nRes;
                        if (false == _full_check)
                        {
                            return nResult;
                        }
                        continue;
                    }
                    if (_basic_check && _ddl_desc->getUnitByName(poUnitTmp->getName()))
                    {
                        pushMessage(a_util::strings::format("The unit '%s' is specified more than once.",
                                                    poUnitTmp->getName().c_str()), importer_error);
                        nResult = ERR_RESOURCE_IN_USE;
                        if (false == _full_check)
                        {
                            return nResult;
                        }
                    }
                    _ddl_desc->addUnit(poUnitTmp);
                }
            }
            for (DDLUnitIt itUnknown = _unknown_units.begin();
                _unknown_units.end() != itUnknown; ++itUnknown)
            {
                pushMessage(a_util::strings::format("The unit '%s' was referenced but not specified.",
                    (*itUnknown)->getName().c_str()), importer_error);
                nResult = ERR_UNKNOWN;
                if (false == _full_check)
                {
                    return nResult;
                }
            }
            if (!_unknown_units.empty())
            {
                std::stringstream logUnspecifiedStructs;
                logUnspecifiedStructs<< "The following units are unknown in this description: ";                
                for (DDLUnitIt itUnknown = _unknown_units.begin();
                    _unknown_units.end() != itUnknown; ++itUnknown)
                {
                    logUnspecifiedStructs << " " << (*itUnknown)->getName();
                    if (false == _full_check)
                    {
                        logUnspecifiedStructs << ".";
                        pushMessage(logUnspecifiedStructs.str(), importer_error);
                        nResult = ERR_UNKNOWN;
                        return nResult;
                    }
                    logUnspecifiedStructs << ",";
                }
                logUnspecifiedStructs.seekp(-1, logUnspecifiedStructs.cur);
                logUnspecifiedStructs << ".";
                pushMessage(logUnspecifiedStructs.str(), importer_error);
                nResult = ERR_UNKNOWN;
            }
            if (isFailed(nResult))
            {
                return nResult;
            }
        }
        return a_util::result::SUCCESS;
    }

a_util::result::Result DDLImporter::buildSingleDatatype(DDLDataType ** ppoNewDatatype, const a_util::xml::DOMElement& oDTElement)
{
    if (!ppoNewDatatype) { return ERR_POINTER; }
    *ppoNewDatatype = NULL;
    a_util::result::Result nResult = a_util::result::SUCCESS;
    if (!oDTElement.hasAttribute("size"))
    {
        pushMessage(a_util::strings::format("Attribute 'size' is missing in 'datatype' tag."),
            importer_error);
        nResult = ERR_UNKNOWN_FORMAT;
        if (false == _full_check)
        {
            return nResult;
        }
    }
    std::string name = a_util::strings::empty_string;
    if (_ddl_desc->getHeader()->getLanguageVersion() <= DDLVersion::ddl_version_11)
    {
        if (!oDTElement.hasAttribute("type"))
        {
            if (oDTElement.hasAttribute("name"))
            {
                //NF: ADTF ist robust!!
                name = oDTElement.getAttribute("name");
            }
            else
            {
                pushMessage(a_util::strings::format("Attribute 'type' is missing in 'datatype' tag."),
                    importer_error);
                nResult = ERR_UNKNOWN_FORMAT;
                if (false == _full_check)
                {
                    return nResult;
                }
            }
        }
        else
        {
            name = oDTElement.getAttribute("type");
        }
    }
    else
    {
        if (!oDTElement.hasAttribute("name"))
        {
            pushMessage(a_util::strings::format("Attribute 'name' is missing in 'datatype' tag."),
                importer_error);
            nResult = ERR_UNKNOWN_FORMAT;
            if (false == _full_check)
            {
                return nResult;
            }
        }
        else
        {
            name = oDTElement.getAttribute("name");
        }
    }
    std::string strDescription = a_util::strings::empty_string;
    if (oDTElement.hasAttribute("description"))
    {
        strDescription = oDTElement.getAttribute("description");
    }
    unsigned int uiArraysize = 1;
    if (oDTElement.hasAttribute("arraysize"))
    {
        uiArraysize = static_cast<unsigned int>
            (a_util::strings::toUInt32(oDTElement.getAttribute("arraysize")));
    }
    DDLAlignment::AlignmentType eAlignment = DDLAlignment::e1;
    if (oDTElement.hasAttribute("alignment"))
    {
        eAlignment = DDLAlignment::fromString(oDTElement.getAttribute("alignment"));
        if (eAlignment == DDLAlignment::e_invalid)
        {
            pushMessage(a_util::strings::format("Attribute 'alignment' is out of range in 'datatype' tag '%s'.",
                name.c_str()), importer_error);
            nResult = ERR_INVALID_ARG;
            if (false == _full_check)
            {
                return nResult;
            }
        }
    }
    if (oDTElement.hasAttribute("size"))
    {
        std::string s = oDTElement.getAttribute("size");
        if (!a_util::strings::isInt32(s) || a_util::strings::toInt32(s) <= 0)
        {
            pushMessage(a_util::strings::format("Attribute 'size' is out of range in 'datatype' tag '%s'.",
                name.c_str()), importer_error);
            nResult = ERR_UNKNOWN_FORMAT;
            if (false == _full_check)
            {
                return nResult;
            }
        }
    }
    std::string strMinVal;
    bool bMinValid = false;
    if (oDTElement.hasAttribute("min"))
    {
        strMinVal = oDTElement.getAttribute("min");
        bMinValid = true;
    }
    std::string strMaxVal;
    bool bMaxValid = false;
    if (oDTElement.hasAttribute("max"))
    {
        strMaxVal = oDTElement.getAttribute("max");
        bMaxValid = true;
    }
    IDDLUnit*     pUnitRef  = NULL;
    if (oDTElement.hasAttribute("unit"))
    {
        DDLBaseunit* pBaseUnit = NULL;
        DDLUnit*     pUnit     = NULL;

        DDLBaseunitVec vecDDLBaseunits = _ddl_desc->getBaseunits();
        DDLUnitVec vecDDLUnits         = _ddl_desc->getUnits();

        DDLUnitVec     vecDDLUnitRefs;
        DDLBaseunitVec vecDDLBaseunitRefs;

        if (_current_ref_ddl)
        {
            vecDDLBaseunitRefs     = _current_ref_ddl->getBaseunits();
            vecDDLUnitRefs         = _current_ref_ddl->getUnits();
        }

        std::string strAttr = oDTElement.getAttribute("unit");
        pBaseUnit = tryToFind(strAttr, vecDDLBaseunitRefs, vecDDLBaseunits);
        if (pBaseUnit == NULL)
        {
            pUnit = tryToFind(strAttr, vecDDLUnitRefs, vecDDLUnits);
            if (pUnit == NULL)
            {
                pushMessage(a_util::strings::format("No 'unit' or 'refUnit' found with name='%s' defined in datatype '%s'.",
                    strAttr.c_str(), name.c_str()), importer_error);
                nResult = ERR_NO_CLASS;
                if (false == _full_check)
                {
                    return nResult;
                }
            }
            else
            {
                pUnitRef = (IDDLUnit*) pUnit;
            }
        }
        else
        {
            pUnitRef = (IDDLUnit*) pBaseUnit;
        }
    }
    *ppoNewDatatype = new DDLDataType(name,
        static_cast<unsigned int>(a_util::strings::toUInt32(oDTElement.getAttribute("size"))),
        pUnitRef,
        strDescription,
        eAlignment,
        uiArraysize,
        _creation_level,
        a_util::strings::empty_string,
        bMinValid,
        strMinVal,
        bMaxValid,
        strMaxVal);

    // Add version information for behavior differentiation
    DDLVersion language_version = DDLVersion::getDefaultVersion();
    DDLHeader* pHeader = _ddl_desc->getHeader();
    if (NULL != pHeader)
    {
        language_version = pHeader->getLanguageVersion();
    }
    (*ppoNewDatatype)->setDDLVersion(language_version);

    return a_util::result::SUCCESS;
}

    a_util::result::Result DDLImporter::buildDatatypes()
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        a_util::xml::DOMElement oDatatypesNode;
        a_util::result::Result nResult = a_util::result::SUCCESS;
        if (!_dom.findNode("//datatypes", oDatatypesNode))
        {
            pushMessage(std::string("DDL does not contain the 'datatypes' element"),
                importer_error);
            return ERR_NOT_FOUND;
        }
        a_util::xml::DOMElementList oDatatypes;
        oDatatypesNode.findNodes("datatype", oDatatypes);
        DDLDTVec vecDDLDataTypes(_sorted);
        for (tDOMElemIt it = oDatatypes.begin(); oDatatypes.end() != it; ++it)
        {
            DDLDataType * poDTTmp = NULL;
            a_util::result::Result nRes = buildSingleDatatype(&poDTTmp, *it);
            if (isFailed(nRes))
            {
                vecDDLDataTypes.deleteAll();
                nResult = nRes;
                if (false == _full_check)
                {
                    return nResult;
                }
                continue;
            }
            if (_basic_check && vecDDLDataTypes.find(poDTTmp->getName()))
            {
                pushMessage(a_util::strings::format("The datatype '%s' is specified more than once.",
                                            poDTTmp->getName().c_str()),
                            importer_error);
                nResult = ERR_RESOURCE_IN_USE;
                if (false == _full_check)
                {
                    return nResult;
                }
            }
            vecDDLDataTypes.insert(poDTTmp);
        }
        _ddl_desc->refDatatypes(vecDDLDataTypes);
        if (isFailed(nResult))
        {
            return nResult;
        }
        return a_util::result::SUCCESS;
    }

    static inline bool CheckAttrib(const a_util::xml::DOMAttributes oAttrs, const std::string& strAttrib, unsigned int& nRes)
    {
        a_util::xml::DOMAttributes::const_iterator itAttr = oAttrs.find(strAttrib);
        if (itAttr != oAttrs.end())
        {
            nRes = a_util::strings::toUInt32(itAttr->second);
            return true;
        }

        return false;
    }

    static inline bool CheckAttrib(const a_util::xml::DOMAttributes oAttrs, const std::string& strAttrib, std::string& strRes)
    {
        a_util::xml::DOMAttributes::const_iterator itAttr = oAttrs.find(strAttrib);
        if (itAttr != oAttrs.end())
        {
            strRes = itAttr->second.c_str();
            return true;
        }

        return false;
    }

    a_util::result::Result DDLImporter::buildSingleStruct(DDLComplex ** ppoNewStruct, const a_util::xml::DOMElement& oStructElement)
    {
        if (!ppoNewStruct) { return ERR_POINTER; }
        *ppoNewStruct = NULL;

        // get already defined types
        DDLDTVec vecDTs = _ddl_desc->getDatatypes();
        DDLComplexVec vecStructs = _ddl_desc->getStructs();
        DDLEnumVec vecEnums = _ddl_desc->getEnums();

        // get already defined types
        DDLDTVec vecDTRefs;
        DDLComplexVec vecStructRefs;
        DDLEnumVec vecEnumRefs;
        if (_current_ref_ddl)
        {
            vecDTRefs     = _current_ref_ddl->getDatatypes();
            vecStructRefs = _current_ref_ddl->getStructs();
            vecEnumRefs = _current_ref_ddl->getEnums();
        }

        a_util::result::Result nResult = a_util::result::SUCCESS;
        std::string strStructName = oStructElement.getAttribute("name");
        if (!oStructElement.hasAttribute("name"))
        {
            strStructName = oStructElement.getAttribute("type");
            if (!oStructElement.hasAttribute("type"))
            {
                pushMessage(a_util::strings::format("Attribute 'name' is missing in 'struct' tag."),
                    importer_error);
                nResult = ERR_UNKNOWN_FORMAT;
                if (false == _full_check)
                {
                    return nResult;
                }
            }
        }

        // mandatory property, but no error (compatibility to former DDL versions)
        std::string strVersion = a_util::strings::empty_string;
        if (!oStructElement.hasAttribute("version"))
        {
            // mandatory property, but no error (compatibility to former DDL versions)
        }
        else
        {
            strVersion = oStructElement.getAttribute("version");
        }
        std::string strComment = a_util::strings::empty_string;
        if (oStructElement.hasAttribute("comment"))
        {
            strComment = oStructElement.getAttribute("comment");
        }
        DDLAlignment::AlignmentType eAlignment = DDLAlignment::e1;
        if (oStructElement.hasAttribute("alignment"))
        {
            std::string strAttr = oStructElement.getAttribute("alignment");
            eAlignment = DDLAlignment::fromString(strAttr);
            if (eAlignment == DDLAlignment::e_invalid)
            {
                pushMessage(a_util::strings::format("The struct '%s' has an invalid alignment of '%s'", strStructName.c_str(), strAttr.c_str()),
                            importer_error);
                nResult = ERR_INVALID_ARG;
                if (false == _full_check)
                {
                    return nResult;
                }
            }
        }
        a_util::xml::DOMElementList oElements;
        oStructElement.findNodes("element", oElements);
        DDLElementVec vecDDLElements;
        for (tDOMElemIt itElem = oElements.begin();
             oElements.end() != itElem;
             ++itElem)
        {
            std::string strElementName;
            if (itElem->hasAttribute("name"))
            {
                strElementName = itElem->getAttribute("name");
            }

            if (_basic_check &&
                std::find_if(vecDDLElements.begin(), vecDDLElements.end(),
                             DDLCompareFunctor<DDLElement>(strElementName)) != vecDDLElements.end())
            {
                pushMessage(a_util::strings::format("The struct element name '%s.%s' is already used", strStructName.c_str(), strElementName.c_str()),
                            importer_error);
                nResult = ERR_RESOURCE_IN_USE;
                if (false == _full_check)
                {
                    return nResult;
                }
            }

            // check optional attributes
            unsigned int uiBitpos = 0;
            CheckAttrib(itElem->getAttributes(), "bitpos", uiBitpos);

            unsigned int uiNumBits = 0;
            CheckAttrib(itElem->getAttributes(), "numbits", uiNumBits);

            a_util::result::Result nRes = a_util::result::SUCCESS;
            bool bNoBytePosSet = !itElem->hasAttribute("bytepos");
            std::string strBytePos = itElem->getAttribute("bytepos");
            bool bNoByteOrderSet = !itElem->hasAttribute("byteorder");
            std::string strByteOrder = itElem->getAttribute("byteorder");
            bool bNoAlignmentSet = !itElem->hasAttribute("alignment");
            std::string strAlignment = itElem->getAttribute("alignment");

            a_util::xml::DOMElement oDeserialized;
            if (itElem->findNode("deserialized", oDeserialized))
            {
                bNoAlignmentSet = !oDeserialized.hasAttribute("alignment");
                strAlignment = oDeserialized.getAttribute("alignment");
            }
            a_util::xml::DOMElement oSerialized;
            if (itElem->findNode("serialized", oSerialized))
            {
                bNoBytePosSet = !oSerialized.hasAttribute("bytepos");
                strBytePos = oSerialized.getAttribute("bytepos");
                bNoByteOrderSet = !oSerialized.hasAttribute("byteorder");
                strByteOrder = oSerialized.getAttribute("byteorder");
                CheckAttrib(oSerialized.getAttributes(), "bitpos", uiBitpos);
                CheckAttrib(oSerialized.getAttributes(), "numbits", uiNumBits);
            }

            if (!itElem->hasAttribute("type") ||
                !itElem->hasAttribute("name") ||
                bNoBytePosSet ||
                !itElem->hasAttribute("arraysize") ||
                bNoByteOrderSet ||
                bNoAlignmentSet)
            {
                std::transform(vecDDLElements.begin(), vecDDLElements.end(), vecDDLElements.begin(), DDLDescription::deleteChild<DDLElement>);
                if (!itElem->hasAttribute("name"))
                {
                    pushMessage(a_util::strings::format("Attribute 'name' is missing in 'struct' tag."),
                        importer_error);
                }
                if (!itElem->hasAttribute("type"))
                {
                    if (!strElementName.empty())
                    {
                        pushMessage(a_util::strings::format("Attribute 'type' is missing in 'struct' tag '%s'.",
                            strElementName.c_str()), importer_error);
                    }
                    else
                    {
                        pushMessage(a_util::strings::format("Attribute 'type' is missing in 'struct' tag."),
                            importer_error);
                    }
                }
                if (bNoBytePosSet)
                {
                    if (!strElementName.empty())
                    {
                        pushMessage(a_util::strings::format("Attribute 'bytepos' is missing in 'struct' tag '%s'.",
                            strElementName.c_str()), importer_error);
                    }
                    else
                    {
                        pushMessage(a_util::strings::format("Attribute 'bytepos' is missing in 'struct' tag."),
                            importer_error);
                    }
                }
                if (!itElem->hasAttribute("arraysize"))
                {
                    if (!strElementName.empty())
                    {
                        pushMessage(a_util::strings::format("Attribute 'arraysize' is missing in 'struct' tag '%s'.",
                            strElementName.c_str()), importer_error);
                    }
                    else
                    {
                        pushMessage(a_util::strings::format("Attribute 'arraysize' is missing in 'struct' tag."),
                            importer_error);
                    }
                }
                if (bNoByteOrderSet)
                {
                    if (!strElementName.empty())
                    {
                        pushMessage(a_util::strings::format("Attribute 'byteorder' is missing in 'struct' tag '%s'.",
                            strElementName.c_str()), importer_error);
                    }
                    else
                    {
                        pushMessage(a_util::strings::format("Attribute 'byteorder' is missing in 'struct' tag."),
                            importer_error);
                    }
                }
                if (bNoAlignmentSet)
                {
                    if (!strElementName.empty())
                    {
                        pushMessage(a_util::strings::format("Attribute 'alignment' is missing in 'struct' tag '%s'.",
                            strElementName.c_str()), importer_error);
                    }
                    else
                    {
                        pushMessage(a_util::strings::format("Attribute 'alignment' is missing in 'struct' tag."),
                            importer_error);
                    }

                }
                nRes = ERR_UNKNOWN_FORMAT;
                if (false == _full_check)
                {
                    return nRes;
                }
            }

            if (itElem->getAttribute("type") == strStructName)
            {
                std::transform(vecDDLElements.begin(), vecDDLElements.end(), vecDDLElements.begin(), DDLDescription::deleteChild<DDLElement>);
                pushMessage(a_util::strings::format("Unresolvable recursion in struct '%s'.",
                    strStructName.c_str()), importer_error);
                nRes = ERR_UNKNOWN_FORMAT;
                if (false == _full_check)
                {
                    return nRes;
                }
            }
            // resolve type
            // if the Importer is called with createPartial then the DDL will search for Referenced first
            // after that it will reference to a local DDL Type or put it to the unresolved types
            DDLDataType * poDataType = NULL;
            DDLComplex * poComplex = NULL;
            DDLEnum* poEnum = NULL;

            std::string strTypeAttr = itElem->getAttribute("type");
            poDataType = tryToFind(strTypeAttr, vecDTRefs, vecDTs);

            if (poDataType == NULL)
            {
                poComplex = tryToFind(strTypeAttr, vecStructRefs, vecStructs);
                if (poComplex == NULL)
                {
                    poComplex = tryToFind(strTypeAttr, _unknown_structs, vecStructs);
                    if (poComplex == NULL)
                    {
                        poEnum = tryToFind(strTypeAttr, vecEnumRefs, vecEnums);
                        if(poEnum == NULL)
                        {
                            // => create object, mark it, and continue
                            poComplex = new DDLComplex(strTypeAttr, 1);
                            _unknown_structs.insert(poComplex);
                        }
                    }
                }
            }

            if (_basic_check && itElem->hasAttribute("arraysize"))
            {
                if (uiNumBits > 0 && a_util::strings::toInt32(itElem->getAttribute("arraysize")) > 1)
                {
                    pushMessage(a_util::strings::format("Attribute 'numbits' is not allowed in array elements."),
                            importer_error);
                    return ERR_INVALID_ARG;
                }
            }

            std::string strDescription;
            CheckAttrib(itElem->getAttributes(), "description", strDescription);

            std::string strComment;
            CheckAttrib(itElem->getAttributes(), "comment", strComment);

            // value is used in combination with enums to implement constants
            std::string strConstantValue;
            CheckAttrib(itElem->getAttributes(), "value", strConstantValue);

            std::string strMinValue;
            bool bMinValid = CheckAttrib(itElem->getAttributes(), "min", strMinValue);

            std::string strMaxValue;
            bool bMaxValid = CheckAttrib(itElem->getAttributes(), "max", strMaxValue);

            std::string strDefaultValue;
            bool bDefaultValid = CheckAttrib(itElem->getAttributes(), "default", strDefaultValue);

            std::string strScaleValue;
            bool bScaleValid = CheckAttrib(itElem->getAttributes(), "scale", strScaleValue);

            std::string strOffsetValue;
            bool bOffsetValid = CheckAttrib(itElem->getAttributes(), "offset", strOffsetValue);

            // resolve unit
            // if the Importer is called with createPartial then the DDL will search for Referenced first
            // after that it will reference to a local DDL Type or put it to the unresolved types
            DDLUnit* poUnit = NULL;
            DDLBaseunit* poBaseUnit = NULL;
            IDDLUnit* pUnitRef = NULL;

            if (itElem->hasAttribute("unit"))
            {
                DDLUnitVec vecDDLUnits = _ddl_desc->getUnits();
                DDLUnitVec vecDDLUnitRefs;
                if (_current_ref_ddl != NULL)
                {
                    vecDDLUnitRefs = _current_ref_ddl->getUnits();
                }
                DDLBaseunitVec vecDDLBaseUnits = _ddl_desc->getBaseunits();
                DDLBaseunitVec vecDDLBaseUnitRefs;
                if (_current_ref_ddl != NULL)
                {
                    vecDDLBaseUnitRefs = _current_ref_ddl->getBaseunits();
                }

                std::string strUnitAttr = itElem->getAttribute("unit");
                poUnit = tryToFind(strUnitAttr, vecDDLUnitRefs, vecDDLUnits);
                if (poUnit == NULL)
                {
                    poBaseUnit = tryToFind(strUnitAttr, vecDDLBaseUnitRefs, vecDDLBaseUnits);
                    if (poBaseUnit != NULL)
                    {
                        pUnitRef = (IDDLUnit*) poBaseUnit;
                    }
                }
                else
                {
                    pUnitRef = (IDDLUnit*) poUnit;
                }
                if (pUnitRef == NULL)
                {
                    std::transform(vecDDLElements.begin(), vecDDLElements.end(), vecDDLElements.begin(), DDLDescription::deleteChild<DDLElement>);
                    pushMessage(a_util::strings::format("Missing class '%s' for unit in element '%s' in struct '%s'.",
                        strUnitAttr.c_str(), strElementName.c_str(), strStructName.c_str()), importer_error);
                    nRes = ERR_NO_CLASS;
                    if (false == _full_check)
                    {
                        return nRes;
                    }
                }
            }

            // do not continue with struct, if any mandatory property could not found
            if (isFailed(nRes))
            {
                nResult = nRes;
                if (false == _full_check)
                {
                    return nResult;
                }
                continue;
            }

            if (DDLAlignment::fromString(strAlignment) == DDLAlignment::e_invalid)
            {
                pushMessage(a_util::strings::format("The struct element '%s.%s' has an invalid alignment of '%s'", strStructName.c_str(), strElementName.c_str(), strAlignment.c_str()),
                            importer_error);
                nResult = ERR_INVALID_ARG;
                if (false == _full_check)
                {
                    return nResult;
                }
            }

            const std::string& strArraySize = itElem->getAttribute("arraysize");
            if (NULL == poDataType)
            {
                if (poEnum != NULL)
                {
                    bool bIsDynamic = !a_util::strings::isUInt32(strArraySize);

                    vecDDLElements.push_back(new DDLElement(poEnum,
                        strElementName,
                        static_cast<unsigned int>(a_util::strings::toUInt32(strBytePos)),
                        bIsDynamic ? 0 : static_cast<unsigned int>(a_util::strings::toUInt32(strArraySize)),
                        DDLByteorder::fromString(strByteOrder),
                        DDLAlignment::fromString(strAlignment),
                        pUnitRef,
                        uiBitpos,
                        uiNumBits,
                        strDescription,
                        strComment,
                        bIsDynamic ? strArraySize: a_util::strings::empty_string,
                        strConstantValue));
                }
                else
                {
                    bool bIsDynamic = !a_util::strings::isUInt32(strArraySize);

                    vecDDLElements.push_back(new DDLElement(poComplex,
                        strElementName,
                        static_cast<unsigned int>(a_util::strings::toUInt32(strBytePos)),
                        bIsDynamic ? 0 : static_cast<unsigned int>(a_util::strings::toUInt32(strArraySize)),
                        DDLByteorder::fromString(strByteOrder),
                        DDLAlignment::fromString(strAlignment),
                        pUnitRef,
                        uiBitpos,
                        uiNumBits,
                        strDescription,
                        strComment,
                        bIsDynamic ? strArraySize : a_util::strings::empty_string));
                }
            }
            else
            {
                bool bIsDynamic = !a_util::strings::isUInt32(strArraySize);

                vecDDLElements.push_back(new DDLElement(poDataType,
                                                         strElementName,
                                                         static_cast<unsigned int>(a_util::strings::toUInt32(strBytePos)),
                                                         bIsDynamic ? 0 : static_cast<unsigned int>(a_util::strings::toUInt32(strArraySize)),
                                                         DDLByteorder::fromString(strByteOrder),
                                                         DDLAlignment::fromString(strAlignment),
                                                         pUnitRef,
                                                         uiBitpos,
                                                         uiNumBits,
                                                         strDescription,
                                                         strComment,
                                                         bIsDynamic ? strArraySize : a_util::strings::empty_string,
                                                         a_util::strings::empty_string,
                                                         bMinValid,
                                                         strMinValue,
                                                         bMaxValid,
                                                         strMaxValue,
                                                         bDefaultValid,
                                                         strDefaultValue,
                                                         bScaleValid,
                                                         strScaleValue,
                                                         bOffsetValid,
                                                         strOffsetValue));
            }
        }
        // do not create new DDLComplex, if any mandatory property could not found
        if (isFailed(nResult))
        {
            return nResult;
        }
        DDLComplexIt itUnknown = std::find_if(_unknown_structs.begin(), _unknown_structs.end(), DDLCompareFunctor<>(oStructElement.getAttribute("name")));
        if (_unknown_structs.end() == itUnknown)
        {
            // create new object
            *ppoNewStruct = new DDLComplex(strStructName,
                                            static_cast<unsigned int>(a_util::strings::toUInt32(strVersion)),
                                            strComment,
                                            eAlignment,
                                            DDLElementVec(),
                                            _creation_level);
        }
        else
        {
            // use placeholder object and fill it with concrete data
            (*itUnknown)->create(strStructName,
                                 static_cast<unsigned int>(a_util::strings::toUInt32(strVersion)),
                                 strComment,
                                 eAlignment,
                                 DDLElementVec(),
                                 _creation_level);
            *ppoNewStruct = *itUnknown;
            // unmark object
            _unknown_structs.erase(itUnknown);
        }

        if (oStructElement.hasAttribute("ddlversion"))
        {
            auto version = DDLVersion::fromString(oStructElement.getAttribute("ddlversion"));
            (*ppoNewStruct)->setDDLVersion(version);
        }
        else
        {
            // Add version information for behavior differentiation
            DDLVersion language_version = DDLVersion::getDefaultVersion();
            DDLHeader* pHeader = _ddl_desc->getHeader();
            if (NULL != pHeader)
            {
                language_version = pHeader->getLanguageVersion();
            }
            (*ppoNewStruct)->setDDLVersion(language_version);
        }

        (*ppoNewStruct)->setElements(vecDDLElements);
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLImporter::buildStructs()
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        a_util::xml::DOMElement oStructs;
        a_util::result::Result nResult = a_util::result::SUCCESS;
        if (!_dom.findNode("//structs", oStructs))
        {
            pushMessage(std::string("DDL does not contain the 'structs' element"),
                importer_error);
            return ERR_NOT_FOUND;
        }
        else
        {
            // build up struct objects
            a_util::xml::DOMElementList oStructList;
            if (!oStructs.findNodes("struct", oStructList))
            {
                // => no error
            }
            else
            {
                DDLComplexVec vecStructs(_sorted);
                _ddl_desc->cloneStructs(vecStructs);
                for (tDOMElemIt it = oStructList.begin(); oStructList.end() != it; ++it)
                {
                    DDLComplex * poStructTmp = NULL;
                    a_util::result::Result nRes = buildSingleStruct(&poStructTmp, *it);
                    if (isFailed(nRes))
                    {
                        nResult = nRes;
                        if (false == _full_check)
                        {
                            return nResult;
                        }
                        continue;
                    }
                    if (_basic_check && vecStructs.find(poStructTmp->getName()))
                    {
                        pushMessage(a_util::strings::format("The struct '%s' is specified more than once.",
                                                    poStructTmp->getName().c_str()),
                                    importer_error);
                        nResult = ERR_RESOURCE_IN_USE;
                        if (false == _full_check)
                        {
                            return nResult;
                        }
                    }
                    _ddl_desc->addStruct(poStructTmp);
                }

                for (DDLComplexIt itUnknown = _unknown_structs.begin();
                    _unknown_structs.end() != itUnknown; ++itUnknown)
                {
                    pushMessage(a_util::strings::format("The struct '%s' was referenced but not defined.",
                        (*itUnknown)->getName().c_str()), importer_error);
                }

                if (!_unknown_structs.empty())
                {

                    vecStructs.deleteAll();
                    // unmark object
                    pushMessage(std::string("There are undefined types/structs in this description."),
                        importer_error);
                    nResult = ERR_UNKNOWN;
                    if (false == _full_check)
                    {
                        return nResult;
                    }
                }
            }
            if (isFailed(nResult))
            {
                return nResult;
            }
        }

        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLImporter::buildSingleStream(DDLStream ** ppoNewStream, a_util::xml::DOMElement oStreamElement)
    {
        if (!ppoNewStream) { return ERR_POINTER; }
        *ppoNewStream = NULL;
        std::string name = a_util::strings::empty_string;
        a_util::result::Result nResult = a_util::result::SUCCESS;
        if (!oStreamElement.hasAttribute("name"))
        {
            if (!oStreamElement.hasAttribute("type"))
            {
                pushMessage(a_util::strings::format("Attribute 'name' is missing in 'stream' tag."),
                    importer_error);
                nResult = ERR_UNKNOWN_FORMAT;
                if (false == _full_check)
                {
                    return nResult;
                }
            }
            else
            {
                // compatibility with old versions
                oStreamElement.setAttribute("name", oStreamElement.getAttribute("type"));
                name = oStreamElement.getAttribute("type");
            }
        }
        else
        {
            name = oStreamElement.getAttribute("name");
        }

        // resolve type
        DDLComplexVec vecStructs = _ddl_desc->getStructs();
        DDLComplexVec vecStructRefs;
        if (_current_ref_ddl)
        {
            vecStructRefs = _current_ref_ddl->getStructs();
        }

        DDLComplex* pTypeFound = tryToFind(oStreamElement.getAttribute("type"), vecStructRefs , vecStructs);

        if (pTypeFound == NULL)
        {
            if (!name.empty())
            {
                pushMessage(a_util::strings::format("The type '%s' of the stream '%s' is not defined.",
                    oStreamElement.getAttribute("type").c_str(), name.c_str()), importer_error);
            }
            else
            {
                pushMessage(a_util::strings::format("The type '%s' of an unnamed stream is not defined.",
                    oStreamElement.getAttribute("type").c_str()), importer_error);
            }
            nResult = ERR_NO_CLASS;
            if (false == _full_check)
            {
                return nResult;
            }
        }

        // check optional attribute
        std::string strDescription = a_util::strings::empty_string;
        if (oStreamElement.hasAttribute("description"))
        {
            strDescription = oStreamElement.getAttribute("description");
        }
        a_util::xml::DOMElementList oStructs;
        oStreamElement.findNodes("struct", oStructs);
        DDLStreamStructVec vecDDLStreamStructs;
        for (tDOMElemIt itStruct = oStructs.begin();
            oStructs.end() != itStruct; ++itStruct)
        {
            std::string strElementName = itStruct->getAttribute("name");
            a_util::result::Result nRes = a_util::result::SUCCESS;
            if (!itStruct->hasAttribute("type") ||
                !itStruct->hasAttribute("bytepos"))
            {
                std::transform(vecDDLStreamStructs.begin(), vecDDLStreamStructs.end(), vecDDLStreamStructs.begin(), DDLDescription::deleteChild<DDLStreamStruct>);
                if (!strElementName.empty() && !name.empty())
                {
                    pushMessage(a_util::strings::format("The struct '%s' in stream '%s' is missing required attributes (type/bytepos).",
                        strElementName.c_str(), name.c_str()), importer_error);
                }
                else if (!name.empty())
                {
                    pushMessage(a_util::strings::format("An unnamed struct in stream '%s' is missing required attributes (type/bytepos).",
                        name.c_str()), importer_error);
                }
                else
                {
                    pushMessage(a_util::strings::format("An unnamed struct in an unnamed stream is missing required attributes (type/bytepos)."),
                        importer_error);
                }
                nRes = ERR_UNKNOWN_FORMAT;
                if (false == _full_check)
                {
                    return nRes;
                }
            }

            // resolve type
            DDLComplex* pStreamStructType = tryToFind(itStruct->getAttribute("type"), vecStructRefs , vecStructs);
            if (pStreamStructType == NULL)
            {
                std::transform(vecDDLStreamStructs.begin(), vecDDLStreamStructs.end(), vecDDLStreamStructs.begin(), DDLDescription::deleteChild<DDLStreamStruct>);
                pushMessage(a_util::strings::format("The type '%s' of a struct of stream '%s' is not defined.",
                    itStruct->getAttribute("type").c_str(), name.c_str()), importer_error);
                nRes = ERR_NO_CLASS;
                if (false == _full_check)
                {
                    return nRes;
                }
            }

            // check optional attribute
            std::string strStructName = pStreamStructType->getName();
            if (itStruct->hasAttribute("name"))
            {
                strStructName = itStruct->getAttribute("name");
            }
            // do not continue with stream struct, if any mandatory property could not found
            if (isFailed(nRes))
            {
                nResult = nRes;
                if (false == _full_check)
                {
                    return nResult;
                }
                continue;
            }
            vecDDLStreamStructs.push_back(new DDLStreamStruct(pStreamStructType,
                                          static_cast<unsigned int>(a_util::strings::toUInt32(itStruct->getAttribute("bytepos"))),
                                          strStructName));
        }
        // do not create new DDLStream, if any mandatory property could not found
        if (isFailed(nResult))
        {
            return nResult;
        }
        *ppoNewStream = new DDLStream(pTypeFound,
                                       name,
                                       strDescription,
                                       DDLStreamStructVec(),
                                       _creation_level);
        // Add version information for behavior differentiation
        DDLVersion language_version = DDLVersion::ddl_version_invalid;
        DDLHeader* pHeader = _ddl_desc->getHeader();
        if (NULL != pHeader)
        {
            language_version = pHeader->getLanguageVersion();
        }
        (*ppoNewStream)->setDDLVersion(language_version);
        (*ppoNewStream)->refStructs(vecDDLStreamStructs);
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLImporter::buildStreams()
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        a_util::xml::DOMElement oStreams;
        a_util::result::Result nResult = a_util::result::SUCCESS;
        if (!_dom.findNode("//streams", oStreams))
        {
            pushMessage(std::string("DDL does not contain the 'streams' element"),
                importer_error);
            return nResult;
        }
        else
        {
            a_util::xml::DOMElementList oStreamList;
            DDLStreamVec vecStreams(_sorted);
            if (!oStreams.findNodes("stream", oStreamList))
            {
                // => no error
            }
            else
            {
                for (tDOMElemIt it = oStreamList.begin(); oStreamList.end() != it; ++it)
                {
                    DDLStream * poStreamTmp = NULL;
                    a_util::result::Result nRes = buildSingleStream(&poStreamTmp, *it);
                    if (isFailed(nRes))
                    {
                        vecStreams.deleteAll();
                        nResult = nRes;
                        if (isFailed(nResult))
                        {
                            return nResult;
                        }
                        continue;
                    }
                    if (_basic_check && vecStreams.find(poStreamTmp->getName()))
                    {
                        pushMessage(a_util::strings::format("The stream '%s' is specified more than once.",
                                                    poStreamTmp->getName().c_str()),
                                    importer_error);
                        nResult = ERR_RESOURCE_IN_USE;
                        if (false == _full_check)
                        {
                            return nResult;
                        }
                    }
                    vecStreams.insert(poStreamTmp);
                }
                // make all elements of stream vector unique
                _ddl_desc->refStreams(vecStreams);
            }
            if (isFailed(nResult))
            {
                return nResult;
            }
        }
        return a_util::result::SUCCESS;
    }

    void DDLImporter::pushMessage(const std::string& strMsg, ImporterMsgSeverity severity)
    {
        ImporterMsg sError = {strMsg, severity};
        _errors.push_back(sError);
    }

    template<typename T>
    T* DDLImporter::tryToFind(const std::string& name,
                               DDLContainer<T>& first_data,
                               DDLContainer<T>& second_data)
    {
        T* pRes = NULL;
        if (_prefere_reference)
        {
            pRes = first_data.find(name);
            if (!pRes)
            {
                pRes = second_data.find(name);
            }
        }
        else
        {
            pRes = second_data.find(name);
            if (!pRes)
            {
                pRes = first_data.find(name);
            }
        }

        return pRes;
    }

    a_util::result::Result DDLImporter::buildEnums()
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        a_util::xml::DOMElement oEnums;
        if (!_dom.findNode("//enums", oEnums))
        {
            //kompatibilit�t zu alten files!!
            if (_ddl_desc->getHeader()->getLanguageVersion() <  DDLVersion::ddl_version_20)
            {
                return a_util::result::SUCCESS;
            }
            else
            {
                pushMessage(std::string("DDL does not contain the 'enums' element."),
                importer_warning);
            }
            return a_util::result::SUCCESS;
        }
        else
        {
            a_util::result::Result nResult = a_util::result::SUCCESS;

            // build up enum objects
            a_util::xml::DOMElementList oEnumList;
            if (!oEnums.findNodes("enum", oEnumList))
            {
                // => no error
            }
            else
            {
                DDLEnumVec vecDDLEnums(_sorted);
                for (tDOMElemIt it = oEnumList.begin(); oEnumList.end() != it; ++it)
                {
                    DDLEnum * poEnumTmp = NULL;
                    a_util::result::Result nRes = buildSingleEnum(&poEnumTmp, *it);
                    if (isFailed(nRes))
                    {
                        vecDDLEnums.deleteAll();
                        nResult = nRes;
                        if (false == _full_check)
                        {
                            return nResult;
                        }
                        continue;
                    }
                    vecDDLEnums.insert(poEnumTmp);
                }
                _ddl_desc->refEnums(vecDDLEnums);
            }
            if (isFailed(nResult))
            {
                return nResult;
            }
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLImporter::buildSingleEnum( DDLEnum** ppoNewEnum, const a_util::xml::DOMElement& oEnumElement )
    {
        if (!ppoNewEnum) { return ERR_POINTER; }
        *ppoNewEnum = NULL;
        std::string name = oEnumElement.getAttribute("name");
        a_util::result::Result nResult = a_util::result::SUCCESS;
        if (!oEnumElement.hasAttribute("name"))
        {
            pushMessage(a_util::strings::format("Attribute 'name' is missing in 'enum' tag."),
                importer_error);
            nResult = ERR_UNKNOWN_FORMAT;
            if (false == _full_check)
            {
                return nResult;
            }
        }

        std::string strType;
        if (!oEnumElement.hasAttribute("type"))
        {
            if (!name.empty())
            {
                pushMessage(a_util::strings::format("Attribute 'type' is missing in 'constant' tag '%s'.",
                    name.c_str()), importer_error);
            }
            else
            {
                pushMessage(a_util::strings::format("Attribute 'type' is missing in 'constant' tag."),
                    importer_error);
            }
            nResult = ERR_UNKNOWN_FORMAT;
            if (false == _full_check)
            {
                return nResult;
            }
        }
        else
        {
            strType = oEnumElement.getAttribute("type");
        }
        // do not continue with enum, if any enum property could not found (all properties are mandatory!)
        if (isFailed(nResult))
        {
            return nResult;
        }

        EnumNameValueVec vecNameValues;
        a_util::xml::DOMElementList oEnumElements;
        oEnumElement.findNodes("element", oEnumElements);
        a_util::result::Result nRes = a_util::result::SUCCESS;
        for (a_util::xml::DOMElementList::iterator itEnumElement = oEnumElements.begin();
             itEnumElement != oEnumElements.end();
             ++itEnumElement)
        {
            nRes = a_util::result::SUCCESS;

            std::string strElementName;
            if(!itEnumElement->hasAttribute("name"))
            {
                pushMessage(a_util::strings::format("Attribute 'name' is missing in 'enum/element' tag."),
                    importer_error);
                nRes = ERR_UNKNOWN_FORMAT;
                if (false == _full_check)
                {
                    return nRes;
                }
            }
            else
            {
                strElementName = itEnumElement->getAttribute("name");
            }

            std::string strElementValue;
            if(!itEnumElement->hasAttribute("value"))
            {
                if (!name.empty() && !strElementName.empty())
                {
                    pushMessage(a_util::strings::format("No 'value' defined in 'enum/element' tag '%s/%s'.",
                        name.c_str(), strElementName.c_str()), importer_error);
                }
                else if (!strElementName.empty())
                {
                    pushMessage(a_util::strings::format("No 'value' defined in 'enum/element' tag '%s'.",
                        strElementName.c_str()), importer_error);
                }
                else
                {
                    pushMessage(a_util::strings::format("No 'value' defined in 'enum/element' tag."),
                        importer_error);
                }
                nRes = ERR_UNKNOWN_FORMAT;
                if (false == _full_check)
                {
                    return nRes;
                }
            }
            else
            {
                strElementValue = itEnumElement->getAttribute("value");
            }
            if (isFailed(nRes))
            {
                nResult = nRes;
                if (false == _full_check)
                {
                    return nResult;
                }
                continue;
            }
            vecNameValues.push_back(std::make_pair<std::string, std::string>(std::string(strElementName), std::string(strElementValue)));
        }
        // do not continue with enum, if any enum element property could not found (all properties are mandatory!)
        if (isFailed(nResult))
        {
            return nResult;
        }

        // get already defined types
        DDLDTVec vecDTs = _ddl_desc->getDatatypes();

        // get already defined types
        DDLDTVec vecDTRefs;
        if (_current_ref_ddl)
        {
            vecDTRefs     = _current_ref_ddl->getDatatypes();
        }

        DDLDataType * poDataType = NULL;

        poDataType = tryToFind(strType, vecDTRefs, vecDTs);
        if (poDataType == NULL)
        {
            pushMessage(a_util::strings::format("Data type '%s' could not be resolved.",
                strType.c_str()), importer_error);
            nResult = ERR_UNKNOWN_FORMAT;
            if (false == _full_check)
            {
                return nResult;
            }
        }
        // do not create new DDLEnum, if any enum property could not found (all properties are mandatory!)
        if (isFailed(nResult))
        {
            return nResult;
        }

        *ppoNewEnum = new DDLEnum(poDataType, name, vecNameValues, NULL, _creation_level);
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLImporter::buildStreamMetaTypes()
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        a_util::xml::DOMElement oTypes;
        if (!_dom.findNode("//streammetatypes", oTypes))
        {
            return a_util::result::SUCCESS;
        }

        a_util::result::Result nResult = a_util::result::SUCCESS;

        // build up stream meta type objects
        a_util::xml::DOMElementList oTypeList;
        if (oTypes.findNodes("streammetatype", oTypeList))
        {
            DDLStreamMetaTypeVec vecDDLStreamMetaTypes(_sorted);
            for (a_util::xml::DOMElementList::iterator it = oTypeList.begin(); it != oTypeList.end(); ++it)
            {
                a_util::result::Result nRes = buildSingleStreamMetaType(*it, vecDDLStreamMetaTypes);
                if (isFailed(nRes))
                {
                    nResult = nRes;
                    if (false == _full_check)
                    {
                        vecDDLStreamMetaTypes.deleteAll();
                        return nRes;
                    }
                    continue;
                }
            }

            for (DDLStreamMetaTypeVec::iterator it = _unknown_stream_meta_types.begin(); it != _unknown_stream_meta_types.end(); ++it)
            {
                pushMessage(a_util::strings::format("The struct '%s' was referenced but not defined.",
                    (*it)->getName().c_str()), importer_error);
            }

            if (!_unknown_stream_meta_types.empty())
            {

                vecDDLStreamMetaTypes.deleteAll();
                // unmark object
                pushMessage(std::string("There are undefined streammetatypes in this description."),
                    importer_error);
                nResult = ERR_UNKNOWN;
                if (false == _full_check)
                {
                    return nResult;
                }
            }


            _ddl_desc->refStreamMetaTypes(vecDDLStreamMetaTypes);
        }

        return nResult;
    }

    a_util::result::Result DDLImporter::buildSingleStreamMetaType(const a_util::xml::DOMElement& oStreamMetaTypeElement,
                                                    DDLStreamMetaTypeVec& vecStreamMetaTypes)
    {
        const a_util::xml::DOMAttributes oAttributes = oStreamMetaTypeElement.getAttributes();
        a_util::xml::DOMAttributes::const_iterator itName = oAttributes.find("name");
        a_util::xml::DOMAttributes::const_iterator itVersion = oAttributes.find("version");
        a_util::xml::DOMAttributes::const_iterator itParent = oAttributes.find("parent");

        if (itName == oAttributes.end())
        {
            pushMessage(a_util::strings::format("Attribute 'name' is missing in 'streammetatype' tag."),
                        importer_error);
            return ERR_UNKNOWN_FORMAT;
        }
        if (itVersion == oAttributes.end())
        {
            pushMessage(a_util::strings::format("Attribute 'version' is missing in 'streammetatype' tag."),
                        importer_error);
            return ERR_UNKNOWN_FORMAT;
        }

        a_util::result::Result nResult = a_util::result::SUCCESS;

        DDLStreamMetaType* pParent = NULL;
        if (itParent != oAttributes.end() && !itParent->second.empty())
        {
            if (_current_ref_ddl)
            {
                pParent = const_cast<DDLStreamMetaType*>(_current_ref_ddl->getStreamMetaTypeByName(itParent->second));
            }
            if (!pParent)
            {
                pParent = tryToFind(itParent->second, _unknown_stream_meta_types, vecStreamMetaTypes);
                if (!pParent)
                {
                    pParent = new DDLStreamMetaType(itParent->second, "1");
                    _unknown_stream_meta_types.insert(pParent);
                }
            }
        }

        DDLPropertyVec oProps;
        a_util::xml::DOMElementList oPropertyNodes;
        if (oStreamMetaTypeElement.findNodes("property", oPropertyNodes))
        {
            for (a_util::xml::DOMElementList::const_iterator it = oPropertyNodes.begin();
                it != oPropertyNodes.end(); ++it)
            {
                a_util::xml::DOMAttributes attrs = it->getAttributes();
                a_util::xml::DOMAttributes::const_iterator itName = attrs.find("name");
                a_util::xml::DOMAttributes::const_iterator itType = attrs.find("type");
                if (itName != attrs.end() && itType != attrs.end())
                {
                    oProps.push_back(new DDLProperty(itName->second,
                                                      itType->second));
                }
            }
        }

        a_util::memory::unique_ptr<DDLStreamMetaType> pStreamMetaType(new DDLStreamMetaType(itName->second, itVersion->second,
                                                                                   oProps, pParent));

        DDLStreamMetaTypeVec::iterator itUnknown = std::find_if(_unknown_stream_meta_types.begin(),
                                      _unknown_stream_meta_types.end(),
                                      DDLCompareFunctor<>(itName->second));
        if (itUnknown != _unknown_stream_meta_types.end())
        {
            *(*itUnknown) = *pStreamMetaType.get();
            vecStreamMetaTypes.insert(*itUnknown);
            _unknown_stream_meta_types.erase(itUnknown);
        }
        else
        {
            vecStreamMetaTypes.insert(pStreamMetaType.release());
        }

        return nResult;
    }


}   // namespace ddl
