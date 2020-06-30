/**
 * @file
 * Implementation of the tester for the DDL representation.
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
 *
 * QNX support Copyright (c) 2019 by dSPACE GmbH, Paderborn, Germany. All Rights Reserved
 */

#include <ddl.h>
#include <gtest/gtest.h>
#include "../../_common/adtf_compat.h"
#include "../../_common/compat.h"
#include "ddl_definitions.h"

using namespace ddl;

_MAKE_RESULT(-4, ERR_POINTER);
_MAKE_RESULT(-5, ERR_INVALID_ARG);
_MAKE_RESULT(-18, ERR_NO_CLASS);
_MAKE_RESULT(-36, ERR_UNKNOWN_FORMAT);
_MAKE_RESULT(-37, ERR_NOT_INITIALIZED);
_MAKE_RESULT(-38, ERR_FAILED);

static a_util::result::Result CompOutput(const std::string& strFile1, const std::string& strFile2, int nSkip = 4)
{
    std::string strContent1;
    std::string strContent2;

    if (a_util::filesystem::readTextFile(strFile1, strContent1) != a_util::filesystem::OK ||
        a_util::filesystem::readTextFile(strFile2, strContent2) != a_util::filesystem::OK)
    {
        return ::ERR_FAILED;
    }

    a_util::result::Result res = DDLCompare::isEqual(strContent1, strContent2,
        DDLCompare::dcf_all | DDLCompare::dcf_subset | DDLCompare::dcf_no_header_dates);
#ifdef __QNX__
    if (res != a_util::result::SUCCESS)
    {
        LOG_ERROR("CompOutput: LastDDLErrorCode=%d Label=%s Desc=%s",
                  DDLError::getLastDDLErrorCode().getErrorCode(),
                  DDLError::getLastDDLErrorCode().getErrorLabel(),
                  DDLError::getLastDDLErrorDescription());
    }
#endif
    return res;
}

/**
* @detail The building up of a DDL object representation. 
* Read an adtf.description file and build up the object representation
*/
TEST(cTesterDDLRep,
    TestBuildUpFile)
{
    TEST_REQ("ACORE-3904");

    DDLImporter oDDLImporter;
    ASSERT_TRUE((ERR_NOT_INITIALIZED == oDDLImporter.createNew())) << 
        "Uninitialized status of the DDLImporter not detected.";
    ASSERT_EQ(a_util::result::SUCCESS, oDDLImporter.setFile("files/adtf.description")) << 
        "Could not load adtf.description file.";
    ASSERT_EQ(a_util::result::SUCCESS,oDDLImporter.createNew()) << 
        "Creation of object hierarchy failed.";
    DDLDescription * poDDL = oDDLImporter.getDDL();
    ASSERT_TRUE( (NULL !=poDDL)) <<  "Invalid DDL object retrieved.";
    // free allocated resources
    oDDLImporter.destroyDDL();
    poDDL = NULL;
}

/**
* @detail The building up of a DDL object representation.
* Read string and build up the object representation
*/
TEST(cTesterDDLRep,
    TestBuildUpString)
{
    TEST_REQ("ACORE-3904");

    DDLImporter oDDLImporter;
    std::string strXML (DDL_TEST_STRING);
    ASSERT_EQ(a_util::result::SUCCESS,oDDLImporter.setXML(strXML)) << 
        "Could not set XML string.";
    ASSERT_EQ(a_util::result::SUCCESS,oDDLImporter.createNew()) <<  "Creation of DDL failed.";
    DDLPrinter oDDLPrinter;
    ASSERT_TRUE( (NULL !=oDDLImporter.getDDL())) <<  "Invalid DDL object retrieved.";
    // free allocated resources
    oDDLImporter.destroyDDL();
}

/**
* @detail The building up of a default DDL object representation.
* Build up the object representation from the default description
*/
TEST(cTesterDDLRep,
    TestBuildDefault)
{
    TEST_REQ("ACORE-3904");

    DDLDescription *poDDL = DDLDescription::createDefault();
    ASSERT_TRUE( (NULL !=poDDL)) <<  "Creation of default DDL failed.";
    // free allocated resources
    DDLImporter::destroyDDL(poDDL);
}

/**
* @detail The printing of the XML file based on a DDL in object representation.
* Write an own adtf.xml file containing the XML representation of the DDL
*/
TEST(cTesterDDLRep,
    TestPrinter)
{
    TEST_REQ("ACORE-3904 ACORE-6050");

    {
        DDLImporter oDDLImporter("files/printer.description");
        ASSERT_EQ(a_util::result::SUCCESS, oDDLImporter.createNew());
        DDLDescription * poDDL = oDDLImporter.getDDL();
        std::string const strTargetFile ("adtf_out.xml");
        DDLPrinter oDDLPrinter(true);
        // tidy up before new file is created
        if (a_util::filesystem::exists(strTargetFile))
        {
            a_util::filesystem::remove(strTargetFile);
        }
        ASSERT_EQ(a_util::result::SUCCESS,oDDLPrinter.visitDDL(poDDL)) <<  "Print process failed.";
        ASSERT_EQ(a_util::result::SUCCESS,oDDLPrinter.toFile(strTargetFile)) << 
            "Export to file failed.";
        ASSERT_TRUE(a_util::filesystem::exists(strTargetFile));
        a_util::result::Result res = CompOutput(strTargetFile, "files/adtf_out_expected.xml", 9);
        a_util::filesystem::remove(strTargetFile);
        ASSERT_EQ(a_util::result::SUCCESS, res);
        // free allocated resources
        oDDLImporter.destroyDDL();
        poDDL = NULL;
    }
    {
        // Test setting unknown Version
        DDLPrinter oDDLPrinter;
        ASSERT_NE(a_util::result::SUCCESS, oDDLPrinter.forceVersion(DDLVersion(99, 99)));
    }
    {
        // Testing of setting explicitly version
        // 2.0 on 2.0 file.
        // 3.0 on 2.0 file.
        // 3.0 on 3.0 file.
        // 2.0 on 3.0 file.
        // This test does not check if the printer is able to print DDL 2.0 and DDL 3.0 in general
#define __TESTER_DDL_REPRESENTATION_TEST_AMOUNT 4
        DDLVersion versions[__TESTER_DDL_REPRESENTATION_TEST_AMOUNT] =
        {   ddl::DDLVersion::ddl_version_30,
            ddl::DDLVersion::ddl_version_40,
            ddl::DDLVersion::ddl_version_40,
            ddl::DDLVersion::ddl_version_30 };
        const char* strSourceFiles[__TESTER_DDL_REPRESENTATION_TEST_AMOUNT] =
        {"files/force_version_ddl-2.0.description",
        "files/force_version_ddl-2.0.description",
        "files/force_version_ddl-3.0.description",
        "files/force_version_ddl-3.0.description"};
        const char* strExpectedFiles[__TESTER_DDL_REPRESENTATION_TEST_AMOUNT] =
        {"files/force_version_ddl-2.0.description",
        "files/force_version_ddl-2.0_to_3.0_expected.description",
        "files/force_version_ddl-3.0.description",
        "files/force_version_ddl-2.0.description"};
        for (unsigned int nIdx = 0; nIdx < __TESTER_DDL_REPRESENTATION_TEST_AMOUNT; nIdx++)
        {
            std::string strResultFile = a_util::strings::format("%s_result.description", strExpectedFiles[nIdx]);
            if (a_util::filesystem::exists(strResultFile.c_str()))
            {
                a_util::filesystem::remove(strResultFile);
            }
            DDLImporter oDDLImporter;
            oDDLImporter.setFile(strSourceFiles[nIdx]);
            ASSERT_EQ(a_util::result::SUCCESS, oDDLImporter.createNew());
            DDLDescription* pDDL = oDDLImporter.getDDL();
            DDLPrinter oDDLPrinter;
            oDDLPrinter.forceVersion(versions[nIdx]);
            ASSERT_EQ(a_util::result::SUCCESS, oDDLPrinter.visitDDL(pDDL));
            ASSERT_EQ(a_util::result::SUCCESS, oDDLPrinter.toFile(strResultFile));
            ASSERT_TRUE(a_util::filesystem::exists(strResultFile));
            a_util::result::Result res = CompOutput(strResultFile, strExpectedFiles[nIdx], 0);
            a_util::filesystem::remove(strResultFile);
            ASSERT_EQ(a_util::result::SUCCESS, res);
        }
    }

}

/**
* @detail The merging of 2 DDL descriptions in object-representation is tested.
* Import and merge 2 existing DDL descriptions and print the result to an own file
* Repeat the test with the overwrite flag
*/
TEST(cTesterDDLRep,
    TestMerge)
{
    TEST_REQ("ACORE-3904");

    DDLImporter oDDLImporter("files/adtf1.description");
    ASSERT_EQ(a_util::result::SUCCESS, oDDLImporter.createNew());
    DDLDescription *poDDL1 = oDDLImporter.getDDL();
    ASSERT_EQ(a_util::result::SUCCESS, oDDLImporter.setFile("files/adtf2.xml"));
    ASSERT_EQ(a_util::result::SUCCESS, oDDLImporter.createNew());
    DDLDescription *poDDL2 = oDDLImporter.getDDL();
    ASSERT_EQ(a_util::result::SUCCESS,poDDL1->merge(*poDDL2)) <<  "Merge process failed.";
    std::string strTargetFile = "adtf_merged.xml";
    if (a_util::filesystem::exists(strTargetFile))
    {
        a_util::filesystem::remove(strTargetFile);
    }
    DDLPrinter oDDLPrinter;
    ASSERT_EQ(a_util::result::SUCCESS, oDDLPrinter.visitDDL(poDDL1));
    ASSERT_EQ(a_util::result::SUCCESS, oDDLPrinter.toFile(strTargetFile));
    ASSERT_TRUE(a_util::filesystem::exists(strTargetFile));
    a_util::result::Result res = CompOutput(strTargetFile, "files/adtf_merged_expected.xml", 9);
    ASSERT_EQ(a_util::result::SUCCESS, res);
    a_util::filesystem::remove(strTargetFile);
    // free allocated resources
    DDLImporter::destroyDDL(poDDL1);
    DDLImporter::destroyDDL(poDDL2);
    poDDL1 = NULL;
    poDDL2 = NULL;
    // merge with overwrite flag
    ASSERT_EQ(a_util::result::SUCCESS, oDDLImporter.setFile("files/adtf.description"));
    ASSERT_EQ(a_util::result::SUCCESS, oDDLImporter.createNew());
    poDDL1 = oDDLImporter.getDDL();
    ASSERT_EQ(a_util::result::SUCCESS, oDDLImporter.setFile("files/adtf2.xml"));
    ASSERT_EQ(a_util::result::SUCCESS, oDDLImporter.createNew());
    poDDL2 = oDDLImporter.getDDL();
    ASSERT_EQ(a_util::result::SUCCESS,poDDL1->merge(
        *poDDL2, IMediaDescriptionManager::MDMF_ForceOverwrite)) <<
        "Merge with overwrite flag failed.";
    strTargetFile = "adtf_merged_ovrwrt.xml";
    if (a_util::filesystem::exists(strTargetFile))
    {
        a_util::filesystem::remove(strTargetFile);
    }
    ASSERT_EQ(a_util::result::SUCCESS, oDDLPrinter.visitDDL(poDDL1));
    ASSERT_EQ(a_util::result::SUCCESS, oDDLPrinter.toFile(strTargetFile));
    ASSERT_TRUE(a_util::filesystem::exists(strTargetFile));
    res = CompOutput(strTargetFile, "files/adtf_merged_ovrwrt_expected.xml", 9);
    ASSERT_EQ(a_util::result::SUCCESS, res);
    a_util::filesystem::remove(strTargetFile);
    // free allocated resources
    DDLImporter::destroyDDL(poDDL1);
    DDLImporter::destroyDDL(poDDL2);

}

/**
* @detail The resolving of a specific target inside a DDL is tested.
* Import an existing DDL description, resolve the target,
* and print the result to an own file
*/
TEST(cTesterDDLRep,
    TestResolver)
{
    TEST_REQ("ACORE-3904");

    DDLImporter oDDLImporter("files/adtf.description");
    ASSERT_EQ(a_util::result::SUCCESS, oDDLImporter.createNew());
    DDLResolver oResolver;
    oResolver.setTargetName("extended_can_stream");
    ASSERT_EQ(a_util::result::SUCCESS,oResolver.visitDDL(oDDLImporter.getDDL())) << 
        "Could not resolve specified target.";
    a_util::xml::DOM oDOM;
    ASSERT_TRUE(oDOM.fromString(oResolver.getResolvedXML())) << 
        "Invalid XML string retrieved.";
    std::string const strTargetFile ("adtf_resolved.xml");
    if (a_util::filesystem::exists(strTargetFile))
    {
        a_util::filesystem::remove(strTargetFile);
    }
    ASSERT_TRUE(oDOM.save(strTargetFile)) << 
        "Could not save resolved XML to file.";
    ASSERT_TRUE(a_util::filesystem::exists(strTargetFile));
    a_util::result::Result res = CompOutput(strTargetFile, "files/adtf_resolved_expected.xml", 9);
    a_util::filesystem::remove(strTargetFile);
    ASSERT_EQ(a_util::result::SUCCESS, res);
    // free allocated resources
    oDDLImporter.destroyDDL();

}

/**
* @detail The renaming of streams and the removal of types and streams are tested.
* Import an existing DDL description, rename and remove some elements,
* and print the result to an own file
*/
TEST(cTesterDDLRep,
    TestElementHandling)
{
    TEST_REQ("ACORE-3904");

    DDLImporter oDDLImporter("files/adtf.description");
    ASSERT_EQ(a_util::result::SUCCESS, oDDLImporter.createNew());
    DDLDescription *poDDL = oDDLImporter.getDDL();
    ASSERT_TRUE(NULL !=poDDL);
    ASSERT_EQ(a_util::result::SUCCESS,poDDL->renameStream("video_stream", "new_video")) <<  "Could not rename stream.";
    ASSERT_EQ(a_util::result::SUCCESS,poDDL->removeStream("extended_can_stream")) <<  "Could not remove stream.";
    ASSERT_EQ(a_util::result::SUCCESS,poDDL->removeType("tCanMessageExt")) <<  "Could not remove struct.";
    ASSERT_EQ(a_util::result::SUCCESS,poDDL->removeType("tFloat32")) <<  "Could not remove primitive data type.";
    DDLPrinter oDDLPrinter;
    ASSERT_EQ(a_util::result::SUCCESS, oDDLPrinter.visitDDL(poDDL));
    std::string const strTargetFile ("adtf_changed.xml");
    if (a_util::filesystem::exists(strTargetFile))
    {
        a_util::filesystem::remove(strTargetFile);
    }
    ASSERT_EQ(a_util::result::SUCCESS, oDDLPrinter.toFile(strTargetFile));
    ASSERT_TRUE(a_util::filesystem::exists(strTargetFile));
    a_util::result::Result res = CompOutput(strTargetFile, "files/adtf_changed_expected.xml", 9);
    a_util::filesystem::remove(strTargetFile);
    ASSERT_EQ(a_util::result::SUCCESS, res);
    // free allocated resources
    DDLImporter::destroyDDL(poDDL);
    poDDL = NULL;

}

/**
* @detail The creation of a partial DDL is tested.
* Import an existing DDL description, create a partial DDL, try to merge them together
*/
TEST(cTesterDDLRep,
    TestCreatePartial)
{
    TEST_REQ("ACORE-3904 ACORE-6148");

    // import a wholly defined DDL
    DDLImporter oImporter("files/adtf.description");
    ASSERT_EQ(a_util::result::SUCCESS, oImporter.createNew());
    DDLDescription *poRefDDL = oImporter.getDDL();

    // try to create a partial DDL with an undefined type
    std::string strPartialDesc ("<stream name=\"foo\" type=\"bar\" description=\"Test fail\"/>");
    ASSERT_EQ(a_util::result::SUCCESS, oImporter.setXML(strPartialDesc));
    ASSERT_TRUE((ERR_NO_CLASS == oImporter.createPartial(poRefDDL, ddl::DDLVersion::ddl_version_invalid))) <<  "Erroneous partial DDL not detected.";
    oImporter.destroyDDL();
    strPartialDesc = std::string("<test_error_class/>");
    ASSERT_EQ(a_util::result::SUCCESS, oImporter.setXML(strPartialDesc));
    ASSERT_TRUE((ERR_UNKNOWN_FORMAT == oImporter.createPartial(poRefDDL, ddl::DDLVersion::ddl_version_invalid))) <<  "Invalid partial DDL not detected.";
    oImporter.destroyDDL();
    strPartialDesc = std::string("<stream name=\"can2\" type=\"adtf.core.media_type\" description=\"Another CAN\"><struct bytepos=\"0\" type=\"tCanMessage\"/></stream>");
    ASSERT_EQ(a_util::result::SUCCESS, oImporter.setXML(strPartialDesc));
    ASSERT_TRUE((ERR_POINTER == oImporter.createPartial(NULL, ddl::DDLVersion::ddl_version_invalid))) <<  "Null-pointer not detected.";
    ASSERT_EQ(a_util::result::SUCCESS,oImporter.createPartial(poRefDDL, ddl::DDLVersion::ddl_version_invalid)) <<  "Could not create partial DDL";
    DDLDescription *poPartDDL = oImporter.getDDL();
    ASSERT_TRUE( (NULL !=poPartDDL)) <<  "Invalid DDL object retrieved.";
    DDLPrinter oPrinter;
    ASSERT_EQ(a_util::result::SUCCESS, oPrinter.visitDDL(poPartDDL));
    std::string const strTargetFile ("partial.description");
    if (a_util::filesystem::exists(strTargetFile))
    {
        a_util::filesystem::remove(strTargetFile);
    }
    ASSERT_EQ(a_util::result::SUCCESS, oPrinter.toFile(strTargetFile));
    ASSERT_TRUE(a_util::filesystem::exists(strTargetFile));
    a_util::result::Result res = CompOutput(strTargetFile, "files/partial_expected.description", 9);
    a_util::filesystem::remove(strTargetFile);
    ASSERT_EQ(a_util::result::SUCCESS, res);

    // free allocated resources
    DDLImporter::destroyDDL(poPartDDL);
    DDLImporter::destroyDDL(poRefDDL);

}

/**
* @detail The get-by-name methods of the DDL are tested.
* Import an existing DDL description and try to get some specific elements out by name
*/
TEST(cTesterDDLRep,
    TestgetXByName)
{
    TEST_REQ("ACORE-3904");

    DDLImporter oImporter("files/adtf.description");
    ASSERT_EQ(a_util::result::SUCCESS, oImporter.createNew());
    DDLDescription *poDDL = oImporter.getDDL();
    ASSERT_TRUE(NULL != poDDL);
    // test exising items
    ASSERT_TRUE((NULL != poDDL->getBaseunitByName("nou"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getDataTypeByName("tFloat32"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getDataTypeByName("tInt64"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getStructByName("adtf.type.audio"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getStructByName("tMediaTypeInfo"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getStreamByName("video_stream"))) <<  "No object received";
    // test non-existing items
    ASSERT_TRUE((NULL == poDDL->getStructByName("adtf.type.flexray"))) <<  "Invalid pointer received";
    ASSERT_TRUE((NULL == poDDL->getUnitByName("audio_stream"))) <<  "Invalid pointer received";
    ASSERT_TRUE((NULL == poDDL->getPrefixByName("tBitmapInfo"))) <<  "Invalid pointer received";
    // free allocated resources
    oImporter.destroyDDL();

}

/**
* @detail The switch between different creation levels is tested.
* Create a small DDL at level 0, add elements at some other levels,
* and test the existence of some elements at specific levels
*/
TEST(cTesterDDLRep,
    TestrestoreLevel)
{
    TEST_REQ("ACORE-3904");

    // create DDL
    DDLImporter oImporter("files/partial_expected.description");
    oImporter.setCreationLevel(0);  // cMediaManager::DL_AlwaysThere
    ASSERT_EQ(a_util::result::SUCCESS,oImporter.createNew()) <<  oImporter.getErrorDesc().c_str();
    DDLDescription *poDDL = oImporter.getDDL();
    // add some elements with different creation levels
    // level: cMediaManager::DL_SystemGlobal
    poDDL->addDatatype(new DDLDataType(std::string("tInt128"), (unsigned int)128, NULL, std::string("Type for very large numbers"), DDLAlignment::e1, 1, 1));
    DDLPrefix * poPrefix = new DDLPrefix("One", "", 1, 1);
    poDDL->addPrefix(poPrefix);
    // level: cMediaManager::DL_Config
    DDLBaseunit * poBU = new DDLBaseunit("Bit", "b", "Smallest information unit", 2);
    poDDL->addBaseunit(poBU);
    DDLRefUnitVec vecRU;
    vecRU.push_back(new DDLRefUnit(poBU, 1, poPrefix));
    poDDL->addUnit(new DDLUnit("Byte", "8", "1", 0.0,  vecRU, 2));
    std::transform(vecRU.begin(), vecRU.end(), vecRU.begin(), DDLDescription::deleteChild<DDLRefUnit>);
    // level: cMediaManager::DL_Running
    poDDL->addDatatype(new DDLDataType("tFloat96", 96, NULL,
        "Floating-point type for high demands", DDLAlignment::e1, 1, 3));
    // no changes to DDL expected through this level switch
    ASSERT_EQ(a_util::result::SUCCESS,poDDL->restoreLevel(4)) <<  "Level switch failed";
    // test existence
    ASSERT_TRUE((NULL != poDDL->getDataTypeByName("tBool"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getDataTypeByName("tUInt16"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getStructByName("tCanMessage"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getDataTypeByName("tInt128"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getPrefixByName("One"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getBaseunitByName("Bit"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getUnitByName("Byte"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getDataTypeByName("tFloat96"))) <<  "No object received";
    // decrease the level to DL_Running and test existence again
    ASSERT_EQ(a_util::result::SUCCESS,poDDL->restoreLevel(3)) <<  "Level switch failed";
    ASSERT_TRUE((NULL != poDDL->getDataTypeByName("tBool"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getDataTypeByName("tUInt16"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getStructByName("tCanMessage"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getDataTypeByName("tInt128"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getPrefixByName("One"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getBaseunitByName("Bit"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getUnitByName("Byte"))) <<  "No object received";
    ASSERT_TRUE((NULL == poDDL->getDataTypeByName("tFloat96"))) <<  "Invalid pointer received";
    // decrease the level to DL_Config and test existence again
    ASSERT_EQ(a_util::result::SUCCESS,poDDL->restoreLevel(2)) <<  "Level switch failed";
    ASSERT_TRUE((NULL != poDDL->getDataTypeByName("tBool"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getDataTypeByName("tUInt16"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getStructByName("tCanMessage"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getDataTypeByName("tInt128"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getPrefixByName("One"))) <<  "No object received";
    ASSERT_TRUE((NULL == poDDL->getBaseunitByName("Bit"))) <<  "Invalid pointer received";
    ASSERT_TRUE((NULL == poDDL->getUnitByName("Byte"))) <<  "Invalid received";
    ASSERT_TRUE((NULL == poDDL->getDataTypeByName("tFloat96"))) <<  "Invalid pointer received";
    // decrease the level to DL_SystemGlobal and test existence again
    ASSERT_EQ(a_util::result::SUCCESS,poDDL->restoreLevel(1)) <<  "Level switch failed";
    ASSERT_TRUE((NULL != poDDL->getDataTypeByName("tBool"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getDataTypeByName("tUInt16"))) <<  "No object received";
    ASSERT_TRUE((NULL != poDDL->getStructByName("tCanMessage"))) <<  "No object received";
    ASSERT_TRUE((NULL == poDDL->getDataTypeByName("tInt128"))) <<  "Invalid pointer received";
    ASSERT_TRUE((NULL == poDDL->getPrefixByName("One"))) <<  "Invalid pointer received";
    ASSERT_TRUE((NULL == poDDL->getBaseunitByName("Bit"))) <<  "Invalid pointer received";
    ASSERT_TRUE((NULL == poDDL->getUnitByName("Byte"))) <<  "Invalid received";
    ASSERT_TRUE((NULL == poDDL->getDataTypeByName("tFloat96"))) <<  "Invalid pointer received";

    // free allocated resources
    DDLImporter::destroyDDL(poDDL);

}

/**
* @detail The compatibility to 'old' version 1.0+ gets tested.
* Read a DDL1.0+ from a file, test DDLPrinter and DDLResolver
*/
TEST(cTesterDDLRep,
    TestCompatibility)
{
    TEST_REQ("ACORE-3904");

    // test import
    DDLImporter oImporter("files/adtf_1_0p.description");
    ASSERT_EQ(a_util::result::SUCCESS,oImporter.createNew()) <<  "Import of DDL1.0+ failed.";
    DDLDescription *poDDL = oImporter.getDDL();
    ASSERT_TRUE( (NULL !=poDDL)) <<  "Invalid pointer retrieved";
    // test printing
    DDLPrinter oPrinter;
    ASSERT_EQ(a_util::result::SUCCESS,oPrinter.visitDDL(poDDL)) <<  "Printing of DDL1.0+ failed.";
    std::string strTargetFile ("adtf_1_0p_out.xml");
    if (a_util::filesystem::exists(strTargetFile))
    {
        a_util::filesystem::remove(strTargetFile);
    }
    ASSERT_EQ(a_util::result::SUCCESS,oPrinter.toFile(strTargetFile)) <<  "Export to file failed.";
    ASSERT_TRUE(a_util::filesystem::exists(strTargetFile));
    a_util::result::Result res = CompOutput(strTargetFile, "files/adtf_1_0p_out_expected.xml", 9);
    a_util::filesystem::remove(strTargetFile);
    ASSERT_EQ(a_util::result::SUCCESS, res);
    // test resolving
    DDLResolver oResolver("extended_can_stream");
    ASSERT_EQ(a_util::result::SUCCESS,oResolver.visitDDL(poDDL)) <<  "Resolving of DDL1.0+ failed.";
    a_util::xml::DOM oDOM;
    ASSERT_TRUE(oDOM.fromString(oResolver.getResolvedXML()));
    strTargetFile = std::string("adtf_1_0p_resolved.xml");
    if (a_util::filesystem::exists(strTargetFile))
    {
        a_util::filesystem::remove(strTargetFile);
    }
    ASSERT_TRUE(oDOM.save(strTargetFile)) << 
        "Export of resolved XML to file failed.";
    ASSERT_TRUE(a_util::filesystem::exists(strTargetFile));
    res = CompOutput(strTargetFile, "files/adtf_1_0p_resolved_expected.xml", 9);
    a_util::filesystem::remove(strTargetFile);
    ASSERT_EQ(a_util::result::SUCCESS, res);

    // free allocated resources
    DDLImporter::destroyDDL(poDDL);

}

/**
* @detail Check if the DDLImporter correctly detects an illegal recursion.
* Load a DDL and check if an error message is returned. Passed if the 
* DDLImporter announces the expected error messages and no program errors occur.
*/
TEST(cTesterDDLRep,
    TestRecursion)
{
    TEST_REQ("ACORE-5520");

    DDLImporter oImporter;

    ASSERT_EQ(a_util::result::SUCCESS, oImporter.setFile("files/adtf_recursion.description"));

    ASSERT_TRUE(a_util::result::isFailed(oImporter.createNew()));

    std::string strErrorDesc = oImporter.getErrorDesc();

    std::cout << strErrorDesc.c_str();
}

/**
* @detail Test if the support for minimum, maximum, and default values works correctly.
* A DDL is loaded and the contained minimum, maximum and default values are validated. 
* Passed if the validator announces the expected error messages and no program errors occur.
*/
TEST(cTesterDDLRep,
    TestMinMaxDefaultValues)
{
    TEST_REQ("ACORE-5212");

    DDLImporter oImporter;

    ASSERT_EQ(a_util::result::SUCCESS,oImporter.setFile("files/adtf_min_max_default.description")) << 
        "Could not load DDL file with min, max, and default values";
    ASSERT_EQ(a_util::result::SUCCESS, oImporter.createNew());

    DDLDescription *poDDL = oImporter.getDDL();
    ASSERT_TRUE(NULL !=poDDL);

    DDLInspector oInspector;
    ASSERT_TRUE((a_util::result::SUCCESS != oInspector.visitDDL(poDDL))) << 
        "Validator did not detect erroneous default value.";

    auto suggestions = oInspector.getSuggestions();
    // Element (Complex_Datatypes Element Name) 'SomeStructU16WithBoolLimits.ui16Value1' maximum value is 'tTrue' but it should be a floating point value or tTrue if using type tBool.
    // Element (Complex_Datatypes Element Name) 'SomeStructU16WithBoolLimits.ui16Value1' minimum value is 'tFalse' but it should be a floating point value or tFalse if using type tBool.
    // Default value of element 'i16TooLow' is out of range.
    // Default value of element 'tMinMaxDerived' is out of range.
    ASSERT_EQ(suggestions.size(), 4);

    // tidy up
    DDLImporter::destroyDDL(poDDL);
    poDDL = NULL;


}

class cWorker
{
    volatile bool exit;
    a_util::concurrency::thread thread;
    void LoopFunc()
    {
        while (!exit)
        {
            DDLImporter oDDLImporter;
            std::string strXML (DDL_TEST_STRING);
            ASSERT_EQ(a_util::result::SUCCESS,oDDLImporter.setXML(strXML)) <<  "Could not set XML string.";
            ASSERT_EQ(a_util::result::SUCCESS,oDDLImporter.createNew()) <<  "Creation of DDL failed.";
            ASSERT_TRUE( (NULL !=oDDLImporter.getDDL())) <<  "Invalid DDL object retrieved.";

            DDLPrinter oDDLPrinter;
            ASSERT_EQ(a_util::result::SUCCESS,oDDLPrinter.visitDDL(oDDLImporter.getDDL())) <<  "Print process failed.";
            std::string strXMLRes = oDDLPrinter.getXML();
            ASSERT_FALSE(strXMLRes.empty());

            // free allocated resources
            oDDLImporter.destroyDDL();
        }
    }

public:
    cWorker() : exit(false), thread(&cWorker::LoopFunc, this)
    {
    }

    ~cWorker()
    {
        exit = true;
        thread.join();
    }
};

#define THREAD_COUNT 30

/**
* @detail Check if DDL objects are reenrant by creating multiple threads that perform OODDL operations
* Every thread should work without interfering the others.
*/
TEST(cTesterDDLRep,
    TestReentrant)
{
    TEST_REQ("ACORE-6945");

    {
        cWorker workers[THREAD_COUNT];
        a_util::system::sleepMilliseconds(1000);
    }
}

/**
* @detail Test DDLCloner by cloning DDL and compare if equal.
*/
TEST(cTesterDDLRep,
    TestCloner)
{
    TEST_REQ("ACORE-3904");

    // test partial checker (good case)
    DDLImporter oImporter ("files/adtf.description");
    ASSERT_EQ(a_util::result::SUCCESS, oImporter.createNew());
    DDLDescription *poOrgDDL = oImporter.getDDL();
    ASSERT_TRUE(NULL !=poOrgDDL);

    DDLCloner oCloner;
    ASSERT_EQ(a_util::result::SUCCESS, oCloner.setOriginal(poOrgDDL));
    ASSERT_EQ(a_util::result::SUCCESS, oCloner.createNew());

    DDLDescription *pClonedDDL = oCloner.getDDL();
    ASSERT_TRUE(NULL !=pClonedDDL);

    ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(poOrgDDL, pClonedDDL, DDLCompare::dcf_everything));

    // reset original DDL
    oCloner.destroyDDL();
    oImporter.destroyDDL();
    
    oImporter.setFile("files/fep_driver.description");
    ASSERT_EQ(a_util::result::SUCCESS, oImporter.createNew());
    poOrgDDL = oImporter.getDDL();

    ASSERT_EQ(a_util::result::SUCCESS, oCloner.setOriginal(poOrgDDL));
    ASSERT_EQ(a_util::result::SUCCESS, oCloner.createNew());

    pClonedDDL = oCloner.getDDL();
    ASSERT_TRUE(NULL != pClonedDDL);

    ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(poOrgDDL, pClonedDDL, DDLCompare::dcf_everything));

}

#define LOG_AND_RELEASE_EXCEPTION(__ex) LOG_EXCEPTION(__ex); __ex = NULL;

/**
* @detail Test DDLCompare by cloning DDL and compare several DDL description.
*/
TEST(cTesterDDLRep,
    TestCompare)
{
    TEST_REQ("ACORE-7093");

    // isBinaryEqual
    {
        ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isBinaryEqual("adtf.type.video", DDL_TEST_STRING, "adtf.type.video", DDL_TEST_STRING, true));
        ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isBinaryEqual("adtf.type.video", DDL_TEST_STRING, "adtf.type.video", DDL_TEST_STRING, false));

        ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isBinaryEqual("adtf.type.video", DDL_TEST_STRING, "adtf.type.video", DDL_TEST_STRING_DIFF, true));
        ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isBinaryEqual("adtf.type.video", DDL_TEST_STRING, "adtf.type.video", DDL_TEST_STRING_DIFF, false));

        ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isBinaryEqual("adtf.type.video", DDL_TEST_STRING_SUBSET, "adtf.type.video", DDL_TEST_STRING, true));
        ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isBinaryEqual("adtf.type.video", DDL_TEST_STRING, "adtf.type.video", DDL_TEST_STRING_SUBSET, true));
        ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isBinaryEqual("adtf.type.video", DDL_TEST_STRING_SUBSET, "adtf.type.video", DDL_TEST_STRING, false));

        ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isBinaryEqual("adtf.type.video", DDL_TEST_STRING_DYNAMIC, "adtf.type.video", DDL_TEST_STRING_DYNAMIC, false));
        ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isBinaryEqual("adtf.type.video", DDL_TEST_STRING_DYNAMIC, "adtf.type.video", DDL_TEST_STRING_DYNAMIC_NAME_DIFF, false));
        ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isBinaryEqual("adtf.type.video", DDL_TEST_STRING_DYNAMIC, "adtf.type.video", DDL_TEST_STRING_DYNAMIC_BIN_DIFF, false));
        ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isBinaryEqual("adtf.type.video", DDL_TEST_STRING_DYNAMIC, "adtf.type.video", DDL_TEST_STRING_DYNAMIC_BIN_DIFF_2, false));

        ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isBinaryEqual("tTest1", DDL_TEST_STRING_LAYOUT_TEST, "tTest2", DDL_TEST_STRING_LAYOUT_TEST, false));
    }

    // isEqual (types)
    {
        ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqualType("adtf.type.video", DDL_TEST_STRING, "adtf.type.video", DDL_TEST_STRING,
            DDLCompare::icf_memory|DDLCompare::icf_serialized|DDLCompare::icf_names|DDLCompare::icf_versions));
        ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqualType("adtf.type.video", DDL_TEST_STRING, "adtf.type.video", DDL_TEST_STRING_DIFF,
            DDLCompare::icf_memory));

        ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqualType("adtf.type.video", DDL_TEST_STRING, "adtf.type.video", DDL_TEST_STRING_DIFF_NAMES,
            DDLCompare::icf_memory|DDLCompare::icf_names));
        ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqualType("adtf.type.video", DDL_TEST_STRING, "adtf.type.video", DDL_TEST_STRING_DIFF_NAMES,
            DDLCompare::icf_memory));

        ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqualType("tTest1", DDL_TEST_STRING_LAYOUT_TEST, "tTest2", DDL_TEST_STRING_LAYOUT_TEST,
            DDLCompare::icf_memory));

        ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqualType("adtf.type.video", DDL_TEST_STRING, "adtf.type.video", DDL_TEST_STRING_DIFF_VERSION,
            DDLCompare::icf_memory|DDLCompare::icf_versions));
        ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqualType("adtf.type.video", DDL_TEST_STRING, "adtf.type.video", DDL_TEST_STRING_DIFF_VERSION,
            DDLCompare::icf_memory));

        ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqualType("adtf.type.video", DDL_TEST_STRING, "adtf.type.video", DDL_TEST_STRING_DIFF_ARRAY,
            DDLCompare::icf_memory));

        // dynamic stuff
        ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqualType("adtf.type.video", DDL_TEST_STRING_DYNAMIC, "adtf.type.video", DDL_TEST_STRING_DYNAMIC, DDLCompare::icf_memory));
        ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqualType("adtf.type.video", DDL_TEST_STRING_DYNAMIC, "adtf.type.video", DDL_TEST_STRING_DYNAMIC_NAME_DIFF, DDLCompare::icf_memory));
        ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqualType("adtf.type.video", DDL_TEST_STRING_DYNAMIC, "adtf.type.video", DDL_TEST_STRING_DYNAMIC_NAME_DIFF, DDLCompare::icf_memory|DDLCompare::icf_names));

        ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqualType("adtf.type.video", DDL_TEST_STRING_DYNAMIC, "adtf.type.video", DDL_TEST_STRING_DYNAMIC_BIN_DIFF, DDLCompare::icf_memory));

        ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqualType("adtf.type.video", DDL_TEST_STRING_DYNAMIC, "adtf.type.video", DDL_TEST_STRING_DYNAMIC_BIN_DIFF_2, DDLCompare::icf_memory));

        ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqualType("tTest1", DDL_TEST_STRING_DYNAMIC_DIFF_ARRAY_SPEC, "tTest1", DDL_TEST_STRING_DYNAMIC_DIFF_ARRAY_SPEC, DDLCompare::icf_memory));
        ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqualType("tTest1", DDL_TEST_STRING_DYNAMIC_DIFF_ARRAY_SPEC, "tTest2", DDL_TEST_STRING_DYNAMIC_DIFF_ARRAY_SPEC, DDLCompare::icf_memory));

        // subset
        ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqualType("adtf.type.video", DDL_TEST_STRING_SUBSET, "adtf.type.video", DDL_TEST_STRING, DDLCompare::icf_memory|DDLCompare::icf_subset));
        ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqualType("adtf.type.video", DDL_TEST_STRING, "adtf.type.video", DDL_TEST_STRING_SUBSET, DDLCompare::icf_memory|DDLCompare::icf_subset));
        ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqualType("adtf.type.video", DDL_TEST_STRING_INVALID_SUBSET, "adtf.type.video", DDL_TEST_STRING, DDLCompare::icf_memory|DDLCompare::icf_subset));

    }

    // isEqual (descriptions)
    {
        ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, DDL_TEST_STRING,
            DDLCompare::dcf_everything));

        // header
        {
            a_util::xml::DOM oDom;
            ASSERT_TRUE(oDom.fromString(DDL_TEST_STRING_HEADER));
            a_util::xml::DOMElement oNode;
            ASSERT_TRUE(oDom.findNode("//header/language_version", oNode));
            oNode.setData("123");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_HEADER, oDom.toString().c_str(),
                DDLCompare::dcf_header | DDLCompare::dcf_descriptions | DDLCompare::dcf_versions));
            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_HEADER, oDom.toString().c_str(),
                DDLCompare::dcf_header | DDLCompare::dcf_descriptions));

            ASSERT_TRUE(oDom.findNode("//header/description", oNode));
            oNode.setData("blah blah");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_HEADER, oDom.toString().c_str(),
                DDLCompare::dcf_header | DDLCompare::dcf_descriptions));
            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_HEADER, oDom.toString().c_str(),
                DDLCompare::dcf_header));

            ASSERT_TRUE(oDom.findNode("//header/date_creation", oNode));
            std::string strOldData = oNode.getData().c_str();
            oNode.setData("20140213");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_HEADER, oDom.toString().c_str(),
                DDLCompare::dcf_header));
            oNode.setData(strOldData);

            ASSERT_TRUE(oDom.findNode("//header/ext_declaration[@key='AnyKey']", oNode));
            oNode.setAttribute("value", "haha");

            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_HEADER, oDom.toString().c_str(),
                DDLCompare::dcf_header));

            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_HEADER, oDom.toString().c_str(),
                DDLCompare::dcf_header));

            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_HEADER, oDom.toString().c_str(),
                DDLCompare::dcf_all & ~DDLCompare::dcf_header));
        }

        // datatypes
        {
            a_util::xml::DOM oDom;
            ASSERT_TRUE(oDom.fromString(DDL_TEST_STRING));
            a_util::xml::DOMElement oNode;
            ASSERT_TRUE(oDom.findNode("//datatype[@name='tFloat64']", oNode));
            oNode.setAttribute("description", "foo");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_data_types | DDLCompare::dcf_descriptions));
            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_data_types));

            oNode.setAttribute("min", "2000");
            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_data_types));

            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_data_types | DDLCompare::dcf_visualization_attributes));

            oNode.setAttribute("size", "16");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_data_types));

            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_data_types));

            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_all & ~DDLCompare::dcf_data_types));
        }

        // structs
        {
            a_util::xml::DOM oDom;
            ASSERT_TRUE(oDom.fromString(DDL_TEST_STRING));
            a_util::xml::DOMElement oNode;
            ASSERT_TRUE(oDom.findNode("//struct[@name='tBitmapFormat']", oNode));
            oNode.setAttribute("comment", "foo");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_structs | DDLCompare::dcf_comments | DDLCompare::dcf_versions));
            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_structs | DDLCompare::dcf_versions));

            oNode.setAttribute("version", "123");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_structs | DDLCompare::dcf_versions));
            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_structs));

            std::string strOldAlignment = oNode.getAttribute("alignment").c_str();
            oNode.setAttribute("alignment", "16");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_structs));

            oNode.setAttribute("alignment", strOldAlignment);

            ASSERT_TRUE(oNode.findNode(".//element[@name='nPixelFormat']", oNode));

            oNode.setAttribute("scale", "1000");
            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_structs));
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_structs | DDLCompare::dcf_visualization_attributes));

            oNode.setAttribute("name", "sepp");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_structs));

            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_structs | DDLCompare::dcf_no_recursion));

            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_all & ~DDLCompare::dcf_structs & ~DDLCompare::dcf_streams));
        }

        // streams
        {
            a_util::xml::DOM oDom;
            ASSERT_TRUE(oDom.fromString(DDL_TEST_STRING));
            a_util::xml::DOMElement oNode;
            ASSERT_TRUE(oDom.findNode("//stream[@name='can_stream']", oNode));
            oNode.setAttribute("description", "foo");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_streams | DDLCompare::dcf_descriptions));
            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_streams));

            std::string strOldType = oNode.getAttribute("type").c_str();
            oNode.setAttribute("type", "tCanMessageExt");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_streams));
            oNode.setAttribute("type", strOldType);

            a_util::xml::DOMElement oStructNode;
            ASSERT_TRUE(oNode.findNode(".//struct[@type='tCanMessage']", oStructNode));

            oStructNode.setAttribute("bytepos", "500");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_streams));

            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_streams | DDLCompare::dcf_no_recursion));

            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_streams));

            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_all & ~DDLCompare::dcf_streams));

            // check differences in type as well
            ASSERT_TRUE(oDom.fromString(DDL_TEST_STRING));
            ASSERT_TRUE(oDom.findNode("//struct[@name='adtf.core.media_type']", oNode));
            oNode.setAttribute("alignment", "8");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING, oDom.toString().c_str(), DDLCompare::dcf_streams));
        }

        // base units
        {
            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_UNITS, DDL_TEST_STRING_UNITS, DDLCompare::dcf_all));

            a_util::xml::DOM oDom;
            ASSERT_TRUE(oDom.fromString(DDL_TEST_STRING_UNITS));
            a_util::xml::DOMElement oNode;
            ASSERT_TRUE(oDom.findNode("//baseunit[@name='Candela']", oNode));
            oNode.setAttribute("description", "foo");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_UNITS, oDom.toString().c_str(), DDLCompare::dcf_base_units | DDLCompare::dcf_descriptions));
            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_UNITS, oDom.toString().c_str(), DDLCompare::dcf_base_units));

            oNode.setAttribute("name", "foo_unit");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_UNITS, oDom.toString().c_str(), DDLCompare::dcf_base_units));

            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_UNITS, oDom.toString().c_str(), DDLCompare::dcf_base_units));

            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_UNITS, oDom.toString().c_str(),  DDLCompare::dcf_all & ~DDLCompare::dcf_base_units));
        }

        // prefix
        {
            a_util::xml::DOM oDom;
            ASSERT_TRUE(oDom.fromString(DDL_TEST_STRING_UNITS));
            a_util::xml::DOMElement oNode;
            ASSERT_TRUE(oDom.findNode("//prefixes[@name='deca']", oNode));

            std::string strOldName = oNode.getAttribute("name").c_str();
            oNode.setAttribute("name", "deca2");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_UNITS, oDom.toString().c_str(), DDLCompare::dcf_prefixes));
            oNode.setAttribute("name", strOldName);

            std::string strOldSymbol = oNode.getAttribute("symbol").c_str();
            oNode.setAttribute("symbol", "XF");

            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_UNITS, oDom.toString().c_str(), DDLCompare::dcf_prefixes));
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_UNITS, oDom.toString().c_str(), DDLCompare::dcf_units)); //a unit that uses the prefix
            oNode.setAttribute("symbol", strOldSymbol);

            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_UNITS, oDom.toString().c_str(), DDLCompare::dcf_all));
        }

        // units
        {
            a_util::xml::DOM oDom;
            ASSERT_TRUE(oDom.fromString(DDL_TEST_STRING_UNITS));
            a_util::xml::DOMElement oNode;
            ASSERT_TRUE(oDom.findNode("//unit[@name='bar_unit2']", oNode));

            std::string strOldName = oNode.getAttribute("name").c_str();
            oNode.setAttribute("name", "foo_unit");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_UNITS, oDom.toString().c_str(), DDLCompare::dcf_units));
            oNode.setAttribute("name", strOldName);

            a_util::xml::DOMElement oNumerator;
            ASSERT_TRUE(oNode.findNode(".//numerator", oNumerator));

            std::string strOldNumerator = oNumerator.getData().c_str();
            oNumerator.setData("200");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_UNITS, oDom.toString().c_str(), DDLCompare::dcf_units));
            oNumerator.setData(strOldNumerator);

            a_util::xml::DOMElement oRef;
            ASSERT_TRUE(oNode.findNode(".//refUnit", oRef));
            oRef.setAttribute("power", "100");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_UNITS, oDom.toString().c_str(), DDLCompare::dcf_units));

            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_UNITS, oDom.toString().c_str(), DDLCompare::dcf_units | DDLCompare::dcf_no_recursion));

            oRef.setAttribute("name", "Kelvin");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_UNITS, oDom.toString().c_str(), DDLCompare::dcf_units));

            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_UNITS, oDom.toString().c_str(), DDLCompare::dcf_units | DDLCompare::dcf_no_recursion));

            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_UNITS, oDom.toString().c_str(), DDLCompare::dcf_units));

            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_UNITS, oDom.toString().c_str(), DDLCompare::dcf_all & ~DDLCompare::dcf_units));
        }

        // enums
        {
            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_ENUMS, DDL_TEST_STRING_ENUMS, DDLCompare::dcf_all));

            a_util::xml::DOM oDom;
            ASSERT_TRUE(oDom.fromString(DDL_TEST_STRING_ENUMS));
            a_util::xml::DOMElement oNode;
            ASSERT_TRUE(oDom.findNode("//enum[@name='tValueDefinitions2']", oNode));

            std::string strOldType = oNode.getAttribute("type").c_str();
            oNode.setAttribute("type", "tFloat64");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_ENUMS, oDom.toString().c_str(), DDLCompare::dcf_enums));
            oNode.setAttribute("type", strOldType);

            a_util::xml::DOMElement oElement;
            ASSERT_TRUE(oNode.findNode(".//element[@name='ELEMENT_TWO2']", oElement));
            oElement.setAttribute("value", "321");

            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_ENUMS, oDom.toString().c_str(), DDLCompare::dcf_enums));

            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_ENUMS, oDom.toString().c_str(), DDLCompare::dcf_enums | DDLCompare::dcf_no_enum_values_check));
            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_ENUMS, oDom.toString().c_str(), DDLCompare::dcf_enums | DDLCompare::dcf_no_enum_values_check));
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_ENUMS, oDom.toString().c_str(), DDLCompare::dcf_enums));

            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(DDL_TEST_STRING_ENUMS, oDom.toString().c_str(), DDLCompare::dcf_all & ~DDLCompare::dcf_enums));
        }
    }

    {
        //Item based string methods

        {
            // enums
            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqualType("tValueDefinitions2", DDL_TEST_STRING_ENUMS, "tValueDefinitions2", DDL_TEST_STRING_ENUMS));

            a_util::xml::DOM oDom;
            ASSERT_TRUE(oDom.fromString(DDL_TEST_STRING_ENUMS));
            a_util::xml::DOMElement oNode;
            ASSERT_TRUE(oDom.findNode("//enum[@name='tValueDefinitions2']", oNode));
            oNode.setAttribute("type", "tBool");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqualType("tValueDefinitions2", DDL_TEST_STRING_ENUMS, "tValueDefinitions2", oDom.toString().c_str(), DDLCompare::icf_none));
        }

        {
            // prefixes
            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqualPrefix("deca", DDL_TEST_STRING_UNITS, "deca", DDL_TEST_STRING_UNITS));

            a_util::xml::DOM oDom;
            ASSERT_TRUE(oDom.fromString(DDL_TEST_STRING_UNITS));
            a_util::xml::DOMElement oNode;
            ASSERT_TRUE(oDom.findNode("//prefixes[@name='deca']", oNode));
            oNode.setAttribute("symbol", "XF");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqualPrefix("deca", DDL_TEST_STRING_UNITS, "deca", oDom.toString().c_str(), DDLCompare::icf_none));
        }

        {
            // units
            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqualUnit("bar_unit2", DDL_TEST_STRING_UNITS, "bar_unit2", DDL_TEST_STRING_UNITS));

            a_util::xml::DOM oDom;
            ASSERT_TRUE(oDom.fromString(DDL_TEST_STRING_UNITS));
            a_util::xml::DOMElement oNode;
            ASSERT_TRUE(oDom.findNode("//unit[@name='bar_unit2']", oNode));
            a_util::xml::DOMElement oNumerator;
            ASSERT_TRUE(oNode.findNode(".//numerator", oNumerator));
            oNumerator.setData("200");

            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqualUnit("bar_unit2", DDL_TEST_STRING_UNITS, "bar_unit2", oDom.toString().c_str(), DDLCompare::icf_none));
        }

        {
            // structs
            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqualType("tBitmapFormat", DDL_TEST_STRING, "tBitmapFormat", DDL_TEST_STRING));

            a_util::xml::DOM oDom;
            ASSERT_TRUE(oDom.fromString(DDL_TEST_STRING));
            a_util::xml::DOMElement oNode;
            ASSERT_TRUE(oDom.findNode("//struct[@name='tBitmapFormat']", oNode));
            oNode.setAttribute("alignment", "2");

            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqualType("tBitmapFormat", DDL_TEST_STRING, "tBitmapFormat", oDom.toString().c_str(), DDLCompare::icf_memory));
        }

        {
            // datatypes
            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqualType("tUInt8", DDL_TEST_STRING, "tUInt8", DDL_TEST_STRING));

            a_util::xml::DOM oDom;
            ASSERT_TRUE(oDom.fromString(DDL_TEST_STRING));
            a_util::xml::DOMElement oNode;
            ASSERT_TRUE(oDom.findNode("//datatype[@name='tUInt8']", oNode));
            oNode.setAttribute("size", "9");

            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqualType("tUInt8", DDL_TEST_STRING, "tUInt8", oDom.toString().c_str(), DDLCompare::icf_memory));

            oNode.setAttribute("size", "8");
            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqualType("tUInt8", DDL_TEST_STRING, "tUInt8", oDom.toString().c_str(), DDLCompare::icf_memory));

            oNode.setAttribute("max", "10"); //check attribute set only on one of the two
            std::string strHelper = oDom.toString().c_str();
            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqualType("tUInt8", DDL_TEST_STRING, "tUInt8", strHelper, DDLCompare::icf_names | DDLCompare::icf_memory));
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqualType("tUInt8", DDL_TEST_STRING, "tUInt8", strHelper, DDLCompare::icf_names | DDLCompare::icf_visualizations_attributes));

            // check set on both but different
            oNode.setAttribute("max", "11");
            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqualType("tUInt8", oDom.toString().c_str(), "tUInt8", strHelper, DDLCompare::icf_names | DDLCompare::icf_memory));
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqualType("tUInt8", oDom.toString().c_str(), "tUInt8", strHelper, DDLCompare::icf_names | DDLCompare::icf_visualizations_attributes));
        }

        {
            // streams
            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqualStream("video_stream", DDL_TEST_STRING, "video_stream", DDL_TEST_STRING));

            a_util::xml::DOM oDom;
            ASSERT_TRUE(oDom.fromString(DDL_TEST_STRING));
            a_util::xml::DOMElement oNode;
            ASSERT_TRUE(oDom.findNode("//stream[@name='video_stream']", oNode));
            oNode.setAttribute("type", "adtf.type.audio");

            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqualStream("video_stream", DDL_TEST_STRING, "video_stream", oDom.toString().c_str(), DDLCompare::icf_memory));

            // check recursion from stream to struct to unit to prefix.
            ASSERT_TRUE(oDom.fromString(DDL_TEST_STRING_STREAM_TO_PREFIX));
            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqualStream("a", DDL_TEST_STRING_STREAM_TO_PREFIX, "a", oDom.toString().c_str(), DDLCompare::icf_units));
            ASSERT_TRUE(oDom.findNode("//prefixes[@name='TestPrefix']", oNode));
            oNode.setAttribute("symbol", "p2");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqualStream("a", DDL_TEST_STRING_STREAM_TO_PREFIX, "a", oDom.toString().c_str(), DDLCompare::icf_units));

            // check recursion from stream to struct to unit to baseunit.
            ASSERT_TRUE(oDom.fromString(DDL_TEST_STRING_STREAM_TO_PREFIX));
            ASSERT_TRUE(oDom.findNode("//baseunit[@name='nou']", oNode));
            oNode.setAttribute("symbol", "1");
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqualStream("a", DDL_TEST_STRING_STREAM_TO_PREFIX, "a", oDom.toString().c_str(), DDLCompare::icf_units));
        }

        {
            DDLImporter oImporter1;
            ASSERT_EQ(a_util::result::SUCCESS, oImporter1.setXML(DDL_TEST_STRING_UNITS));
            ASSERT_EQ(a_util::result::SUCCESS, oImporter1.createNew());

            DDLRefUnit* pRefUnit1 = oImporter1.getDDL()->getUnitByName("bar_unit")->getRefUnits().front();
            ASSERT_EQ(a_util::result::SUCCESS, DDLCompare::isEqual(pRefUnit1, pRefUnit1));

            a_util::xml::DOM oDom;
            ASSERT_TRUE(oDom.fromString(DDL_TEST_STRING_UNITS));
            a_util::xml::DOMElement oNode;
            ASSERT_TRUE(oDom.findNode("//refUnit", oNode));
            oNode.setAttribute("prefix", "giga");

            DDLImporter oImporter2;
            ASSERT_EQ(a_util::result::SUCCESS, oImporter2.setXML(oDom.toString().c_str()));
            ASSERT_EQ(a_util::result::SUCCESS, oImporter2.createNew());

            DDLRefUnit* pRefUnit2 = oImporter2.getDDL()->getUnitByName("bar_unit")->getRefUnits().front();
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(pRefUnit1, pRefUnit2, DDLCompare::icf_none));
            ASSERT_NE(a_util::result::SUCCESS, DDLCompare::isEqual(pRefUnit1, pRefUnit2, DDLCompare::icf_no_recursion | DDLCompare::icf_names));
        }
    }
}

/**
* @detail Test use defined base units by importing a ddl with user defined base units.
*/
TEST(cTesterDDLRep,
    TestUserBaseUnit)
{
    TEST_REQ("ACORE-7149");

    {
        DDLImporter oImporter;
        oImporter.setMergeDefaults(false);
        ASSERT_EQ(a_util::result::SUCCESS, oImporter.setXML(DDL_TEST_STRING_USER_BASE_UNITS));
        ASSERT_EQ(a_util::result::SUCCESS, oImporter.createNew());
        ASSERT_TRUE(oImporter.getDDL()->getStructByName("test_struct"));
    }

    {
        DDLImporter oImporter;
        oImporter.setMergeDefaults(true);
        ASSERT_EQ(a_util::result::SUCCESS, oImporter.setXML(DDL_TEST_STRING_USER_BASE_UNITS));
        ASSERT_EQ(a_util::result::SUCCESS, oImporter.createNew());
        ASSERT_TRUE(oImporter.getDDL()->getStructByName("test_struct"));
        auto uint8_item = oImporter.getDDL()->getDataTypeByName("tUInt8");
        ASSERT_TRUE(uint8_item->getMaxValue() == "255");
        ASSERT_TRUE(uint8_item->getMinValue() == "0");
    }

    {
        DDLImporter oImporter;
        oImporter.setMergeDefaults(false);
        ASSERT_EQ(a_util::result::SUCCESS, oImporter.setXML(DDL_TEST_STRING_USER_BASE_UNITS));
        ASSERT_EQ(a_util::result::SUCCESS, oImporter.createPartial(DDLDescription::createDefault(), ddl::DDLVersion::ddl_version_invalid));
        ASSERT_TRUE(oImporter.getDDL()->getStructByName("test_struct"));
    }

    {
        DDLImporter oImporter;
        oImporter.setMergeDefaults(true);
        ASSERT_EQ(a_util::result::SUCCESS, oImporter.setXML(DDL_TEST_STRING_USER_BASE_UNITS));
        ASSERT_EQ(a_util::result::SUCCESS, oImporter.createPartial(DDLDescription::createDefault(), ddl::DDLVersion::ddl_version_invalid));
        ASSERT_TRUE(oImporter.getDDL()->getStructByName("test_struct"));
    }
}

void InsertHeaderExtDeclaration(DDLDescription* poDDL)
{
    {
        // insert at the third position
        DDLExtDeclaration* pExtDcl = new DDLExtDeclaration("c", "c");
        poDDL->getHeader()->addExtDeclaration(pExtDcl, 2);
    }
    {
        // insert at the end
        DDLExtDeclaration* pExtDcl = new DDLExtDeclaration("c2", "c2");
        poDDL->getHeader()->addExtDeclaration(pExtDcl, (int)poDDL->getHeader()->getExtDeclarations().size());
    }
    {
        // insert at the beginning
        DDLExtDeclaration* pExtDcl = new DDLExtDeclaration("c3", "c3");
        poDDL->getHeader()->addExtDeclaration(pExtDcl, 0);
    }
}

void InsertBaseUnit(DDLDescription* poDDL)
{
    {
        // insert at the third position
        DDLBaseunit* poBaseunit = new DDLBaseunit("c", "c", "c");
        poDDL->addBaseunit(poBaseunit, 2);
    }
    {
        // insert at the end
        DDLBaseunit* poBaseunit = new DDLBaseunit("c2", "c2", "c2");
        poDDL->addBaseunit(poBaseunit, poDDL->getBaseunits().size());
    }
    {
        // insert at the beginning
        DDLBaseunit* poBaseunit = new DDLBaseunit("c3", "c3", "c3");
        poDDL->addBaseunit(poBaseunit, 0);
    }
}

void InsertRefUnit(DDLDescription* poDDL)
{
    DDLUnit* poUnit = poDDL->getUnitByName("a");
    DDLPrefix* poPrefix = poDDL->getPrefixByName("a");

    {
        // insert at the third position
        DDLBaseunit* poBaseUnit = poDDL->getBaseunitByName("c");
        DDLRefUnit* poRefUnit = new DDLRefUnit(poBaseUnit, 1, poPrefix);
        poUnit->addRefUnit(poRefUnit, 2);
    }
    {
        // insert at the end
        DDLBaseunit* poBaseUnit = poDDL->getBaseunitByName("c2");
        DDLRefUnit* poRefUnit = new DDLRefUnit(poBaseUnit, 1, poPrefix);
        poUnit->addRefUnit(poRefUnit, (int)poUnit->getRefUnits().size());
    }
    {
        // insert at the beginning
        DDLBaseunit* poBaseUnit = poDDL->getBaseunitByName("c3");
        DDLRefUnit* poRefUnit = new DDLRefUnit(poBaseUnit, 1, poPrefix);
        poUnit->addRefUnit(poRefUnit, 0);
    }
}

void InsertUnit(DDLDescription* poDDL)
{
    {
        // insert at the third position
        DDLUnit* poUnit = new DDLUnit(*poDDL->getUnitByName("a"));
        poUnit->setName("c");
        poDDL->addUnit(poUnit, 2);
    }
    {
        // insert at the end
        DDLUnit* poUnit = new DDLUnit(*poDDL->getUnitByName("a"));
        poUnit->setName("c2");
        poDDL->addUnit(poUnit, poDDL->getUnits().size());
    }
    {
        // insert at the beginning
        DDLUnit* poUnit = new DDLUnit(*poDDL->getUnitByName("a"));
        poUnit->setName("c3");
        poDDL->addUnit(poUnit, 0);
    }
}

void InserStructElement(const std::string& strComplexDT,
    const std::string& strElementDT,
    DDLDescription* poDDL)
{
    DDLComplex* pComplexDT = poDDL->getStructByName(strComplexDT);

    IDDLDataType* pElementDT = poDDL->getStructByName(strElementDT);
    if (!pElementDT)
    {
        DDLDataType* pPrimitiveDT = poDDL->getDataTypeByName(strElementDT);
        if (NULL == pPrimitiveDT)
        {
            DDLEnum* poEnum = poDDL->getEnumByName(strElementDT);
            pPrimitiveDT = poDDL->getDataTypeByName(poEnum->getType());
        }
        pElementDT = static_cast<IDDLDataType*>(pPrimitiveDT);
    }

    // insert the element at the third, the last and the first position
    unsigned int aPosition[] = { 2, 100, 0 };
    for (unsigned int nPos = 0; nPos < 3; ++nPos)
    {
        // byte position has to be passed by the DDLElement's constructor
        unsigned int nBytePos;
        if (aPosition[nPos] < pComplexDT->getElements().size())
        {
            nBytePos = pComplexDT->getElements()[aPosition[nPos]]->getBytepos();
        }
        else
        {
            DDLInspector oInspector(false);
            ASSERT_EQ(a_util::result::SUCCESS, oInspector.visitDDL(poDDL));
            nBytePos = oInspector.getLastBytePosOfAStructReal(strComplexDT);
        }

        DDLElement* pElement = new DDLElement(pElementDT,
            a_util::strings::format("%s_%d", strElementDT.c_str(), nPos),
            nBytePos,
            1,
            DDLByteorder::e_le,
            DDLAlignment::e1);

        pComplexDT->addElement(pElement, aPosition[nPos]);
    }
}

/**
* @detail Test the insert function by inserting DDL objects at different positions.
*/
TEST(cTesterDDLRep,
    TestInsert)
{
    TEST_REQ("ACORE-7238");

    DDLImporter oDDLImporter("files/test_insert.description", false, false);
    oDDLImporter.setMergeDefaults(false);
    ASSERT_EQ(a_util::result::SUCCESS, oDDLImporter.createNew());

    DDLDescription * poDDL = oDDLImporter.getDDL();
    ASSERT_TRUE(NULL !=poDDL);

    InsertHeaderExtDeclaration(poDDL);
    InsertBaseUnit(poDDL);
    InsertRefUnit(poDDL);
    InsertUnit(poDDL);

    // It is not necessary to test the insert function for the prefixes, enums,
    // primitive data types, complex data types and streams,
    // because cDDLContainerNoClone<T>::Insert is already tested by the base units.

    // There is no insert function for cDDLStreamStruct, because
    // there is always only one cDDLStreamStruct.

    {
        // test primitive data type
        InserStructElement("test_complex_dt", "tUInt16", poDDL);
        // test complex data type
        InserStructElement("test_complex_dt", "tHeaderStruct", poDDL);
        // test enum type
        InserStructElement("test_complex_dt", "test_enum", poDDL);

        // calculate the byte positions
        DDLComplex* pComplexDT = poDDL->getStructByName("test_complex_dt");
        DDLInspector oInspector(false);
        oInspector.visitDDL(poDDL);
        oInspector.InitNewCheck();
        oInspector.setAutoCorrect(true),
            oInspector.visit(pComplexDT);
    }

    a_util::filesystem::Path strTargetFile("test_insert_results.description");
    if (a_util::filesystem::exists(strTargetFile))
    {
        a_util::filesystem::remove(strTargetFile);
    }

    DDLPrinter oPrinter;
    ASSERT_EQ(a_util::result::SUCCESS, oPrinter.visitDDL(poDDL));
    ASSERT_EQ(a_util::result::SUCCESS, oPrinter.toFile(strTargetFile.toString()));
    a_util::result::Result res = CompOutput(strTargetFile.toString(), "files/test_insert_results_ref.description");
    a_util::filesystem::remove(strTargetFile);
    ASSERT_EQ(a_util::result::SUCCESS, res);

    oDDLImporter.destroyDDL();
    poDDL = NULL;

}

/**
* @detail Test import from invalid description file.
* Read an invalid_numbits_attribute.description file and build up the object representation.
*/
TEST(cTesterDDLRep,
    TestImportInvalidNumbitsAttributeWithArray)
{
    TEST_REQ("ACORE-6472");

    DDLImporter oDDLImporter;
    ASSERT_TRUE((ERR_NOT_INITIALIZED == oDDLImporter.createNew())) << 
        "Uninitialized status of the DDLImporter not detected.";
    ASSERT_EQ(a_util::result::SUCCESS,oDDLImporter.setFile("files/invalid_numbits_attribute.description")) << 
        "Could not load invalid_numbits_attribute.description file.";
    ASSERT_EQ(ERR_INVALID_ARG , oDDLImporter.createNew());
    // free allocated resources
    oDDLImporter.destroyDDL();

}

/**
* @detail Test inspection of an invalid description file.
* Try to inspect a complex structure which is invalid due to a misplaced numbits attribute.
* Passed if an error occurs on visiting a complex structure which is invalid and thus not existing.
*/
TEST(cTesterDDLRep,
    TestInspectInvalidNumbitsAttributeWithArray)
{
    TEST_REQ("ACORE-6472");

    DDLImporter oDDLImporter;
    ASSERT_TRUE((ERR_NOT_INITIALIZED == oDDLImporter.createNew())) << 
        "Uninitialized status of the DDLImporter not detected.";
    ASSERT_EQ(a_util::result::SUCCESS,oDDLImporter.setFile("files/invalid_numbits_attribute.description")) << 
        "Could not load invalid_numbits_attribute.description file.";
    ASSERT_EQ(ERR_INVALID_ARG , oDDLImporter.createNew());

    DDLInspector oInspector;
    DDLDescription *pDDLDesc = oDDLImporter.getDDL();
    ASSERT_TRUE(NULL !=pDDLDesc);

    ASSERT_EQ(ERR_POINTER , oInspector.visit(pDDLDesc->getStructByName("ObjectFusiontAssociation")));

    // free allocated resources
    oDDLImporter.destroyDDL();

}

void TestDDL40(const DDLDescription* pDDL)
{
    const DDLComplex* pStruct = pDDL->getStructByName("tTest");
    ASSERT_TRUE(pStruct);
    ASSERT_EQ(pStruct->getDDLVersion() , ddl::DDLVersion::ddl_version_20);
    const DDLElementVec& oElements = pStruct->getElements();
    ASSERT_EQ(oElements.size() , 4);

    ASSERT_EQ(oElements[0]->getName() , "bBool");
    ASSERT_EQ(oElements[0]->getByteorder() , DDLByteorder::plattform_little_endian_8);
    ASSERT_EQ(oElements[0]->getBytepos() , 0);
    ASSERT_EQ(oElements[0]->getBitpos() , 0);
    ASSERT_EQ(oElements[0]->getNumBits() , 8);
    ASSERT_EQ(oElements[0]->getAlignment() , 1);

    ASSERT_EQ(oElements[3]->getName() , "fFloat32");
    ASSERT_EQ(oElements[3]->getByteorder() , DDLByteorder::plattform_little_endian_8);
    ASSERT_EQ(oElements[3]->getBytepos() , 6);
    ASSERT_EQ(oElements[3]->getBitpos() , 0);
    ASSERT_EQ(oElements[3]->getNumBits() , 32);
    ASSERT_EQ(oElements[3]->getAlignment() , 4);

    const DDLStreamMetaType* pStreamMetaType = pDDL->getStreamMetaTypeByName("adtf/default");
    ASSERT_TRUE(pStreamMetaType);
    ASSERT_EQ(pStreamMetaType->getVersion() , "1");
    ASSERT_EQ(pStreamMetaType->getParent() , "");
    ASSERT_TRUE(pStreamMetaType->getParentObject() == NULL);
    ASSERT_EQ(pStreamMetaType->getProperties().size() , 2);
    ASSERT_EQ(pStreamMetaType->getProperties()[0]->getName() , "md_struct");
    ASSERT_EQ(pStreamMetaType->getProperties()[0]->getType() , "string");
    ASSERT_EQ(pStreamMetaType->getProperties()[1]->getName() , "md_definitions");
    ASSERT_EQ(pStreamMetaType->getProperties()[1]->getType() , "string");
    const DDLStreamMetaType* pADTFDefault = pStreamMetaType;

    pStreamMetaType = pDDL->getStreamMetaTypeByName("test_parent_not_yet_defined");
    ASSERT_TRUE(pStreamMetaType);
    ASSERT_EQ(pStreamMetaType->getVersion() , "1");
    ASSERT_EQ(pStreamMetaType->getParent() , "adtf/default");
    ASSERT_EQ(pStreamMetaType->getParentObject() , pADTFDefault);
    ASSERT_TRUE(pStreamMetaType->getProperties().empty());

    pStreamMetaType = pDDL->getStreamMetaTypeByName("test");
    ASSERT_TRUE(pStreamMetaType);
    ASSERT_EQ(pStreamMetaType->getVersion() , "1");
    ASSERT_EQ(pStreamMetaType->getParent() , "adtf/default");
    ASSERT_EQ(pStreamMetaType->getParentObject() , pADTFDefault);
    ASSERT_EQ(pStreamMetaType->getProperties().size() , 1);
    ASSERT_EQ(pStreamMetaType->getProperties()[0]->getName() , "test_prop");
    ASSERT_EQ(pStreamMetaType->getProperties()[0]->getType() , "tInt32");
}

/**
* @detail Test the building up of a DDL object representation in Version 4.0
* Read an adtf.description file and build up the object representation
*/
TEST(cTesterDDLRep,
    TestVersion40)
{
    TEST_REQ("ACORE-7704 CDPKGDDL-14");

    a_util::memory::unique_ptr<DDLDescription> pDDL;

    {
        DDLImporter oDDLImporter;
        ASSERT_EQ(a_util::result::SUCCESS,oDDLImporter.setFile("files/adtf_v40.description")) << 
            "Could not load adtf_v40.description file.";
        ASSERT_EQ(a_util::result::SUCCESS,oDDLImporter.createNew()) << 
            "Creation of object hierarchy failed.";
        pDDL.reset(oDDLImporter.getDDL());
        ASSERT_TRUE( (NULL !=pDDL.get())) <<  "Invalid DDL object retrieved.";
        TestDDL40(pDDL.get());
    }

    DDLPrinter oPrinter;
    ASSERT_EQ(a_util::result::SUCCESS, oPrinter.visitDDL(pDDL.get()));

    {
        DDLImporter oDDLImporter;
        ASSERT_EQ(a_util::result::SUCCESS,oDDLImporter.setXML(oPrinter.getXML())) << 
            "Could not load adtf_v40.description file.";
        ASSERT_EQ(a_util::result::SUCCESS,oDDLImporter.createNew()) << 
            "Creation of object hierarchy failed.";
        pDDL.reset(oDDLImporter.getDDL());
        ASSERT_TRUE( (NULL !=pDDL.get())) <<  "Invalid DDL object retrieved.";
        TestDDL40(pDDL.get());
    }    
}

/**
* @detail Test the building up of a DDL object representation from manually set information
*/
TEST(cTesterDDLRep,
    TestManualCreation)
{
    TEST_REQ("CDPKGDDL-63");

    std::unique_ptr<DDLDescription> pDescription(DDLDescription::createDefault(ddl::DDLVersion::ddl_version_invalid));
    auto pTestStruct = new DDLComplex("test", 1);
    ASSERT_EQ(pTestStruct->getDDLVersion(), DDLVersion::getDefaultVersion());
    pDescription->addStruct(pTestStruct);

    pTestStruct = new DDLComplex("test_version", 1, "", DDLAlignment::e1, DDLElementVec(), 1, ddl::DDLVersion::ddl_version_30);
    ASSERT_EQ(pTestStruct->getDDLVersion(), ddl::DDLVersion::ddl_version_30);
    pDescription->addStruct(pTestStruct);

    DDLPrinter oPrinter;
    oPrinter.visitDDL(pDescription.get());
    a_util::xml::DOM oDom;
    ASSERT_EQ(a_util::result::SUCCESS, oDom.fromString(oPrinter.getXML()));

    a_util::xml::DOMElement oStructNode;
    ASSERT_EQ(a_util::result::SUCCESS, oDom.findNode("//struct[@name='test']", oStructNode));
    ASSERT_EQ(oStructNode.getAttribute("ddlversion"), DDLVersion::getDefaultVersion().toString());
    ASSERT_EQ(a_util::result::SUCCESS, oDom.findNode("//struct[@name='test_version']", oStructNode));
    ASSERT_EQ(ddl::DDLVersion::fromString(oStructNode.getAttribute("ddlversion")),
        ddl::DDLVersion::ddl_version_30);
}

/**
* @detail Test DDL with special chars in names, special characters + _ . / should be supported.
*/
TEST(cTesterDDLRep,
    TestSpecialChars)
{
    TEST_REQ("CDDDL-43");

    DDLImporter oDDLImporter;
    std::string strXML(DDL_TEST_STRING_SPECIAL_NAMES);
    ASSERT_EQ(a_util::result::SUCCESS, oDDLImporter.setXML(strXML)) << 
        "Could not set XML string.";
    ASSERT_EQ(a_util::result::SUCCESS, oDDLImporter.createNew()) <<  "Creation of DDL failed.";
    auto ddl = oDDLImporter.getDDL();
    ASSERT_TRUE((NULL != oDDLImporter.getDDL())) <<  "Invalid DDL object retrieved.";

    auto special_struct = ddl->getStructByName("name_with+special/characters");
    ASSERT_TRUE(NULL != special_struct);


    ASSERT_EQ(special_struct->getName(), "name_with+special/characters");

    for (auto element : special_struct->getElements())
    {
        if (a_util::strings::compare("ui32/MajorType", element->getName().c_str()) == 0)
        {
            continue;
        }
        else if (a_util::strings::compare("ui32_Sub+Type", element->getName().c_str()) == 0)
        {
            continue;
        }
        else if (a_util::strings::compare("ui32.Flags", element->getName().c_str()) == 0)
        {
            continue;
        }
        else
        {
            auto err_msg = a_util::strings::format("Unexpected element name: '%s'", element->getName().c_str());
            ASSERT_TRUE(false) <<  err_msg.c_str();
        }
    }

    DDLInspector oInspector;
    ASSERT_TRUE(a_util::result::SUCCESS == oInspector.visitDDL(ddl));

    // free allocated resources
    oDDLImporter.destroyDDL();

}

/**
* @detail Test if changing the size of static size arrays works.
* Strategy:   - Load DDL with static sized array
*             - Change size of array.
*             - Print DDL description as xml
*             - import generated description
*             - Check if DDL has updated array size
*/
TEST(cTesterDDLRep,
    TestChangeStaticArraySize)
{
    TEST_REQ("CDDDL-35");

    std::string updatedDDL;
    {
        DDLImporter oDDLImporter;
        std::string strXML(DDL_TEST_STRING_STATIC_ARRAY);
        ASSERT_EQ(a_util::result::SUCCESS, oDDLImporter.setXML(strXML)) << 
            "Could not set XML string.";
        ASSERT_EQ(a_util::result::SUCCESS, oDDLImporter.createNew()) <<  "Creation of DDL failed.";
        auto ddl = oDDLImporter.getDDL();
        ASSERT_TRUE((NULL != oDDLImporter.getDDL())) <<  "Invalid DDL object retrieved.";

        auto array_struct = ddl->getStructByName("tStaticArray");
        ASSERT_TRUE(NULL != array_struct);

        auto array_element = array_struct->getElements()[0];

        ASSERT_TRUE(NULL != array_element);

        ASSERT_EQ(8, array_element->getArraysize());

        // change arraysize
        array_element->setArraysize(16);

        ASSERT_EQ(16, array_element->getArraysize());

        DDLPrinter oPrinter;
        oPrinter.visitDDL(ddl);

        updatedDDL = oPrinter.getXML();

        oDDLImporter.destroyDDL();
    }

    // import the exported description
    {
        DDLImporter oDDLImporter;
        ASSERT_EQ(a_util::result::SUCCESS, oDDLImporter.setXML(updatedDDL)) << 
            "Could not set XML string.";
        ASSERT_EQ(a_util::result::SUCCESS, oDDLImporter.createNew()) <<  "Creation of DDL failed.";
        auto ddl = oDDLImporter.getDDL();
        ASSERT_TRUE((NULL != oDDLImporter.getDDL())) <<  "Invalid DDL object retrieved.";

        auto array_struct = ddl->getStructByName("tStaticArray");
        ASSERT_TRUE(NULL != array_struct);

        auto array_element = array_struct->getElements()[0];

        ASSERT_TRUE(NULL != array_element);

        ASSERT_EQ(16, array_element->getArraysize());
    }

}

/**
* @detail Test the identification of dynamic objects in a DDL object representation
* by reading strings and build up object representations.
*/
TEST(cTesterDDLRep,
    TestHasDynamicArrays)
{
    TEST_REQ("ACORE-3904");

    /* Import DDL without dynamic structures*/
    DDLImporter oDDLImporter;
    ASSERT_TRUE((ERR_NOT_INITIALIZED == oDDLImporter.createNew())) << 
        "Uninitialized status of the DDLImporter not detected.";
    ASSERT_EQ(a_util::result::SUCCESS, oDDLImporter.setFile("files/adtf.description")) << 
        "Could not load adtf.description file.";
    ASSERT_EQ(a_util::result::SUCCESS, oDDLImporter.createNew()) << 
        "Creation of object hierarchy failed.";
    DDLDescription * poDDL = oDDLImporter.getDDL();

    ASSERT_FALSE(poDDL->hasDynamicStructs());

    // free allocated resources
    oDDLImporter.destroyDDL();

    /* Import DDL with dynamic structures*/
    DDLImporter oDDLImporterDyn;
    ASSERT_TRUE((ERR_NOT_INITIALIZED == oDDLImporterDyn.createNew())) << 
        "Uninitialized status of the DDLImporter not detected.";
    ASSERT_EQ(a_util::result::SUCCESS, oDDLImporterDyn.setFile("files/adtf_dynamic.description")) << 
        "Could not load adtf_dynamic.description file.";
    ASSERT_EQ(a_util::result::SUCCESS, oDDLImporterDyn.createNew()) << 
        "Creation of object hierarchy failed.";
    DDLDescription * poDDLDyn = oDDLImporterDyn.getDDL();

    ASSERT_TRUE(poDDLDyn->hasDynamicStructs()) <<  "DDL should contain dynamic structs";

    // free allocated resources
    oDDLImporterDyn.destroyDDL();
}
