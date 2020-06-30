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

#include <utility>  // std::swap
#include <functional>
#include <limits>
#include "a_util/result/error_def.h"
#include "legacy_error_macros.h"
#include "ddldescription.h"
#include "ddlversion.h"
#include "ddlalignment.h"
#include "ddldatatype.h"
#include "ddlbaseunit.h"
#include "ddlprefix.h"
#include "ddlenum.h"
#include "ddlheader.h"
#include "ddlrefunit.h"
#include "ddlstreammetatype.h"
#include "ddlunit.h"
#include "ddlcomplex.h"
#include "ddlrepair.h"
#include "ddlstream.h"
#include "ddlinspector.h"


#ifndef WIN32
// only necessary on Linux platforms, on Windows defined inside MSVC
#include <climits>
#include <cfloat>
#endif

/// XML header for the data definition language description file
#define DESCRPTIONFILE_HEADER_DDL1_DEFAULT_IMPORTER          "<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\n \
                                                                <adtf:ddl xmlns:adtf=\"adtf\"> \n \
                                                                    <header> \
                                                                        <language_version>1.02</language_version>\n \
                                                                        <author></author>\n \
                                                                        <date_creation></date_creation>\n \
                                                                        <date_change></date_change>\n \
                                                                        <description>ADTF generated</description>\n \
                                                                    </header> \n \
                                                                    <units/> \n\
                                                                    <datatypes/> \n\
                                                                    <structs/> \n\
                                                                    <streams/> \n\
                                                                </adtf:ddl>"

#define DESCRPTIONFILE_HEADER_DDL2_DEFAULT_IMPORTER          "<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\n \
                                                                <adtf:ddl xmlns:adtf=\"adtf\"> \n \
                                                                    <header> \
                                                                        <language_version>2.00</language_version>\n \
                                                                        <author></author>\n \
                                                                        <date_creation></date_creation>\n \
                                                                        <date_change></date_change>\n \
                                                                        <description>ADTF generated</description>\n \
                                                                    </header> \n \
                                                                    <units/> \n\
                                                                    <datatypes/> \n\
                                                                    <enums/> \n\
                                                                    <structs/> \n\
                                                                    <streams/> \n\
                                                                </adtf:ddl>"

#define DESCRPTIONFILE_HEADER_DDL3_DEFAULT_IMPORTER           "<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\n \
                                                                    <adtf:ddl xmlns:adtf=\"adtf\"> \n \
                                                                    <header> \
                                                                        <language_version>3.00</language_version>\n \
                                                                        <author></author>\n \
                                                                        <date_creation></date_creation>\n \
                                                                        <date_change></date_change>\n \
                                                                        <description>ADTF generated</description>\n \
                                                                    </header> \n \
                                                                    <units/> \n\
                                                                    <datatypes/> \n\
                                                                    <enums/> \n\
                                                                    <structs/> \n\
                                                                    <streams/> \n\
                                                                </adtf:ddl>"

#define DESCRPTIONFILE_HEADER_DDL4_DEFAULT_IMPORTER           "<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\n \
                                                                    <adtf:ddl xmlns:adtf=\"adtf\"> \n \
                                                                    <header> \
                                                                        <language_version>4.00</language_version>\n \
                                                                        <author></author>\n \
                                                                        <date_creation></date_creation>\n \
                                                                        <date_change></date_change>\n \
                                                                        <description>ADTF generated</description>\n \
                                                                    </header> \n \
                                                                    <units/> \n\
                                                                    <datatypes/> \n\
                                                                    <enums/> \n\
                                                                    <structs/> \n\
                                                                    <streams/> \n\
                                                                    <streammetatypes/> \n\
                                                                </adtf:ddl>"

#define DESCRPTIONFILE_HEADER_DDLNEWST_DEFAULT_IMPORTER DESCRPTIONFILE_HEADER_DDL4_DEFAULT_IMPORTER

namespace ddl
{
    //define all needed error types and values locally
    _MAKE_RESULT(-4, ERR_POINTER);
    _MAKE_RESULT(-20, ERR_NOT_FOUND);
    _MAKE_RESULT(-37, ERR_NOT_INITIALIZED);

    class DDLDefault
    {
        private:
            explicit DDLDefault(DDLVersion version, bool bSorted):
                m_aoPredefDTs(bSorted),
                m_aoPredefBUs(bSorted),
                m_aoPredefPrefixes(bSorted),
                m_aoPredefEnums(bSorted),
                _version(version)
            {
                initialize();
            };
            void initialize()
            {
                // Make sure this instance is only initialized once

#if defined(WIN32) && _MSC_VER < 1900
                _set_output_format(_TWO_DIGIT_EXPONENT);
#endif
                m_aoPredefDTs.insert(new DDLDataType("tBool", 8, NULL,
                    "predefined ADTF tBool datatype", DDLAlignment::e1, 1,
                    -1, a_util::strings::empty_string, true, "tFalse", true, "tTrue"));
                m_aoPredefDTs.insert(new DDLDataType("tChar", 8, NULL,
                    "predefined ADTF tChar datatype", DDLAlignment::e1, 1,
                    -1, a_util::strings::empty_string, true,
                    a_util::strings::toString((int8_t)std::numeric_limits<char>::min()), true,
                    a_util::strings::toString((int8_t)std::numeric_limits<char>::max())));
                m_aoPredefDTs.insert(new DDLDataType("tUInt8", 8, NULL,
                    "predefined ADTF tUInt8 datatype", DDLAlignment::e1, 1,
                    -1, a_util::strings::empty_string, true, "0", true,
                    a_util::strings::toString(std::numeric_limits<uint8_t>::max())));
                m_aoPredefDTs.insert(new DDLDataType("tInt8", 8, NULL,
                    "predefined ADTF tInt8 datatype", DDLAlignment::e1, 1,
                    -1, a_util::strings::empty_string, true,
                    a_util::strings::toString(std::numeric_limits<int8_t>::min()),
                    true, a_util::strings::toString(std::numeric_limits<int8_t>::max())));
                m_aoPredefDTs.insert(new DDLDataType("tUInt16", 16, NULL,
                    "predefined ADTF tUInt16 datatype", DDLAlignment::e1, 1,
                    -1, a_util::strings::empty_string, true, "0", true,
                    a_util::strings::toString(std::numeric_limits<uint16_t>::max())));
                m_aoPredefDTs.insert(new DDLDataType("tInt16", 16, NULL,
                    "predefined ADTF tInt16 datatype", DDLAlignment::e1, 1,
                    -1, a_util::strings::empty_string, true,
                    a_util::strings::toString(std::numeric_limits<int16_t>::min()),
                    true, a_util::strings::toString(std::numeric_limits<int16_t>::max())));
                m_aoPredefDTs.insert(new DDLDataType("tUInt32", 32, NULL,
                    "predefined ADTF tUInt32 datatype", DDLAlignment::e1, 1,
                    -1, a_util::strings::empty_string, true, "0", true,
                    a_util::strings::toString(std::numeric_limits<uint32_t>::max())));
                m_aoPredefDTs.insert(new DDLDataType("tInt32", 32, NULL,
                    "predefined ADTF tInt32 datatype", DDLAlignment::e1, 1,
                    -1, a_util::strings::empty_string, true,
                    a_util::strings::toString(std::numeric_limits<int32_t>::min()),
                    true, a_util::strings::toString(std::numeric_limits<int32_t>::max())));
                m_aoPredefDTs.insert(new DDLDataType("tUInt64", 64, NULL,
                    "predefined ADTF tUInt64 datatype", DDLAlignment::e1, 1,
                    -1, a_util::strings::empty_string, true, "0", true,
                    a_util::strings::toString(std::numeric_limits<uint64_t>::max())));
                m_aoPredefDTs.insert(new DDLDataType("tInt64", 64, NULL,
                    "predefined ADTF tInt64 datatype", DDLAlignment::e1, 1,
                    -1, a_util::strings::empty_string, true,
                    a_util::strings::toString(std::numeric_limits<int64_t>::min()),
                    true, a_util::strings::toString(std::numeric_limits<int64_t>::max())));
                m_aoPredefDTs.insert(new DDLDataType("tFloat32", 32,
                    NULL, "predefined ADTF tFloat32 datatype",
                    DDLAlignment::e1, 1, -1, a_util::strings::empty_string, true,
                    a_util::strings::format("%e", -std::numeric_limits<float>::max()), true,
                    a_util::strings::format("%e", std::numeric_limits<float>::max())));
                m_aoPredefDTs.insert(new DDLDataType("tFloat64", 64,
                    NULL, "predefined ADTF tFloat64 datatype",
                    DDLAlignment::e1, 1, -1, a_util::strings::empty_string, true,
                    a_util::strings::format("%e", -std::numeric_limits<double>::max()), true,
                    a_util::strings::format("%e", std::numeric_limits<double>::max())));

                for (DDLDTVec::iterator pIter = m_aoPredefDTs.begin();
                    pIter != m_aoPredefDTs.end(); pIter++)
                {
                    (*pIter)->setDDLVersion(_version);
                }

                m_aoPredefBUs.insert(new DDLBaseunit("Metre", "m",
                    "Fundamental unit for length", -1));
                m_aoPredefBUs.insert(new DDLBaseunit("Kilogram", "kg",
                    "Fundamental unit for mass", -1));
                m_aoPredefBUs.insert(new DDLBaseunit("Second", "s",
                    "Fundamental unit for time", -1));
                m_aoPredefBUs.insert(new DDLBaseunit("Ampere", "A",
                    "Fundamental unit for electric current", -1));
                m_aoPredefBUs.insert(new DDLBaseunit("Kelvin", "K",
                    "Fundamental unit for thermodynamic temperature", -1));
                m_aoPredefBUs.insert(new DDLBaseunit("Mole", "mol",
                    "Fundamental unit for amount of substance", -1));
                m_aoPredefBUs.insert(new DDLBaseunit("Candela", "cd",
                    "Fundamental unit for luminous intensity", -1));
                m_aoPredefBUs.insert(new DDLBaseunit("Degree", "deg",
                    "Non-SI standard unit for angle", -1));
                m_aoPredefBUs.insert(new DDLBaseunit("Radiant", "rad",
                    "Non-SI standard unit for angle", -1));
                m_aoPredefBUs.insert(new DDLBaseunit("Unitless", "",
                    "No SI, but needed for own unit definitions", -1));
                m_aoPredefBUs.insert(new DDLBaseunit("nou", "",
                    "No SI, but needed for no unit definitions", -1));

                m_aoPredefPrefixes.insert(new DDLPrefix("yotta", "Y",
                    24, -1));
                m_aoPredefPrefixes.insert(new DDLPrefix("zetta", "Z",
                    21, -1));
                m_aoPredefPrefixes.insert(new DDLPrefix("exa", "E", 18,
                    -1));
                m_aoPredefPrefixes.insert(new DDLPrefix("peta", "P",
                    15, -1));
                m_aoPredefPrefixes.insert(new DDLPrefix("tera", "T",
                    12, -1));
                m_aoPredefPrefixes.insert(new DDLPrefix("giga", "G", 9,
                    -1));
                m_aoPredefPrefixes.insert(new DDLPrefix("mega", "M", 6,
                    -1));
                m_aoPredefPrefixes.insert(new DDLPrefix("kilo", "k", 3,
                    -1));
                m_aoPredefPrefixes.insert(new DDLPrefix("hecto", "h",
                    2, -1));
                m_aoPredefPrefixes.insert(new DDLPrefix("deca", "da",
                    1, -1));
                m_aoPredefPrefixes.insert(new DDLPrefix("deci", "d",
                    -1, -1));
                m_aoPredefPrefixes.insert(new DDLPrefix("centi", "c",
                    -2, -1));
                m_aoPredefPrefixes.insert(new DDLPrefix("milli", "m",
                    -3, -1));
                m_aoPredefPrefixes.insert(new DDLPrefix("micro", "u",
                    -6, -1));
                m_aoPredefPrefixes.insert(new DDLPrefix("nano", "n",
                    -9, -1));
                m_aoPredefPrefixes.insert(new DDLPrefix("pico", "p",
                    -12, -1));
                m_aoPredefPrefixes.insert(new DDLPrefix("femto", "f",
                    -15, -1));
                m_aoPredefPrefixes.insert(new DDLPrefix("atto", "a",
                    -18, -1));
                m_aoPredefPrefixes.insert(new DDLPrefix("zepto", "z",
                    -21, -1));
                m_aoPredefPrefixes.insert(new DDLPrefix("yocto", "y",
                    -24, -1));

                EnumNameValueVec vecEnumValuesBitmap;
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_16BIT", "20"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_24BIT", "40"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_32BIT", "50"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_8BIT", "10"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_ABGR_4444", "29"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_ABGR_8888", "52"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_ARGB_8888", "51"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_BGRA_8888", "54"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_BGR_555", "34"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_BGR_565", "35"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_BGR_888", "46"));

                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_CUSTOM", "1000"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_GREYSCALE_10", "21"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_GREYSCALE_12", "22"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_GREYSCALE_14", "23"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_GREYSCALE_16", "24"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_GREYSCALE_18", "41"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_GREYSCALE_20", "42"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_GREYSCALE_22", "43"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_GREYSCALE_24", "44"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_GREYSCALE_32", "55"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_GREYSCALE_8", "11"));

                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_GREYSCALE_FLOAT32", "56"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_RGBA_4444", "28"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_RGBA_8888", "53"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_RGB_444", "25"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_RGB_555", "26"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_RGB_565", "27"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_RGB_8", "12"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_RGB_888", "45"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_RIII_10", "30"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_RIII_12", "31"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_RIII_14", "32"));

                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_RIII_16", "33"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_UNKNOWN", "0"));
                vecEnumValuesBitmap.push_back(std::make_pair<std::string, std::string>("PF_YUV420P_888", "60"));

                DDLEnum* pBitmapTypeEnum = new DDLEnum(m_aoPredefDTs.find("tInt16"),
                    "tPixelFormat", vecEnumValuesBitmap, NULL, -1);
                m_aoPredefEnums.insert(pBitmapTypeEnum);
            }
            virtual ~DDLDefault()
            {

                for (DDLEnumVec::iterator it = m_aoPredefEnums.begin();
                    it != m_aoPredefEnums.end(); ++it)
                {
                    delete (*it);
                }

                m_aoPredefEnums.clear();

                for (DDLDTIt oItDT = m_aoPredefDTs.begin();
                    oItDT != m_aoPredefDTs.end();
                    ++oItDT)
                {
                    delete (*oItDT);
                }
                m_aoPredefDTs.clear();

                for (DDLBaseunitIt oItBU = m_aoPredefBUs.begin();
                    oItBU != m_aoPredefBUs.end();
                    ++oItBU)
                {
                    delete (*oItBU);
                }
                m_aoPredefBUs.clear();

                for (DDLPrefixIt oItPref = m_aoPredefPrefixes.begin();
                    oItPref != m_aoPredefPrefixes.end();
                    ++oItPref)
                {
                    delete (*oItPref);
                }
                m_aoPredefPrefixes.clear();
            };

        public:
            static const DDLDefault* getInstance(DDLVersion& language_version, bool bSorted)
            {
                static const DDLDefault m_gDefaultDDL_v10(DDLVersion::ddl_version_10, false);
                static const DDLDefault m_gDefaultDDL_v20(DDLVersion::ddl_version_20, false);
                static const DDLDefault m_gDefaultDDL_v30(DDLVersion::ddl_version_30, false);
                static const DDLDefault m_gDefaultDDL_v40(DDLVersion::ddl_version_40, false);

                static const DDLDefault m_gDefaultDDL_v10_sorted(DDLVersion::ddl_version_10, true);
                static const DDLDefault m_gDefaultDDL_v20_sorted(DDLVersion::ddl_version_20, true);
                static const DDLDefault m_gDefaultDDL_v30_sorted(DDLVersion::ddl_version_30, true);
                static const DDLDefault m_gDefaultDDL_v40_sorted(DDLVersion::ddl_version_40, true);

                if (bSorted)
                {
                    if (DDLVersion::ddl_version_20 > language_version)
                    {
                        return &m_gDefaultDDL_v10_sorted;
                    }
                    else if (DDLVersion::ddl_version_20 <= language_version &&  DDLVersion::ddl_version_30 > language_version)
                    {
                        return &m_gDefaultDDL_v20_sorted;
                    }
                    else if (DDLVersion::ddl_version_30 <= language_version &&  DDLVersion::ddl_version_40 > language_version)
                    {
                        return &m_gDefaultDDL_v30_sorted;
                    }
                    else
                    {
                        return &m_gDefaultDDL_v40_sorted;
                    }
                }
                else
                {
                    if (DDLVersion::ddl_version_20 > language_version)
                    {
                      return &m_gDefaultDDL_v10;
                    }
                    else if (DDLVersion::ddl_version_20 <= language_version &&  DDLVersion::ddl_version_30 > language_version)
                    {
                        return &m_gDefaultDDL_v20;
                    }
                    else if (DDLVersion::ddl_version_30 <= language_version &&  DDLVersion::ddl_version_40 > language_version)
                    {
                        return &m_gDefaultDDL_v30;
                    }
                    else
                    {
                      return &m_gDefaultDDL_v40;
                    }
                }
            }

           

            const DDLDTVec& GetDefaultDTs() const
            {
                return m_aoPredefDTs;
            }

            const DDLBaseunitVec& GetDefaultBaseunits() const
            {
                return m_aoPredefBUs;
            }

            const DDLPrefixVec& GetDefaultPrefixes() const
            {
                return m_aoPredefPrefixes;
            }

            const DDLEnumVec& GetDefaultEnums() const
            {
                return m_aoPredefEnums;
            }


        public:
             DDLDTVec       m_aoPredefDTs;
             DDLBaseunitVec m_aoPredefBUs;
             DDLPrefixVec   m_aoPredefPrefixes;
             DDLEnumVec     m_aoPredefEnums;
             DDLVersion     _version;
    };


    DDLDescription::DDLDescription(DDLHeader* poHeader,
                                     const DDLUnitVec &vecDDLUnits,
                                     const DDLBaseunitVec &vecDDLBaseunits,
                                     const DDLPrefixVec &vecDDLPrefixes,
                                     const DDLDTVec &vecDTs,
                                     const DDLComplexVec &vecStructs,
                                     const DDLStreamVec &vecStreams,
                                     const DDLEnumVec &vecEnums,
                                     bool bMergeDefaults) :
                                    _header(poHeader),
                                    _units(vecDDLUnits.isSorted()),
                                    _baseunits(vecDDLBaseunits.isSorted()),
                                    _prefixes(vecDDLPrefixes.isSorted()),
                                    _data_types(vecDTs.isSorted()),
                                    _enums(vecEnums.isSorted()),
                                    _structs(vecStructs.isSorted()),
                                    _streams(vecStreams.isSorted()),
                                    _merge_defaults(bMergeDefaults)
    {
        _init_flag = (NULL != poHeader);
        cloneBaseunits(vecDDLBaseunits);
        clonePrefixes(vecDDLPrefixes);
        cloneUnits(vecDDLUnits);
        cloneDatatypes(vecDTs);
        cloneStructs(vecStructs);
        cloneStreams(vecStreams);
        cloneEnums(vecEnums);
    }

    DDLDescription& DDLDescription::operator=(DDLDescription other)
    {
        swap(*this, other);
        return *this;
    }

    DDLDescription::DDLDescription(DDLDescription&& other)
    {
        swap(*this, other);
    }

    DDLDescription::~DDLDescription()
    {
        delete _header;
        _header = NULL;

        // delete child objects
        _baseunits.deleteAll();
        _prefixes.deleteAll();
        _units.deleteAll();
        _data_types.deleteAll();
        _structs.deleteAll();
        _streams.deleteAll();
        _enums.deleteAll();
    }

    a_util::result::Result DDLDescription::accept(IDDLVisitor *poVisitor) const
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        return poVisitor->visitDDL(this);
    }

    a_util::result::Result DDLDescription::accept(IDDLChangeVisitor *poVisitor)
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        return poVisitor->visitDDL(this);
    }

    const std::string& DDLDescription::getName() const
    {
        static std::string s_Name("DDL");
        return s_Name;
    }

    a_util::result::Result DDLDescription::create(DDLHeader* poHeader,
                                    DDLUnitVec vecDDLUnits,
                                    DDLBaseunitVec vecDDLBaseunits,
                                    DDLPrefixVec vecDDLPrefixes,
                                    DDLDTVec vecDTs,
                                    DDLComplexVec vecStructs,
                                    DDLStreamVec vecStreams,
                                    DDLEnumVec vecEnums)
    {
        if (!poHeader) { return ERR_POINTER; }
        _header = poHeader;
        cloneBaseunits(vecDDLBaseunits);
        clonePrefixes(vecDDLPrefixes);
        cloneDatatypes(vecDTs);
        cloneStructs(vecStructs);
        cloneStreams(vecStreams);
        cloneEnums(vecEnums);
        _init_flag = true;
        return a_util::result::SUCCESS;
    }

    DDLHeader * DDLDescription::getHeader() const
    {
        return _header;
    }

    void DDLDescription::cloneUnits(const DDLUnitVec& vecDDLUnits)
    {
        _units.deleteAll();
        mergeUnits(vecDDLUnits, IDDL::ddl_merge_force_overwrite, NULL);
    }

    void DDLDescription::refUnits(DDLUnitVec& vecDDLUnits)
    {
        _units.deleteAll();
        mergePart(_units, vecDDLUnits, IDDL::ddl_merge_force_overwrite, NULL, NULL, true, true);
    }

    void DDLDescription::mergeUnits(DDLUnitVec vecDDLUnits, uint32_t ui32JoinFlags, DDLVec* pvecDeleteData)
    {
        mergePart(_units, vecDDLUnits, ui32JoinFlags, pvecDeleteData, this, false);
    }

    void DDLDescription::addUnit(DDLUnit* poUnit, int nPos)
    {
        if (NULL != poUnit)
        {
            _units.insert(poUnit, nPos);
        }
    }

    DDLUnitVec& DDLDescription::getUnits()
    {
        return _units;
    }

    const DDLUnitVec& DDLDescription::getUnits() const
    {
        return _units;
    }

    const DDLUnit * DDLDescription::getUnitByName(const std::string& name) const
    {
        return _units.find(name);
    }

    DDLUnit * DDLDescription::getUnitByName(const std::string& name)
    {
        return _units.find(name);
    }

    void DDLDescription::cloneBaseunits(const DDLBaseunitVec& vecDDLBaseunits)
    {
        _baseunits.deleteAll();
        if (_merge_defaults)
        {
            DDLVersion version = DDLVersion::getDefaultVersion();
            if (NULL != _header)
            {
                version = _header->getLanguageVersion();
            }
            _baseunits.cloneFrom(DDLDefault::getInstance(version, _baseunits.isSorted())->GetDefaultBaseunits());
            mergeBaseunits(vecDDLBaseunits, IDDL::ddl_merge_force_overwrite, NULL);
        }
        else
        {
            _baseunits.cloneFrom(vecDDLBaseunits);
        }
    }

    void DDLDescription::refBaseunits(DDLBaseunitVec& vecDDLBaseunits)
    {
        _baseunits.deleteAll();
        if (_merge_defaults)
        {
            DDLVersion version = DDLVersion::getDefaultVersion();
            if (NULL != _header)
            {
                version = _header->getLanguageVersion();
            }
            _baseunits.cloneFrom(DDLDefault::getInstance(version, _baseunits.isSorted())->GetDefaultBaseunits());
            mergePart(_baseunits, vecDDLBaseunits, IDDL::ddl_merge_force_overwrite, NULL, NULL, true, true);
        }
        else
        {
            _baseunits.copyFromRef(vecDDLBaseunits);
        }
    }

    void DDLDescription::mergeBaseunits(DDLBaseunitVec vecDDLBaseunits, uint32_t ui32JoinFlags, DDLVec* pvecDeleteData)
    {
        mergePart(_baseunits, vecDDLBaseunits, ui32JoinFlags, pvecDeleteData, this, false);
    }

    void DDLDescription::addBaseunit(DDLBaseunit* poBaseunit, int nPos)
    {
        if (NULL != poBaseunit)
        {
            _baseunits.insert(poBaseunit, nPos);
        }
    }

    DDLBaseunitVec DDLDescription::getBaseunits() const
    {
        return _baseunits;
    }

    DDLBaseunit * DDLDescription::getBaseunitByName(const std::string& name)
    {
        return _baseunits.find(name);
    }

    const DDLBaseunit * DDLDescription::getBaseunitByName(const std::string& name) const
    {
        return _baseunits.find(name);
    }

    void DDLDescription::clonePrefixes(const DDLPrefixVec& vecDDLPrefixes)
    {
        _prefixes.deleteAll();
        if (_merge_defaults)
        {
            DDLVersion version = DDLVersion::getDefaultVersion();
            if (NULL != _header)
            {
                version = _header->getLanguageVersion();
            }
            _prefixes.cloneFrom(DDLDefault::getInstance(version, _prefixes.isSorted())->GetDefaultPrefixes());
            mergePrefixes(vecDDLPrefixes, IDDL::ddl_merge_force_overwrite, NULL);
        }
        else
        {
            _prefixes.cloneFrom(vecDDLPrefixes);
        }
    }

    void DDLDescription::refPrefixes(DDLPrefixVec& vecDDLPrefixes)
    {
        _prefixes.deleteAll();
        if (_merge_defaults)
        {
            DDLVersion version = DDLVersion::getDefaultVersion();
            if (NULL != _header)
            {
                version = _header->getLanguageVersion();
            }
            _prefixes.cloneFrom(DDLDefault::getInstance(version, _prefixes.isSorted())->GetDefaultPrefixes());
            mergePart(_prefixes, vecDDLPrefixes, IDDL::ddl_merge_force_overwrite, NULL, NULL, true , true);
        }
        else
        {
            _prefixes.copyFromRef(vecDDLPrefixes);
        }
    }

    void DDLDescription::mergePrefixes(DDLPrefixVec vecDDLPrefixes, uint32_t ui32JoinFlags, DDLVec* pvecDeleteData)
    {
        mergePart(_prefixes, vecDDLPrefixes, ui32JoinFlags, pvecDeleteData, this, false);
    }

    void DDLDescription::addPrefix(DDLPrefix* poPrefix, int nPos)
    {
        if (NULL != poPrefix)
        {
            _prefixes.insert(poPrefix, nPos);
        }
    }

    DDLPrefixVec DDLDescription::getPrefixes() const
    {
        return _prefixes;
    }

    const DDLPrefix * DDLDescription::getPrefixByName(const std::string& name) const
    {
        return _prefixes.find(name);
    }

    DDLPrefix * DDLDescription::getPrefixByName(const std::string& name)
    {
        return _prefixes.find(name);
    }

    void DDLDescription::cloneDatatypes(const DDLDTVec& vecDDLDataTypes)
    {
        _data_types.deleteAll();
        if (_merge_defaults)
        {
            DDLVersion version = DDLVersion::getDefaultVersion();
            if (NULL != _header)
            {
                version = _header->getLanguageVersion();
            }
            _data_types.cloneFrom(DDLDefault::getInstance(version, _data_types.isSorted())->GetDefaultDTs());
            mergeDatatypes(vecDDLDataTypes, IDDL::ddl_merge_force_overwrite, NULL);
        }
        else
        {
            _data_types.cloneFrom(vecDDLDataTypes);
        }
    }

    void DDLDescription::refDatatypes(DDLDTVec& vecDDLDataTypes)
    {
        _data_types.deleteAll();
        if (_merge_defaults)
        {
            DDLVersion version = DDLVersion::getDefaultVersion();
            if (NULL != _header)
            {
                version = _header->getLanguageVersion();
            }
            _data_types.cloneFrom(DDLDefault::getInstance(version, _data_types.isSorted())->GetDefaultDTs());
            mergePart(_data_types, vecDDLDataTypes, IDDL::ddl_merge_force_overwrite, NULL, NULL, true, true);
        }
        else
        {
            _data_types.copyFromRef(vecDDLDataTypes);
        }
    }

    void DDLDescription::mergeDatatypes(DDLDTVec vecDDLDataTypes, uint32_t ui32JoinFlags, DDLVec* pvecDeleteData)
    {
        mergePart(_data_types, vecDDLDataTypes, ui32JoinFlags, pvecDeleteData, this, false);
    }

    void DDLDescription::addDatatype(DDLDataType* poDatatype, int nPos)
    {
        if (NULL != poDatatype)
        {
            _data_types.insert(poDatatype, nPos);
        }
    }

    DDLDTVec& DDLDescription::getDatatypes()
    {
        return _data_types;
    }

    const DDLDTVec& DDLDescription::getDatatypes() const
    {
        return _data_types;
    }

    DDLDataType * DDLDescription::getDataTypeByName(const std::string& name)
    {
        return _data_types.find(name);
    }

    const DDLDataType * DDLDescription::getDataTypeByName(const std::string& name) const
    {
        return _data_types.find(name);
    }

    void DDLDescription::cloneStructs(const DDLComplexVec& vecStructs)
    {
        _structs.deleteAll();
        _structs.clear();
        //initdefault ...
        mergeStructs(vecStructs, IDDL::ddl_merge_force_overwrite, NULL);
    }


    void DDLDescription::refStructs(DDLComplexVec& vecStructs)
    {
        _structs.deleteAll();
        _structs.clear();
        //initdefault ...
        mergePart(_structs, vecStructs, IDDL::ddl_merge_force_overwrite, NULL, NULL, true, true);
    }

    void DDLDescription::mergeStructs(DDLComplexVec vecStructs, uint32_t ui32JoinFlags, DDLVec* pvecDeleteData)
    {
        mergePart(_structs, vecStructs, ui32JoinFlags, pvecDeleteData, this, false, false);
    }

    void DDLDescription::addStruct(DDLComplex* poStruct, int nPos)
    {
        if (NULL != poStruct)
        {
            _structs.insert(poStruct, nPos);
        }
    }

    bool DDLDescription::hasDynamicStructs()
    {
        bool _dynamic_structs = false;
        for (DDLComplexIt itEl = _structs.begin(); itEl != _structs.end(); itEl++)
        {
            if ((*itEl)->hasDynamicElements())
            {
                _dynamic_structs = true;
                break;
            }
        }
        return _dynamic_structs;
    }

    DDLComplexVec& DDLDescription::getStructs()
    {
        return _structs;
    }

    const DDLComplexVec& DDLDescription::getStructs() const
    {
        return _structs;
    }

    const DDLComplex * DDLDescription::getStructByName(const std::string& name) const
    {
        return _structs.find(name);
    }

    DDLComplex* DDLDescription::getStructByName(const std::string& name)
    {
        return _structs.find(name);
    }

    a_util::result::Result DDLDescription::removeType(const std::string& name)
    {
        DDLDTIt itDTFound = _data_types.findIt(name);
        if (_data_types.end() == itDTFound)
        {
            DDLComplexIt itStructFound = _structs.findIt(name);
            if (_structs.end() == itStructFound)
            {
                return ERR_NOT_FOUND;
            }
            DDLDescription::deleteChild(*itStructFound);
            _structs.erase(itStructFound);
        }
        else
        {
            deleteChild(*itDTFound);
            _data_types.erase(itDTFound);
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLDescription::removeDataType(const std::string& name)
    {
        DDLDTIt itDTFound = _data_types.findIt(name);
        if (_data_types.end() == itDTFound)
        {
            return ERR_NOT_FOUND;
        }
        else
        {
            deleteChild(*itDTFound);
            _data_types.erase(itDTFound);
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLDescription::removeComplexDataType(const std::string& name)
    {
        DDLComplexIt itStructFound = _structs.findIt(name);
        if (_structs.end() == itStructFound)
        {
            return ERR_NOT_FOUND;
        }
        else
        {
            DDLDescription::deleteChild(*itStructFound);
            _structs.erase(itStructFound);
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLDescription::moveHeaderExcDec
    (
        const int nFrom,
        const int nTo
    )
    {
        return DDLDescription::moveChild(&_header->getExtDeclarations(), nFrom, nTo);
    }

    a_util::result::Result DDLDescription::moveBaseunit
    (
        const int nFrom,
        const int nTo
    )
    {
        return DDLDescription::moveChild(&_baseunits, nFrom, nTo);
    }

    a_util::result::Result DDLDescription::movePrefix
    (
        const int nFrom,
        const int nTo
    )
    {
        return DDLDescription::moveChild(&_prefixes, nFrom, nTo);
    }

    a_util::result::Result DDLDescription::moveUnit
    (
        const int nFrom,
        const int nTo
    )
    {
        return DDLDescription::moveChild(&_units, nFrom, nTo);
    }

    a_util::result::Result DDLDescription::moveRefUnit
    (
        const int nFrom,
        const int nTo,
        const std::string& strParent
    )
    {
        DDLUnitIt itUnitFound = _units.findIt(strParent);
        if (_units.end() == itUnitFound)
        {
            return ERR_NOT_FOUND;
        }

        return DDLDescription::moveChild(&((*itUnitFound)->getRefUnits()), nFrom, nTo);
    }

    a_util::result::Result DDLDescription::moveEnum
    (
        const int nFrom,
        const int nTo
    )
    {
        return DDLDescription::moveChild(&_enums, nFrom, nTo);
    }

    a_util::result::Result DDLDescription::moveEnumElement
    (
        const int nFrom,
        const int nTo,
        const std::string& strParent
    )
    {
        DDLEnumIt itEnumFound = _enums.findIt(strParent);
        if (_enums.end() == itEnumFound)
        {
            return ERR_NOT_FOUND;
        }

        EnumNameValueVec vecValue = (*itEnumFound)->getValues();
        RETURN_IF_FAILED(DDLDescription::moveChild(&vecValue, nFrom, nTo));

        (*itEnumFound)->setValues(vecValue);
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLDescription::moveDatatype
    (
        const int nFrom,
        const int nTo
    )
    {
        return DDLDescription::moveChild(&_data_types, nFrom, nTo);
    }

    a_util::result::Result DDLDescription::moveComplexDataType
        (
        const int nFrom,
        const int nTo
        )
    {
        return DDLDescription::moveChild(&_structs, nFrom, nTo);
    }

    a_util::result::Result DDLDescription::moveComplexDataTypeElement
        (
        const int nFrom,
        const int nTo,
        const std::string& strParent
        )
    {
        DDLComplexIt itStructFound = _structs.findIt(strParent);
        if (_structs.end() == itStructFound)
        {
            return ERR_NOT_FOUND;
        }

        return DDLDescription::moveChild(&((*itStructFound)->getElements()), nFrom, nTo);
    }

    a_util::result::Result DDLDescription::moveStream
    (
        const int nFrom,
        const int nTo
    )
    {
        return DDLDescription::moveChild(&_streams, nFrom, nTo);
    }

    a_util::result::Result DDLDescription::removeEnum( const std::string& name )
    {
        DDLEnumIt itEnum = _enums.findIt(name);
        if (itEnum == _enums.end())
        {
            return ERR_NOT_FOUND;
        }

        deleteChild(*itEnum);
        _enums.erase(itEnum);

        return a_util::result::SUCCESS;
    }


    void DDLDescription::cloneStreams(const DDLStreamVec& vecStreams)
    {
        _streams.deleteAll();
        _streams.clear();
        //initdefault ...
        mergeStreams(vecStreams, IDDL::ddl_merge_force_overwrite, NULL);
    }

    void DDLDescription::refStreams(DDLStreamVec& vecStreams)
    {
       _streams.deleteAll();
        _streams.clear();
        //initdefault ...
        mergePart(_streams, vecStreams, IDDL::ddl_merge_force_overwrite, NULL, NULL, true, true);
    }

    void DDLDescription::mergeStreams(DDLStreamVec vecStreams, uint32_t ui32JoinFlags, DDLVec* pvecDeleteData)
    {
        mergePart(_streams, vecStreams, ui32JoinFlags, pvecDeleteData, this, false);
    }

    void DDLDescription::addStream(DDLStream* poStream, int nPos)
    {
        if (NULL != poStream)
        {
            _streams.insert(poStream, nPos);
        }
    }

    DDLStreamVec& DDLDescription::getStreams()
    {
        return _streams;
    }

    const DDLStreamVec& DDLDescription::getStreams() const
    {
        return _streams;
    }

    DDLStream* DDLDescription::getStreamByName(const std::string& name)
    {
        return _streams.find(name);
    }

    const DDLStream* DDLDescription::getStreamByName(const std::string& name) const
    {
        return _streams.find(name);
    }

    a_util::result::Result DDLDescription::renameStream(const std::string& strOldName, const std::string& strNewName)
    {
        DDLStream * poStream = getStreamByName(strOldName);
        if (NULL == poStream)
        {
            return ERR_NOT_FOUND;
        }
        poStream->setName(strNewName);
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLDescription::removeStream(const std::string& name)
    {
        DDLStreamIt itFound = _streams.findIt(name);
        if (_streams.end() == itFound)
        {
            return ERR_NOT_FOUND;
        }
        deleteChild(*itFound);
        _streams.erase(itFound);
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLDescription::removeBaseunit(const std::string& name)
    {
        DDLBaseunitIt itFound = _baseunits.findIt(name);
        if (_baseunits.end() == itFound)
        {
            return ERR_NOT_FOUND;
        }
        deleteChild(*itFound);
        _baseunits.erase(itFound);
        return a_util::result::SUCCESS;
    }


    a_util::result::Result DDLDescription::removeUnit(const std::string& name)
    {
        DDLUnitIt itFound = _units.findIt(name);
        if (_units.end() == itFound)
        {
            return ERR_NOT_FOUND;
        }
        deleteChild(*itFound);
        _units.erase(itFound);
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLDescription::removePrefix(const std::string& name)
    {
        DDLPrefixIt itFound = _prefixes.findIt(name);
        if (_prefixes.end() == itFound)
        {
            return ERR_NOT_FOUND;
        }
        deleteChild(*itFound);
        _prefixes.erase(itFound);
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLDescription::merge(DDLDescription &oDDL, uint32_t ui32JoinFlags)
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }

        if (oDDL.getHeader()->getLanguageVersion() >
            _header->getLanguageVersion())
        {
            LOG_INFO(a_util::strings::format("Merge of %s version to %s language version",
                oDDL.getHeader()->getLanguageVersion().toString().c_str(), _header->getLanguageVersion().toString().c_str()).c_str());
            _header->setLanguageVersion(oDDL.getHeader()->getLanguageVersion());
        }

        DDLVec vecDeletionVector; //(this is the vector for all vector items that can be deleted after merge)

        mergeBaseunits(oDDL.getBaseunits(), ui32JoinFlags, &vecDeletionVector);
        mergePrefixes(oDDL.getPrefixes(), ui32JoinFlags,  &vecDeletionVector);
        mergeUnits(oDDL.getUnits(), ui32JoinFlags, &vecDeletionVector);
        mergeDatatypes(oDDL.getDatatypes(), ui32JoinFlags, &vecDeletionVector);
        mergeEnums(oDDL.getEnums(), ui32JoinFlags, &vecDeletionVector);
        mergeStructs(oDDL.getStructs(), ui32JoinFlags, &vecDeletionVector);
        mergeStreams(oDDL.getStreams(), ui32JoinFlags, &vecDeletionVector);

        DDLRepair oRepair;
        oRepair.visitDDL(this);
        vecDeletionVector.deleteAll();

        _header->setDateChange(a_util::datetime::getCurrentLocalDate());
        return a_util::result::SUCCESS;
    }

    bool DDLDescription::isInitialized() const
    {
        return _init_flag;
    }

    a_util::result::Result DDLDescription::restoreLevel(int nLevel)
    {
        for (DDLStreamIt itStream = _streams.begin();
            _streams.end() != itStream; ++itStream)
        {
            if ((*itStream)->getCreationLevel() >= nLevel)
            {
                *itStream = deleteChild(*itStream);
            }
        }
        DDLStreamIt itRemStream = std::remove_if(_streams.begin(),
            _streams.end(), std::bind1st(std::equal_to<DDLStream*>(),
            static_cast<DDLStream*>(NULL)));
        _streams.erase(itRemStream, _streams.end());

        for (DDLComplexIt itStruct = _structs.begin();
            _structs.end() != itStruct; ++itStruct)
        {
            if ((*itStruct)->getCreationLevel() >= nLevel)
            {
                *itStruct = deleteChild(*itStruct);
            }
        }
        DDLComplexIt itRemStruct = std::remove_if(_structs.begin(),
            _structs.end(), std::bind1st(std::equal_to<DDLComplex*>(),
            static_cast<DDLComplex*>(NULL)));
        _structs.erase(itRemStruct, _structs.end());

        for (DDLDTIt itDT = _data_types.begin(); _data_types.end() != itDT;
            ++itDT)
        {
            if ((*itDT)->getCreationLevel() >= nLevel)
            {
                *itDT = deleteChild(*itDT);
            }
        }
        DDLDTIt itRemDT = std::remove_if(_data_types.begin(),
            _data_types.end(), std::bind1st(std::equal_to<DDLDataType*>(),
            static_cast<DDLDataType*>(NULL)));
        _data_types.erase(itRemDT, _data_types.end());

        for (DDLUnitIt itUnit = _units.begin(); _units.end() != itUnit;
            ++itUnit)
        {
            if ((*itUnit)->getCreationLevel() >= nLevel)
            {
                *itUnit = deleteChild(*itUnit);
            }
        }
        DDLUnitIt itRemUnit = std::remove_if(_units.begin(),
            _units.end(), std::bind1st(std::equal_to<DDLUnit*>(),
            static_cast<DDLUnit*>(NULL)));
        _units.erase(itRemUnit, _units.end());

        for (DDLPrefixIt itPrefix = _prefixes.begin();
            _prefixes.end() != itPrefix; ++itPrefix)
        {
            if ((*itPrefix)->getCreationLevel() >= nLevel)
            {
                *itPrefix = deleteChild(*itPrefix);
            }
        }
        DDLPrefixIt itRemPrefix = std::remove_if(_prefixes.begin(),
            _prefixes.end(), std::bind1st(std::equal_to<DDLPrefix*>(),
            static_cast<DDLPrefix*>(NULL)));
        _prefixes.erase(itRemPrefix, _prefixes.end());

        for (DDLBaseunitIt itBU = _baseunits.begin();
            _baseunits.end() != itBU; ++itBU)
        {
            if ((*itBU)->getCreationLevel() >= nLevel)
            {
                *itBU = deleteChild(*itBU);
            }
        }
        DDLBaseunitIt itRemBU = std::remove_if(_baseunits.begin(),
            _baseunits.end(), std::bind1st(std::equal_to<DDLBaseunit*>(),
            static_cast<DDLBaseunit*>(NULL)));
        _baseunits.erase(itRemBU, _baseunits.end());


        for (DDLEnumIt itEnum = _enums.begin();
            _enums.end() != itEnum; ++itEnum)
        {
            if ((*itEnum)->getCreationLevel() >= nLevel)
            {
                *itEnum = deleteChild(*itEnum);
            }
        }
        DDLEnumIt itRemEnum = std::remove_if(_enums.begin(),
            _enums.end(), std::bind1st(std::equal_to<DDLEnum*>(),
            static_cast<DDLEnum*>(NULL)));
        _enums.erase(itRemEnum, _enums.end());

        DDLInspector oInspector;
        return oInspector.visitDDL(this);
    }

    DDLDescription * DDLDescription::createDefault(int const nCreationLevel, bool bSorted)
    {
        return createDefault(DDLVersion::ddl_version_invalid, nCreationLevel, bSorted);
    }

    DDLDescription * DDLDescription::createDefault(const DDLVersion& version, int const nCreationLevel, bool bSorted)
    {
        DDLImporter oImporter(nCreationLevel, false, bSorted);
        std::string strDescription;
        if (version == DDLVersion::ddl_version_invalid)
        {
            strDescription = DESCRPTIONFILE_HEADER_DDLNEWST_DEFAULT_IMPORTER;
        }
        else if(version <  DDLVersion::ddl_version_20)
        {
            strDescription = DESCRPTIONFILE_HEADER_DDL1_DEFAULT_IMPORTER;
        }
        else if (version <  DDLVersion::ddl_version_30)
        {
            strDescription = DESCRPTIONFILE_HEADER_DDL2_DEFAULT_IMPORTER;
        }
        else if (version <  DDLVersion::ddl_version_40)
        {
            strDescription = DESCRPTIONFILE_HEADER_DDL3_DEFAULT_IMPORTER;
        }
        else
        {
            strDescription = DESCRPTIONFILE_HEADER_DDL4_DEFAULT_IMPORTER;
        }

        if (isFailed(oImporter.setXML(strDescription)))
        {
            return NULL;
        }

        if (isFailed(oImporter.createNew(version)))
        {
            return NULL;
        }
        DDLDescription *poDDL = oImporter.getDDL();
        a_util::datetime::Date sDate = a_util::datetime::getCurrentLocalDate();
        DDLHeader* poHeader = poDDL->getHeader();
        poHeader->setDateCreation(sDate);
        poHeader->setDateChange(sDate);
        poHeader->setAuthor(a_util::system::getCurrentUserName());
        return poDDL;
    }

    template<typename T>
    bool DDLDescription::mergePart(DDLContainer<T> &vecMemberData,
                                     DDLContainer<T> &vecOtherData,
                                     uint32_t ui32JoinFlags,
                                     DDLVec* pvecDeleteData,
                                     DDLDescription* pDesc,
                                     const bool& bRefs,
                                     const bool& bDeleteNonOverwritten)
    {
        typedef typename DDLContainer<T>::iterator tMergeIt;
        bool bVectorUpdated = false;
        //fast merge only copies
        if (vecMemberData.empty())
        {
            if (!vecOtherData.empty())
            {
                if (bRefs)
                {
                    vecMemberData.copyFromRef(vecOtherData);
                }
                else
                {
                    vecMemberData.cloneFrom(vecOtherData);
                }

                bVectorUpdated = true;
            }
        }
        else
        {
            bool bOverWrite = ((IDDL::ddl_merge_force_overwrite | ui32JoinFlags) == IDDL::ddl_merge_force_overwrite);
            for (tMergeIt itRefOther = vecOtherData.begin();
                     itRefOther != vecOtherData.end();
                     itRefOther++)
            {
                //search if part of MemberData for the Member data and needs to be overwritten
                tMergeIt itFound = vecMemberData.findIt((*itRefOther)->getName());

                if (itFound != vecMemberData.end())
                {
                    T* pFound = *itFound;

                    //found and copy content
                    if (bOverWrite)
                    {
                        auto data_type_ref = reinterpret_cast<DDLDataType*>(*itRefOther);
                        auto data_type = reinterpret_cast<DDLDataType*>(pFound);
                        if (data_type_ref && data_type && (std::is_same<T, DDLDataType>::value))
                        {
                            copyMinMaxValues(data_type, data_type_ref);
                        }

                        if (pFound->isOverwriteable())
                        {
                            if (bRefs)
                            {
                                //nothing to do
                            }
                            else
                            {
                                *itFound = clone<T>(*itRefOther);
                            }
                            if (pvecDeleteData)
                            {
                                pvecDeleteData->insert(pFound);
                            }
                            else
                            {
                                if (pDesc)
                                {
                                    DDLRepair oDDLRepair;
                                    oDDLRepair.visitDDL(pDesc);
                                }
                                pFound = deleteChild<T>(pFound);
                            }
                            bVectorUpdated = (pFound != NULL);
                        }
                        else
                        {
                            if (bDeleteNonOverwritten)
                            {
                                deleteChild<T>(*itRefOther);
                                *itRefOther = NULL;
                            }
                        }
                    }
                    else
                    {
                        //only delete from this Members!!
                    }
                }
                else
                {
                    if (bRefs)
                    {
                        vecMemberData.insert(*itRefOther);
                    }
                    else
                    {
                        vecMemberData.insert(clone<T>(*itRefOther));
                    }
                    bVectorUpdated = true;
                }
            }
        }

        return bVectorUpdated;
    }

    void DDLDescription::copyMinMaxValues(DDLDataType* dest_container, DDLDataType* src_container)
    {
        // isValid is set if a value is defined
        if (src_container->isMaxValid())
        {
            dest_container->setMaxValue(src_container->getMaxValue());
        }
        if (src_container->isMinValid())
        {
            dest_container->setMinValue(src_container->getMinValue());
        }
    }

    void DDLDescription::refEnums(DDLEnumVec& vecDDLEnums)
    {
        _enums.deleteAll();
        if (_merge_defaults)
        {
            DDLVersion version = DDLVersion::getDefaultVersion();
            if (NULL != _header)
            {
                version = _header->getLanguageVersion();
            }

            _enums.cloneFrom(DDLDefault::getInstance(version, _enums.isSorted())->GetDefaultEnums());
            mergePart(_enums, vecDDLEnums, IDDL::ddl_merge_force_overwrite, NULL, NULL, true, true);
        }
        else
        {
            _enums.cloneFrom(vecDDLEnums);
        }
    }

    DDLEnumVec& DDLDescription::getEnums()
    {
        return _enums;
    }

    const DDLEnumVec& DDLDescription::getEnums() const
    {
        return _enums;
    }

    void DDLDescription::mergeEnums( DDLEnumVec vecDDLEnums, uint32_t ui32JoinFlags /*= 0*/, DDLVec* pvecDeleteData /*= NULL*/ )
    {
        mergePart(_enums, vecDDLEnums, ui32JoinFlags, pvecDeleteData, this, false);
    }

    void DDLDescription::cloneEnums(const DDLEnumVec & vecDDLEnums)
    {
        _enums.deleteAll();
        if (_merge_defaults)
        {
            DDLVersion version = DDLVersion::getDefaultVersion();
            if (NULL != _header)
            {
                version = _header->getLanguageVersion();
            }
            _enums.cloneFrom(DDLDefault::getInstance(version, vecDDLEnums.isSorted())->GetDefaultEnums());
            mergeEnums(vecDDLEnums, IDDL::ddl_merge_force_overwrite, NULL);
        }
        else
        {
            _enums.cloneFrom(vecDDLEnums);
        }
    }

    void DDLDescription::addEnum( DDLEnum* const poEnum, int nPos)
    {
        if (NULL != poEnum)
        {
            _enums.insert(poEnum, nPos);
        }
    }

    DDLEnum* DDLDescription::getEnumByName(const std::string& name)
    {
        return _enums.find(name);
    }

    const DDLEnum* DDLDescription::getEnumByName(const std::string& name) const
    {
        return _enums.find(name);
    }

    void DDLDescription::cloneStreamMetaTypes(const DDLStreamMetaTypeVec& vecStreamMetaTypes)
    {
        _stream_meta_types.deleteAll();
        _stream_meta_types.clear();
        //initdefault ...
        mergeStreamMetaTypes(vecStreamMetaTypes, IDDL::ddl_merge_force_overwrite, NULL);
    }

    void DDLDescription::refStreamMetaTypes(DDLStreamMetaTypeVec& vecStreamMetaTypes)
    {
       _stream_meta_types.deleteAll();
        _stream_meta_types.clear();
        //initdefault ...
        mergePart(_stream_meta_types, vecStreamMetaTypes, IDDL::ddl_merge_force_overwrite, NULL, NULL, true, true);
    }

    void DDLDescription::mergeStreamMetaTypes(DDLStreamMetaTypeVec vecStreamMetaTypes, uint32_t ui32JoinFlags, DDLVec* pvecDeleteData)
    {
        mergePart(_stream_meta_types, vecStreamMetaTypes, ui32JoinFlags, pvecDeleteData, this, false);
    }

    void DDLDescription::addStreamMetaType(DDLStreamMetaType* poStreamMetaType, int nPos)
    {
        if (NULL != poStreamMetaType)
        {
            _stream_meta_types.insert(poStreamMetaType, nPos);
        }
    }

    DDLStreamMetaTypeVec& DDLDescription::getStreamMetaTypes()
    {
        return _stream_meta_types;
    }

    const DDLStreamMetaTypeVec& DDLDescription::getStreamMetaTypes() const
    {
        return _stream_meta_types;
    }

    DDLStreamMetaType* DDLDescription::getStreamMetaTypeByName(const std::string& name)
    {
        return _stream_meta_types.find(name);
    }

    const DDLStreamMetaType* DDLDescription::getStreamMetaTypeByName(const std::string& name) const
    {
        return _stream_meta_types.find(name);
    }

    a_util::result::Result DDLDescription::renameStreamMetaType(const std::string& strOldName, const std::string& strNewName)
    {
        DDLStreamMetaType * poStreamMetaType = getStreamMetaTypeByName(strOldName);
        if (NULL == poStreamMetaType)
        {
            return ERR_NOT_FOUND;
        }
        poStreamMetaType->setName(strNewName);
        return a_util::result::SUCCESS;
    }

    a_util::result::Result DDLDescription::removeStreamMetaType(const std::string& name)
    {
        DDLStreamMetaTypeVec::iterator itFound = _stream_meta_types.findIt(name);
        if (_stream_meta_types.end() == itFound)
        {
            return ERR_NOT_FOUND;
        }
        deleteChild(*itFound);
        _stream_meta_types.erase(itFound);
        return a_util::result::SUCCESS;
    }

    void swap(DDLDescription& lhs, DDLDescription& rhs) noexcept
    {
        using std::swap;
        swap(lhs._header, rhs._header);
        swap(lhs._units, rhs._units);
        swap(lhs._baseunits, rhs._baseunits);
        swap(lhs._prefixes, rhs._prefixes);
        swap(lhs._data_types, rhs._data_types);
        swap(lhs._enums, rhs._enums);
        swap(lhs._structs, rhs._structs);
        swap(lhs._streams, rhs._streams);
        swap(lhs._stream_meta_types, rhs._stream_meta_types);
        swap(lhs._init_flag, rhs._init_flag);
        swap(lhs._merge_defaults, rhs._merge_defaults);
    }
}   // namespace ddl
