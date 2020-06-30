/**
 * @file
 * Tester init.
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

class cCompareFile
{
private:
    std::vector<std::string> _lines;
    std::vector<std::string>::iterator _pos;
public:
    void Open(const std::string& filepath)
    {
        if (a_util::filesystem::OK == a_util::filesystem::readTextLines(filepath, _lines))
        {
            _pos = _lines.begin();
        }
        else
        {
            _lines.clear();
            _pos = _lines.end();
        }
    }
    bool isValid() const 
    { 
        return _pos != _lines.end();
    }
    void ReadLine(std::string& line) 
    {
        if (isValid())
        {
            line = *_pos;
            a_util::strings::replace(line, "\r", "");
            _pos++;
        }
    }
};

namespace ddl_generator
{
    namespace oo
    {
        //define all needed error types and values locally
        _MAKE_RESULT(0, ERR_NOERROR);
        _MAKE_RESULT(-11, ERR_INVALID_FILE);
        _MAKE_RESULT(-20, ERR_NOT_FOUND);
        _MAKE_RESULT(-24, ERR_PATH_NOT_FOUND);
        _MAKE_RESULT(-38, ERR_FAILED);
    }
}
using namespace ddl_generator::oo;

#define __adtf_test_result(expr) ASSERT_TRUE(fep::isOk(expr))

#ifdef WIN32
#pragma warning(disable : 4355) // 'this' used in initializer list
#endif

#define LOG_POS LOG_INFO(a_util::strings::format("Starting Test '%s' (Line %d)", __FUNCTION__, __LINE__).c_str());
 //fprintf(stdout, "Starting Test '%s' (Line %d)\n", __FUNCTION__, __LINE__);

static const std::string OPTION_HELP = " --help ";

static const std::string  PROPERTY_DESCRIPTIONFILE = " --descriptionfile=";
static const std::string  PROPERTY_HEADERFILE = " --headerfile=";
static const std::string  PROPERTY_STRUCT = " --struct=";

static const std::string  DIR_FILES = "./files/";
static const std::string  DIR_EXPECTED = DIR_FILES + "expected_files/";
static const std::string  DIR_ORIGINAL = DIR_FILES + "original_files/";

static const std::string  PATH_RESULT_DESC = DIR_FILES + "result.description";
static const std::string  PATH_RESULT_HEADER = DIR_FILES + "result.h";

static const std::string  PATH_DESC_BASE_ALL = DIR_ORIGINAL + "base_all.description";
static const std::string  PATH_HEADER_BASE_ALL = DIR_ORIGINAL + "base_all.h";

static const std::string  PATH_DESC_ENUMS = DIR_ORIGINAL + "enums.description";
static const std::string  PATH_HEADER_ENUMS = DIR_ORIGINAL + "enums.h";

 static const std::string  PATH_EXPECTED_RES_DESC_EXIST_STRUCT = DIR_EXPECTED + "expected_result_existing_struct.description";
static const std::string  PATH_EXPECTED_RES_DESC_NEWFILE_ALL = DIR_EXPECTED + "expected_result_newfile_all.description";
 static const std::string  PATH_EXPECTED_RES_DESC_NEWFILE_STRUCT = DIR_EXPECTED + "expected_result_newfile_struct.description";
static const std::string  PATH_EXPECTED_RES_HEADER_EXIST_ALL = DIR_EXPECTED + "expected_result_existing_all.h";
 static const std::string  PATH_EXPECTED_RES_HEADER_EXIST_STRUCT = DIR_EXPECTED + "expected_result_existing_struct.h";
 static const std::string  PATH_EXPECTED_RES_HEADER_NEWFILE_STRUCT = DIR_EXPECTED + "expected_result_newfile_struct.h";

a_util::filesystem::Path m_strDDL2HeaderExe;
a_util::filesystem::Path m_strHeader2DDLExe;

static bool isSkippable(std::string strLine)
{
    return strLine.empty() ||
        strLine.find("): dump") != std::string::npos ||
           strLine.find("): info") != std::string::npos ||
           strLine.find("): warning") != std::string::npos ||
           strLine.find("): error") != std::string::npos ||
           strLine.find("leaving thread loop") != std::string::npos;
}

static a_util::result::Result CompDDL(const ddl::DDLDescription* pddl1, const ddl::DDLDescription* pddl2)
{
    int flags = ddl::DDLCompare::dcf_data_types + ddl::DDLCompare::dcf_structs;
    return ddl::DDLCompare::isEqual(pddl1, pddl2, flags);
}

static a_util::result::Result CompOutput(const std::string& strFile1, const std::string& strFile2, int64_t  nSkipLines = 0)
{
    cCompareFile oFile1;
    cCompareFile oFile2;
    oFile1.Open(strFile1);
    oFile2.Open(strFile2);
    
    if (!oFile1.isValid() || !oFile2.isValid())
    {
        return (ERR_FAILED);
    }

    std::string strLine1;
    std::string strLine2;
    while (oFile1.isValid() && oFile2.isValid())
    {
        oFile1.ReadLine(strLine1);
        oFile2.ReadLine(strLine2);
        if (strLine1 != strLine2)
        {
            printf("lines don't match:\n%s\n%s\n", strLine1.c_str(), strLine2.c_str());
            return (ERR_FAILED);
        }
    }

    if (oFile1.isValid() || oFile2.isValid())
    {
        return (ERR_FAILED);
    }

    return ERR_NOERROR;
}

a_util::result::Result CompareDDLFiles(std::string strFile1, std::string strFile2)
{
    ddl::DDLImporter oImporter1;
    oImporter1.setFile(strFile1);
    oImporter1.createNew();

    ddl::DDLImporter oImporter2;
    oImporter2.setFile(strFile2);
    oImporter2.createNew();

    ddl::DDLDescription* pDesc1 = oImporter1.getDDL();

    ddl::DDLDescription* pDesc2 = oImporter2.getDDL();

    if(pDesc1->getStructs().size() == pDesc2->getStructs().size())
    {
        return ERR_NOERROR;
    }
    else
    {
        return (ERR_FAILED);
    }
}

class cTesterddlgeneratorCmdLine : public ::testing::Test
{
protected:
    void SetUp()
    {
        if (m_strDDL2HeaderExe.isEmpty())
        {
#ifndef __QNX__
            m_strDDL2HeaderExe = TEST_INSTALL_DIR;
#else
            // For QNX we have to cross compile and install tester_ddl_* manually by FTP in a test dir ".", then
            // adding the binaries (ddl2header, header2ddl) in ./bin and the .description/.h files in ./files/...
            m_strDDL2HeaderExe = ".";
#endif
            /* Detect platform and switch to correct dummy application */
            m_strDDL2HeaderExe.append("bin");
           

#if _DEBUG
            m_strDDL2HeaderExe.append("debug");
#endif
#if (defined(_MSC_VER))
            // WIN x86 vc90 || WIN x64 vc100
            m_strHeader2DDLExe = m_strDDL2HeaderExe;
            m_strDDL2HeaderExe.append("ddl2header.exe");
            m_strHeader2DDLExe.append("header2ddl.exe");

#elif (defined(__linux) || defined(__QNX__))
            // LINUX x64 gcc46
            m_strHeader2DDLExe = m_strDDL2HeaderExe;
            m_strDDL2HeaderExe.append("ddl2header");
            m_strHeader2DDLExe.append("header2ddl");
#else
            // this goes for vc120 or apple or arm or whatever.
            assert("Platform currently not supported");
#endif // Version check
            m_strDDL2HeaderExe.makeCanonical();
            m_strHeader2DDLExe.makeCanonical();
        }
    }

    void TearDown()
    {
        // nothing to do yet
    }
};

/**
* @detail Test for using the FEP ddlgenerator core to import a structure from ddl to header.
*/
TEST_F(cTesterddlgeneratorCmdLine,
    TestImportStructInHeader)
{
    TEST_REQ("");
    
    // basic generation
    std::string strArgs;
    strArgs = PROPERTY_DESCRIPTIONFILE + PATH_DESC_BASE_ALL +
        PROPERTY_HEADERFILE + PATH_RESULT_HEADER;

    std::remove(PATH_RESULT_HEADER.c_str());
    ASSERT_TRUE(std::system((m_strDDL2HeaderExe.toString() + " " + strArgs).c_str()) == 0);
    ASSERT_TRUE(CompOutput(PATH_RESULT_HEADER, PATH_EXPECTED_RES_HEADER_EXIST_ALL) == 0);
    

    // with input struct 
    strArgs = PROPERTY_DESCRIPTIONFILE + PATH_DESC_BASE_ALL +
        PROPERTY_HEADERFILE + PATH_RESULT_HEADER + PROPERTY_STRUCT + 
        "tWaveFormat";

    std::remove(PATH_RESULT_HEADER.c_str());
    ASSERT_TRUE(std::system((m_strDDL2HeaderExe.toString() + " " + strArgs).c_str()) == 0);
    ASSERT_TRUE(CompOutput(PATH_RESULT_HEADER, PATH_EXPECTED_RES_HEADER_NEWFILE_STRUCT) == 0);

    // merge with existing file
    strArgs = PROPERTY_DESCRIPTIONFILE + PATH_DESC_BASE_ALL +
        PROPERTY_HEADERFILE + PATH_RESULT_HEADER;

    std::remove(PATH_RESULT_HEADER.c_str());
    ASSERT_TRUE(std::system((m_strDDL2HeaderExe.toString() + " " + strArgs).c_str()) == 0);
    ASSERT_TRUE(CompOutput(PATH_RESULT_HEADER, PATH_EXPECTED_RES_HEADER_EXIST_STRUCT) == 0);

    std::remove(PATH_RESULT_HEADER.c_str());
}

/**
* @detail Test for using the FEP ddlgenerator core to import a structure from header to ddl
*/
TEST_F(cTesterddlgeneratorCmdLine,
    TestImportStructInDdl)
{
    TEST_REQ("");
    
    std::string strArgs;
    strArgs = PROPERTY_DESCRIPTIONFILE + PATH_RESULT_DESC +
        PROPERTY_HEADERFILE + PATH_HEADER_BASE_ALL;

    std::remove(PATH_RESULT_HEADER.c_str());
    ASSERT_TRUE(std::system((m_strHeader2DDLExe.toString() + " " + strArgs).c_str()) == 0);
    ASSERT_TRUE(CompareDDLFiles(PATH_EXPECTED_RES_DESC_NEWFILE_ALL, PATH_RESULT_DESC) == 0);

    // with input struct 
    strArgs = PROPERTY_DESCRIPTIONFILE + PATH_RESULT_DESC +
        PROPERTY_HEADERFILE + PATH_HEADER_BASE_ALL + PROPERTY_STRUCT +
        "tWaveFormat";

    std::remove(PATH_RESULT_DESC.c_str());
    ASSERT_TRUE(std::system((m_strHeader2DDLExe.toString() + " " + strArgs).c_str()) == 0);
    ASSERT_TRUE(CompareDDLFiles(PATH_RESULT_DESC, PATH_EXPECTED_RES_DESC_NEWFILE_STRUCT) == 0);

    // merge with existing file
    strArgs = PROPERTY_DESCRIPTIONFILE + PATH_RESULT_DESC +
        PROPERTY_HEADERFILE + PATH_HEADER_BASE_ALL;

    std::remove(PATH_RESULT_HEADER.c_str());
    ASSERT_TRUE(std::system((m_strHeader2DDLExe.toString() + " " + strArgs).c_str()) == 0);
    ASSERT_TRUE(CompareDDLFiles(PATH_RESULT_DESC, PATH_EXPECTED_RES_DESC_EXIST_STRUCT) == 0);

    std::remove(PATH_RESULT_DESC.c_str());
}

/**
* @detail Tests ddl generator with invalid options, properties and combinations.
*/
TEST_F(cTesterddlgeneratorCmdLine,
    TestUseConsole)
{
    TEST_REQ("");
    
    std::string strArgs;

    // show help
    strArgs = OPTION_HELP;
    ASSERT_TRUE(std::system((m_strDDL2HeaderExe.toString() + " " + strArgs).c_str()) == 0);
    ASSERT_TRUE(std::system((m_strHeader2DDLExe.toString() + " " + strArgs).c_str()) == 0);

    // without option
    strArgs = "";
    ASSERT_TRUE(std::system((m_strDDL2HeaderExe.toString() + " " + strArgs).c_str()) != 0);
    ASSERT_TRUE(std::system((m_strHeader2DDLExe.toString() + " " + strArgs).c_str()) != 0);

    // with undefined option
    strArgs = "-wrongOption";
    ASSERT_TRUE(std::system((m_strDDL2HeaderExe.toString() + " " + strArgs).c_str()) != 0);
    ASSERT_TRUE(std::system((m_strHeader2DDLExe.toString() + " " + strArgs).c_str()) != 0);

    // with unknown options
    strArgs = "-no_option";
    ASSERT_TRUE(std::system((m_strDDL2HeaderExe.toString() + " " + strArgs).c_str()) != 0);
    ASSERT_TRUE(std::system((m_strHeader2DDLExe.toString() + " " + strArgs).c_str()) != 0);
    
    // without header file
    strArgs = PROPERTY_DESCRIPTIONFILE + PATH_DESC_BASE_ALL;
    ASSERT_TRUE(std::system((m_strDDL2HeaderExe.toString() + " " + strArgs).c_str()) != 0);
    ASSERT_TRUE(std::system((m_strHeader2DDLExe.toString() + " " + strArgs).c_str()) != 0);

    // with struct and without header file
    strArgs = PROPERTY_DESCRIPTIONFILE + PATH_DESC_BASE_ALL + 
        PROPERTY_STRUCT + "tMyStruct";
    ASSERT_TRUE(std::system((m_strDDL2HeaderExe.toString() + " " + strArgs).c_str()) != 0);
    ASSERT_TRUE(std::system((m_strHeader2DDLExe.toString() + " " + strArgs).c_str()) != 0);

    // with struct and without description file
    strArgs = PROPERTY_HEADERFILE + 
              PATH_HEADER_BASE_ALL + PROPERTY_STRUCT + 
        PROPERTY_STRUCT + "tMyStruct";
    ASSERT_TRUE(std::system((m_strDDL2HeaderExe.toString() + " " + strArgs).c_str()) != 0);
    ASSERT_TRUE(std::system((m_strHeader2DDLExe.toString() + " " + strArgs).c_str()) != 0);

    // with header file as description file
    strArgs = PROPERTY_DESCRIPTIONFILE + PATH_HEADER_BASE_ALL + 
        PROPERTY_HEADERFILE + PATH_RESULT_HEADER;
    ASSERT_TRUE(std::system((m_strDDL2HeaderExe.toString() + " " + strArgs).c_str()) != 0);
    ASSERT_TRUE(std::system((m_strHeader2DDLExe.toString() + " " + strArgs).c_str()) != 0);
    
    // with non existing description file and header file
    strArgs = PROPERTY_DESCRIPTIONFILE + PATH_RESULT_DESC + 
        PROPERTY_HEADERFILE + PATH_RESULT_HEADER;
    ASSERT_TRUE(std::system((m_strDDL2HeaderExe.toString() + " " + strArgs).c_str()) != 0);
    ASSERT_TRUE(std::system((m_strHeader2DDLExe.toString() + " " + strArgs).c_str()) != 0);
    
    // with unknown struct
    strArgs = PROPERTY_DESCRIPTIONFILE + PATH_DESC_BASE_ALL +
        PROPERTY_HEADERFILE + PATH_RESULT_HEADER +
        PROPERTY_STRUCT + "tUnknownStruct";
    ASSERT_TRUE(std::system((m_strDDL2HeaderExe.toString() + " " + strArgs).c_str()) != 0);
    ASSERT_TRUE(std::system((m_strHeader2DDLExe.toString() + " " + strArgs).c_str()) != 0);
}

/**
* @detail Tests if enumerations are written correctly to header
*/
TEST_F(cTesterddlgeneratorCmdLine,
    TestEnumsDDL2Header)
{
    TEST_REQ("");
    
    std::string strArgs;
    strArgs = PROPERTY_DESCRIPTIONFILE + PATH_DESC_ENUMS +
        PROPERTY_HEADERFILE + PATH_RESULT_HEADER;

    std::remove(PATH_RESULT_HEADER.c_str());
    ASSERT_TRUE(std::system((m_strDDL2HeaderExe.toString() + " " + strArgs).c_str()) == 0);

    std::string headerContent;
    a_util::filesystem::readTextFile(PATH_RESULT_HEADER, headerContent);

    // check if types are present in header
    ASSERT_TRUE(headerContent.find("tAlphabet") != std::string::npos);
    ASSERT_TRUE(headerContent.find("tTimber") != std::string::npos);
    ASSERT_TRUE(headerContent.find("tColor") != std::string::npos);
    ASSERT_TRUE(headerContent.find("tEnums") != std::string::npos);

    // check if enum values are present in header
    ASSERT_TRUE(headerContent.find("ABC") != std::string::npos);
    ASSERT_TRUE(headerContent.find("DEF") != std::string::npos);
    ASSERT_TRUE(headerContent.find("GHI") != std::string::npos);
    ASSERT_TRUE(headerContent.find("W_FIR") != std::string::npos);
    ASSERT_TRUE(headerContent.find("W_PINE") != std::string::npos);
    ASSERT_TRUE(headerContent.find("W_OAK") != std::string::npos);
    ASSERT_TRUE(headerContent.find("W_SPRUCE") != std::string::npos);
    ASSERT_TRUE(headerContent.find("C_UNKNOWN") != std::string::npos);
    ASSERT_TRUE(headerContent.find("C_RED") != std::string::npos);
    ASSERT_TRUE(headerContent.find("C_CYAN") != std::string::npos);

    // check if comments are present in header
    ASSERT_TRUE(headerContent.find("tEnumCommentary") != std::string::npos);
    ASSERT_TRUE(headerContent.find("alpha-element-description") != std::string::npos);
    ASSERT_TRUE(headerContent.find("alpha-element-comment") != std::string::npos);

    std::remove(PATH_RESULT_HEADER.c_str());

}
