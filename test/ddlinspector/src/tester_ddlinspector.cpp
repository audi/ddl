/**
 * @file
 * Implementation of the tester for the DDL inspector.
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
#include <gtest/gtest.h>
#include "../../_common/adtf_compat.h"
#include "../../_common/compat.h"

using namespace ddl;

#define DDL_TEST_STRING \
    "<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\n \
<adtf:ddl xmlns:adtf=\"adtf\">\n \
<header>\n \
<language_version>1.0+</language_version>\n \
<author>AUDI Electronics Venture GmbH</author>\n \
<date_creation>20100407</date_creation>\n \
<date_change />\n \
<description>ADTF Common Description File</description>\n \
</header>\n \
<units>\n \
<baseunit description=\"this is the no unit class\" name=\"nou\" symbol=\"\" />\n \
</units>\n \
<datatypes>\n \
<datatype description=\"predefined ADTF tBool datatype\"   size=\"8\"   name=\"tBool\" />\n \
<datatype description=\"predefined ADTF tChar datatype\"   size=\"8\"   name=\"tChar\" />\n \
<datatype description=\"predefined ADTF tUInt8 datatype\"  size=\"8\"   name=\"tUInt8\" />\n \
<datatype description=\"predefined ADTF tInt8 datatype\"   size=\"8\"   name=\"tInt8\" />\n \
<datatype description=\"predefined ADTF tUInt16 datatype\" size=\"16\"  name=\"tUInt16\" />\n \
<datatype description=\"predefined ADTF tInt16 datatype\"  size=\"16\"  name=\"tInt16\" />\n \
<datatype description=\"predefined ADTF tUInt32 datatype\" size=\"32\"  name=\"tUInt32\" />\n \
<datatype description=\"predefined ADTF tInt32 datatype\"  size=\"32\"  name=\"tInt32\" />\n \
<datatype description=\"predefined ADTF tUInt64 datatype\" size=\"64\"  name=\"tUInt64\" />\n \
<datatype description=\"predefined ADTF tInt64 datatype\"  size=\"64\"  name=\"tInt64\" />\n \
<datatype description=\"predefined ADTF tFloat32 datatype\" size=\"32\" name=\"tFloat32\" />\n \
<datatype description=\"predefined ADTF tFloat64 datatype\" size=\"64\" name=\"tFloat64\" />\n \
</datatypes>\n \
<structs>\n \
<!-- TYPE DESCRIPTIONS-->\n \
<struct alignment=\"1\" name=\"tMediaTypeInfo\" version=\"1\">\n \
<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"ui32MajorType\" type=\"tUInt32\" />\n \
<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"4\" name=\"ui32SubType\" type=\"tUInt32\" />\n \
<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"8\" name=\"ui32Flags\" type=\"tUInt32\" />\n \
</struct>\n \
<struct alignment=\"1\" name=\"adtf.core.media_type\" version=\"1\">\n \
<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"mediatype\" type=\"tMediaTypeInfo\" />\n \
</struct>\n \
<struct alignment=\"1\" name=\"tBitmapFormat\" version=\"1\">\n \
<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"32\" name=\"nWidth\" type=\"tInt32\" />\n \
<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"18\" name=\"nHeight\" type=\"tInt32\" />\n \
<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"12\" name=\"nBitsPerPixel\" type=\"tInt16\" />\n \
<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"26\" name=\"nPixelFormat\" type=\"tInt16\" />\n \
<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"14\" name=\"nBytesPerLine\" type=\"tInt32\" />\n \
<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"28\" name=\"nSize\" type=\"tInt32\" />\n \
<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"22\" name=\"nPaletteSize\" type=\"tInt32\" />\n \
</struct>\n \
<struct name=\"tWaveFormat\" alignment=\"1\" version=\"1\">\n \
<element type=\"tInt32\"  name=\"nFormatType\"    bytepos=\"0\" arraysize=\"1\"  byteorder=\"LE\" alignment=\"1\"/>\n \
<element type=\"tInt32\"  name=\"nChannels\"      bytepos=\"4\" arraysize=\"1\"  byteorder=\"LE\" alignment=\"1\"/>\n \
<element type=\"tInt32\"  name=\"nSamplesPerSec\" bytepos=\"8\" arraysize=\"1\"  byteorder=\"LE\" alignment=\"1\"/>\n \
<element type=\"tInt32\"  name=\"nBitsPerSample\" bytepos=\"12\" arraysize=\"1\" byteorder=\"LE\" alignment=\"1\"/>\n \
<element type=\"tInt32\"  name=\"nNumSamples\"    bytepos=\"16\" arraysize=\"1\" byteorder=\"LE\" alignment=\"1\"/>\n \
<element type=\"tInt32\"  name=\"nSize\"          bytepos=\"20\" arraysize=\"1\" byteorder=\"LE\" alignment=\"1\"/>\n \
</struct>\n \
<struct name=\"adtf.type.video\" version=\"1\">\n \
<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"sMediatype\" type=\"tMediaTypeInfo\" />\n \
<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"12\" name=\"sBitmapFormat\" type=\"tBitmapFormat\" />\n \
</struct>\n \
<struct name=\"adtf.type.audio\" version=\"1\">\n \
<element type=\"tMediaTypeInfo\"    name=\"sMediatype\"    bytepos=\"0\" arraysize=\"1\" byteorder=\"LE\" alignment=\"1\"/>\n \
<element type=\"tWaveFormat\"       name=\"sWaveFormat\"   bytepos=\"12\" arraysize=\"1\" byteorder=\"LE\" alignment=\"1\"/>\n \
</struct>\n \
<!--SAMPLE CONTENT DESCRIPTIONS-->\n \
<struct alignment=\"1\" name=\"tCanMessage\" version=\"1\">\n \
<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"ui16Id\" type=\"tUInt16\" />\n \
<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"2\" name=\"ui8Channel\" type=\"tUInt8\" />\n \
<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"3\" name=\"ui8Length\" type=\"tUInt8\" />\n \
<element alignment=\"1\" arraysize=\"8\" byteorder=\"LE\" bytepos=\"4\" name=\"pui8Data\" type=\"tUInt8\" />\n \
</struct>\n \
<struct alignment=\"1\" name=\"tCanMessageExt\" version=\"1\">\n \
<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"ui32Id\" type=\"tUInt32\" />\n \
<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"4\" name=\"ui8Channel\" type=\"tUInt8\" />\n \
<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"5\" name=\"ui8Length\" type=\"tUInt8\" />\n \
<element alignment=\"1\" arraysize=\"8\" byteorder=\"LE\" bytepos=\"6\" name=\"pui8Data\" type=\"tUInt8\" />\n \
</struct>\n \
</structs>\n \
<streams>\n \
<stream description=\"can\" name=\"can_stream\" type=\"adtf.core.media_type\">\n \
<struct bytepos=\"0\" type=\"tCanMessage\" />\n \
</stream>\n \
<stream description=\"extended can\" name=\"extended_can_stream\" type=\"adtf.core.media_type\">\n \
<struct bytepos=\"0\" type=\"tCanMessageExt\" />\n \
</stream>\n \
<stream description=\"video stream\" name=\"video_stream\" type=\"adtf.type.video\" />\n \
<stream description=\"video stream\" name=\"audio_stream\" type=\"adtf.type.audio\" />\n \
<stream description=\"video stream\" type=\"adtf.type.audio\" />\n \
</streams>\n \
</adtf:ddl>"


/**
* @detail Test for auto bytepos correction in nested structs by inspecting 
* an ADTF description file for gaps/overlaps between elements
*/
TEST(cTesterDDLInspector,
    TestGapsAndPadding)
{
    TEST_REQ("ACORE-4637");

    a_util::filesystem::Path strDescriptionFileName =  "files/NestedCaps.description";

    DDLImporter oImporter(strDescriptionFileName);
    ASSERT_EQ(a_util::result::SUCCESS, oImporter.createNew());
    DDLDescription *poDDL = oImporter.getDDL();

    DDLInspector oInspector(true);
    ASSERT_EQ(a_util::result::SUCCESS, oInspector.visitDDL(poDDL));

    ASSERT_FALSE(oInspector.foundDynamicArrays());

    ImporterMsgList lstMsgs = oInspector.getSuggestions();

    DDLComplex *poComplex = poDDL->getStructByName("tBitmapFormat");
    DDLElementVec vecElement = poComplex->getElements();

    ASSERT_TRUE((0 == vecElement[0]->getBytepos())) << "Excpected Bytepos == 0";
    ASSERT_TRUE((8 == vecElement[1]->getBytepos())) << "Excpected Bytepos == 8";
    ASSERT_TRUE((60 == vecElement[2]->getBytepos())) << "Excpected Bytepos == 92";

    poComplex = poDDL->getStructByName("tNest1");
    vecElement = poComplex->getElements();
    ASSERT_TRUE((0 == vecElement[0]->getBytepos())) << "Excpected Bytepos == 0";
    ASSERT_TRUE((8 == vecElement[1]->getBytepos())) << "Excpected Bytepos == 8";
    ASSERT_TRUE((48 == vecElement[2]->getBytepos())) << "Excpected Bytepos == 80";

    poComplex = poDDL->getStructByName("tNest2");
    vecElement = poComplex->getElements();
    ASSERT_TRUE((0 == vecElement[0]->getBytepos())) << "Excpected Bytepos == 0";
    ASSERT_TRUE((8 == vecElement[1]->getBytepos())) << "Excpected Bytepos == 8";
    ASSERT_TRUE((36 == vecElement[2]->getBytepos())) << "Excpected Bytepos == 68";

    poComplex = poDDL->getStructByName("tNest3");
    vecElement = poComplex->getElements();
    ASSERT_TRUE((0 == vecElement[0]->getBytepos())) << "Excpected Bytepos == 0";
    ASSERT_TRUE((8 == vecElement[1]->getBytepos())) << "Excpected Bytepos == 8";
    ASSERT_TRUE((12 == vecElement[2]->getBytepos())) << "Excpected Bytepos == 56";

    poComplex = poDDL->getStructByName("tNestTest");
    vecElement = poComplex->getElements();
    ASSERT_TRUE((0 == vecElement[0]->getBytepos())) << "Excpected Bytepos == 0";
    ASSERT_TRUE((8 == vecElement[1]->getBytepos())) << "Excpected Bytepos == 8";
}

/**
* @detail Test, that no Node name is duplicated by inspecting
* an ADTF description file for correct names
*/
TEST(cTesterDDLInspector,
    TestNaming)
{
    TEST_REQ("ACORE-4637");

    a_util::filesystem::Path strDescriptionFileName = "files/StructNaming.description";

    // Check in Duplicate Names - StructNaming
    DDLImporter oImporter(strDescriptionFileName, false);
    ASSERT_EQ(a_util::result::SUCCESS, oImporter.createNew());
    DDLDescription *poDDL = oImporter.getDDL();

    DDLInspector oInspector;
    oInspector.visitDDL(poDDL);
    ImporterMsgList lstMsgs = oInspector.getSuggestions();

    ASSERT_TRUE(lstMsgs.size() == 1) << "One Error Expected";

    for (ImporterMsgList::iterator itMsg = lstMsgs.begin(); itMsg != lstMsgs.end(); ++itMsg)
    {
        ASSERT_TRUE(((*itMsg).severity == importer_error)) <<  "An Error Expected";
    }

    // Check in Duplicate Names - StructElementNaming
    strDescriptionFileName =  "files/StructElementNaming.description";
    DDLImporter oImporterElementNaming(strDescriptionFileName, false);
    ASSERT_EQ(a_util::result::SUCCESS, oImporterElementNaming.createNew());
    poDDL = oImporterElementNaming.getDDL();

    DDLInspector oInspectorElementNaming(false);
    oInspectorElementNaming.visitDDL(poDDL);
    lstMsgs = oInspectorElementNaming.getSuggestions();

    ASSERT_TRUE(lstMsgs.size() == 1) << "One Error Expected";

    for (ImporterMsgList::iterator itMsg = lstMsgs.begin(); itMsg != lstMsgs.end(); ++itMsg)
    {
        ASSERT_TRUE(((*itMsg).severity == importer_error)) <<  "An Error Expected";
    }
}

/**
* @detail Test that the user became a warning if he used a dynamic array 
* or an element with array size [0] by inspecting an ADTF description file for dynamic Arrays.
*/
TEST(cTesterDDLInspector,
    TestDynArraySize)
{
    TEST_REQ("ACORE-4637");

    a_util::filesystem::Path strDescriptionFileName =  "files/DynArraySize.description";

    DDLImporter oImporterDyn(strDescriptionFileName);
    ASSERT_EQ(a_util::result::SUCCESS, oImporterDyn.createNew());
    DDLDescription *poDDLDyn = oImporterDyn.getDDL();

    DDLInspector oInspectorDyn(false);
    oInspectorDyn.visitDDL(poDDLDyn);

    ASSERT_TRUE(oInspectorDyn.foundDynamicArrays()) <<  "Dynamic array should be found";

    ImporterMsgList lstMsgsDyn = oInspectorDyn.getSuggestions();

    ASSERT_TRUE((lstMsgsDyn.size() == 1)) << "One Warning Expected";

    for (ImporterMsgList::iterator itMsg = lstMsgsDyn.begin(); itMsg != lstMsgsDyn.end(); ++itMsg)
    {
        ASSERT_TRUE((*itMsg).severity == importer_warning) << "An Warning Expected";
    }

    /////
    //ARRAYSIZE == 0
    strDescriptionFileName = "files/DynArraySize.description";

    DDLImporter oImporter(strDescriptionFileName);

    ASSERT_EQ(a_util::result::SUCCESS, oImporter.createNew());
    DDLDescription *poDDL = oImporter.getDDL();
    DDLComplex *poDDLStruct = poDDL->getStructByName("tDynStructTest");
    DDLElementVec vecElement = poDDLStruct->getElements();
    vecElement[0]->setArraysize(0);

    DDLInspector oInspector(false);
    oInspector.visitDDL(poDDL);
    ImporterMsgList lstMsgs = oInspector.getSuggestions();

    ASSERT_TRUE((lstMsgs.size() == 2)) << "Two Warnings Expected";

    for (ImporterMsgList::iterator itMsg = lstMsgs.begin(); itMsg != lstMsgs.end(); ++itMsg)
    {
        ASSERT_TRUE((*itMsg).severity == importer_warning) <<  "An Warning Expected";
    }
}

/**
* @detail Test if the last byte position and last element size are correctly returned
* by inspecting an ADTF description file and returning a Byte position.
*/
TEST(cTesterDDLInspector,
    TestLastByteInclusiveElementSize)
{
    TEST_REQ("ACORE-6774");

    a_util::filesystem::Path strDescriptionFileName =  "files/NestedCaps.description";
    DDLImporter oImporter(strDescriptionFileName);
    ASSERT_EQ(a_util::result::SUCCESS, oImporter.createNew());
    DDLDescription *poDDL = oImporter.getDDL();

    DDLInspector oInspector(false);
    oInspector.visitDDL(poDDL);

    ASSERT_TRUE(7 == oInspector.getLastBytePosOfAStructReal("tBitmapFormat")) << "Excpected Bytepos == 7";
    ASSERT_TRUE(64 == oInspector.getLastBytePosOfAStructCorrected("tBitmapFormat")) << "Excpected Bytepos == 96";

    ASSERT_TRUE(18 == oInspector.getLastBytePosOfAStructReal("tNestTest")) << "Excpected Bytepos == 18";
    ASSERT_TRUE(12 == oInspector.getLastBytePosOfAStructCorrected("tNestTest")) << "Excpected Bytepos == 12";


}

/**
* @detail Test that errors in referenced sub-struct do not trigger an endless loop
* by inspecting an ADTF description file.
*/
TEST(cTesterDDLInspector,
    TestNameDuplicate)
{
    TEST_REQ("ACORE-7178");

    a_util::filesystem::Path strDescriptionFileName =  "files/invalid_duplicate.description";
    DDLImporter oImporter(strDescriptionFileName, false);
    ASSERT_EQ(a_util::result::SUCCESS, oImporter.createNew());
    DDLDescription *poDDL = oImporter.getDDL();

    DDLInspector oInspector(false);
    ASSERT_NE(a_util::result::SUCCESS, oInspector.visitDDL(poDDL));
}

/**
* @detail Test that severity of warnings give output depending on the selected level
* by inspecting an ADTF description file.
*/
TEST(cTesterDDLInspector,
    TestWarningLevel)
{
    TEST_REQ("CDDDL-23");

    a_util::filesystem::Path strDescriptionFileName = "files/base_all_duplicates.description";
    DDLImporter oImporter(strDescriptionFileName, false);
    oImporter.setMergeDefaults(false);
    ASSERT_EQ(a_util::result::SUCCESS, oImporter.createNew());
    DDLDescription *poDDL = oImporter.getDDL();
    DDLInspector oInspector(false);
    // test default is moderate
    // get some warnings about duplicates
    ASSERT_EQ(a_util::result::SUCCESS, oInspector.visitDDL(poDDL));
    ASSERT_EQ(oInspector.getSuggestions().size(), 3);
    ASSERT_EQ(oInspector.getWarningLevel(), ddl::WarningLevel::moderate);

    // no suggestions anymore
    oInspector.setWarningLevel(ddl::WarningLevel::silent);
    ASSERT_EQ(a_util::result::SUCCESS, oInspector.visitDDL(poDDL));
    ASSERT_EQ(oInspector.getWarningLevel(), ddl::WarningLevel::silent);
    ASSERT_EQ(oInspector.getSuggestions().size(), 0);

    // get 4 additional warnings about the biggest element/types first
    oInspector.setWarningLevel(ddl::WarningLevel::verbose);
    ASSERT_EQ(a_util::result::SUCCESS, oInspector.visitDDL(poDDL));
    ASSERT_EQ(oInspector.getWarningLevel(), ddl::WarningLevel::verbose);
    auto lst = oInspector.getSuggestions();
    ASSERT_EQ(oInspector.getSuggestions().size(), 7);
}

