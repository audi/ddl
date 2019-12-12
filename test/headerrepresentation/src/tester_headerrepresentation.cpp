/**
 *
 * Test functions for <template>
 *
 * @file
 * Copyright &copy; Audi Electronics Venture GmbH. All rights reserved
 *
 * $Author$
 * $Date$
 * $Revision$
 *
 * @remarks
 *
 */

#include <ddl.h>
#include <gtest/gtest.h>
#include "../../_common/adtf_compat.h"
#include "../../_common/compat.h"
#include "ddl_definitions.h"
#include "header_importer.h"
#include "header_header.h"
#include "header_printer.h"
#include "header_to_ddl_converter.h"
#include "ddl_to_header_converter.h"

using namespace ddl;
#ifdef WIN32
#pragma warning(disable : 4355) // 'this' used in initializer list
#endif

#define LOG_POS LOG_INFO(a_util::strings::format("Starting Test '%s' (Line %d)", __FUNCTION__, __LINE__).c_str());

HeaderTypesVec* m_pVecADTFTypes;

const ddl::HeaderType* getType(const char* strName)
{
    for (ddl::HeaderTypesVec::const_iterator pIter = m_pVecADTFTypes->begin(); pIter != m_pVecADTFTypes->end(); pIter++)
    {
        if ((*pIter)->getName().compare(strName) == 0)
        {
            return (*pIter);
        }
    }
    return NULL;
}

void TestHeader(ddl::Header* pHeader, const char* strName, size_t szStructs, size_t szTypedefs, size_t szConstants)
{
    ASSERT_TRUE(pHeader);
    ASSERT_TRUE(pHeader->getName().compare(strName) == 0);
    ASSERT_TRUE(pHeader->getStructs().size() == szStructs);
    ASSERT_TRUE(pHeader->getTypedefs().size() == szTypedefs);
    ASSERT_TRUE(pHeader->getConstants().size() == szConstants);
}

void TestStruct(Header* pHeader, size_t szStructIndex, size_t szElements, bool bCanContainMoreElements /* = false */)
{
    ASSERT_TRUE(pHeader);
    ASSERT_TRUE(pHeader->getStructs().size() > szStructIndex);
    HeaderStruct* pStruct = pHeader->getStructs()[szStructIndex];
    ASSERT_TRUE(pStruct);
    if (bCanContainMoreElements)
    {
        ASSERT_TRUE(pStruct->getElements().size() >= szElements);
    }
    else
    {
        ASSERT_TRUE(pStruct->getElements().size() == szElements);
    }
}


void TestStruct(Header* pHeader, size_t szStructIndex, size_t szElements, const char* strName, size_t szPacking, bool bCanContainMoreElements = false)
{
    TestStruct(pHeader, szStructIndex, szElements, bCanContainMoreElements);
    ASSERT_TRUE(pHeader->getStructs().size() > 0);
    HeaderStruct* pStruct = pHeader->getStructs()[szStructIndex];
    ASSERT_TRUE(pStruct->getName().compare(strName) == 0);
    size_t test = pStruct->getPacking();
    size_t test2 = szPacking;
    ASSERT_TRUE(pStruct->getPacking() == szPacking);
}

void TestStructElement(Header* pHeader, size_t szStructIndex, size_t szElementIndex, const HeaderType *pType, const char* strName, size_t szArraySize)
{
    TestStruct(pHeader, szStructIndex, szElementIndex, true);
    ASSERT_TRUE(pHeader->getStructs().size() > 0);
    HeaderStruct* pStruct = pHeader->getStructs()[szStructIndex];
    HeaderStructElement* pElement = pStruct->getElements()[szElementIndex];
    ASSERT_TRUE(pElement);
    ASSERT_TRUE(pElement->getType() == pType);
    ASSERT_TRUE(pElement->getName().compare(strName) == 0);
    ASSERT_TRUE(pElement->getArraySize() == szArraySize);
}

void TestTypedef(Header* pHeader, size_t szTypedefIndex, const HeaderType* pOriginalType, const char* strName, size_t szPacking)
{
    ASSERT_TRUE(pHeader);
    ASSERT_TRUE(pHeader->getTypedefs().size() > szTypedefIndex);
    HeaderTypedef* pTypedef = pHeader->getTypedefs()[szTypedefIndex];
    ASSERT_TRUE(pTypedef);
    ASSERT_TRUE(pTypedef->getOriginalType() == pOriginalType);
    ASSERT_TRUE(pTypedef->getName().compare(strName) == 0);
    ASSERT_TRUE(pTypedef->getPacking() == szPacking);
}

void TestConstant(Header* pHeader, size_t szConstantIndex, const HeaderType* pType, int64_t i64Value, const char* strName)
{
    ASSERT_TRUE(pHeader);
    ASSERT_TRUE(pHeader->getConstants().size() > szConstantIndex);
    HeaderConstant* pConstant = pHeader->getConstants()[szConstantIndex];
    ASSERT_TRUE(pConstant);
    ASSERT_TRUE(pConstant->getName().compare(strName) == 0);
    ASSERT_TRUE(pConstant->getType() == pType);
    ASSERT_TRUE(pConstant->asInt64() == i64Value);
}

void TestDDL(DDLDescription* pDescription, const a_util::filesystem::Path &oReferenceFile)
{
    DDLPrinter oPrinter;
    pDescription->accept(&oPrinter);
    std::string strXML = oPrinter.getXML();
    ASSERT_TRUE(strXML.find("description") != std::string::npos);
    ASSERT_TRUE(pDescription->getHeader()->getLanguageVersion() == ddl::DDLVersion::ddl_version_40);
    ASSERT_TRUE(pDescription->getBaseunits().size() > 0);
    ASSERT_TRUE(pDescription->getDatatypes().size() > 0);
}



void test_set_up()
{
    m_pVecADTFTypes = HeaderImporter::getDefaultTypes();
}

void test_tear_down()
{
    if (m_pVecADTFTypes)
    {
        for (HeaderTypesVec::iterator pIter = m_pVecADTFTypes->begin(); pIter != m_pVecADTFTypes->end(); pIter++)
        {
            delete *pIter;
        }
        delete m_pVecADTFTypes;
        m_pVecADTFTypes = NULL;
    }
}

struct Testinitializer
{
    Testinitializer()
    {
        test_set_up();
    }
    ~Testinitializer()
    {
        test_tear_down();
    }
};

/**
* @detail This test makes sure that basic structures can be read.
*/
TEST(cTesterHeaderRepresentation,
    SimpleHeaders)
{
    TEST_REQ("");

    Testinitializer oTestSetup;
    HeaderImporter oHeaderImporter;
    ASSERT_EQ(a_util::result::SUCCESS, oHeaderImporter.setFileName("files/simple_header.h"));
    ASSERT_EQ(a_util::result::SUCCESS, oHeaderImporter.setKnownTypes(m_pVecADTFTypes));
    ASSERT_EQ(a_util::result::SUCCESS, oHeaderImporter.createNew());
    Header* pHeader = oHeaderImporter.getHeader();

    TestHeader(pHeader, "simple_header.h", 3, 1, 0);

    TestStruct(pHeader, 0, 1, "simple_struct1", getType("uint8_t")->getPacking());
    TestStructElement(pHeader, 0, 0, getType("uint8_t"), "a", 1);

    TestStruct(pHeader, 1, 1, "simple_struct2", getType("tUInt16")->getPacking());
    TestStructElement(pHeader, 1, 0, getType("tUInt16"), "b", 1);

    TestStruct(pHeader, 2, 1, "simple_struct3", getType("unsigned int")->getPacking());
    TestStructElement(pHeader, 2, 0, getType("unsigned int"), "c", 1);

    if (pHeader->getStructs().size() >= 2)
    {
        TestTypedef(pHeader, 0, pHeader->getStructs()[2], "simple_struct4", pHeader->getStructs()[2]->getPacking());
    }
    
    delete pHeader;
    TearDown();
}

/**
* @detail This test makes sure typedefs can be read.
*/
TEST(cTesterHeaderRepresentation,
    Typedefs)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    HeaderImporter oHeaderImporter;
    ASSERT_EQ(a_util::result::SUCCESS, oHeaderImporter.setFileName("files/typedef_header.h"));
    ASSERT_EQ(a_util::result::SUCCESS, oHeaderImporter.setKnownTypes(m_pVecADTFTypes));
    ASSERT_EQ(a_util::result::SUCCESS, oHeaderImporter.createNew());
    Header* pHeader = oHeaderImporter.getHeader();

    TestHeader(pHeader, "typedef_header.h", 0, 5, 0);

    TestTypedef(pHeader, 0, getType("tUInt32"), "UINT_32", getType("tUInt32")->getPacking());
    TestTypedef(pHeader, 1, getType("tUInt16"), "UINT_16", getType("tUInt16")->getPacking());
    TestTypedef(pHeader, 2, getType("tUInt64"), "UINT_64", getType("tUInt64")->getPacking());

    if (pHeader->getTypedefs().size() >= 3)
    {
        TestTypedef(pHeader, 3, pHeader->getTypedefs()[2], "uint64", getType("tUInt64")->getPacking());
        TestTypedef(pHeader, 4, pHeader->getTypedefs()[3], "supergross", getType("tUInt64")->getPacking());
    }
    delete pHeader;
}

/**
* @detail This test makes sure that constants can be read.
*/
TEST(cTesterHeaderRepresentation,
    Constants)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    HeaderImporter oHeaderImporter;
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/constants_header.h"));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setKnownTypes(m_pVecADTFTypes));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.createNew());
    Header* pHeader = oHeaderImporter.getHeader();

    TestHeader(pHeader, "constants_header.h", 0, 0, 2);

    TestConstant(pHeader, 0, getType("tInt16"), 42, "d");
    TestConstant(pHeader, 1, getType("tInt32"), 43, "e");
    delete pHeader;
}

/**
* @detail This test makes sure that defines can be read.
*/
TEST(cTesterHeaderRepresentation,
    Defines)
{
    TEST_REQ("ACORE-6822, ACORE-7188");

    Testinitializer oTestSetup;
    HeaderImporter oHeaderImporter;
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/defines_header.h"));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setKnownTypes(m_pVecADTFTypes));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.createNew());
    Header* pHeader = oHeaderImporter.getHeader();

    TestHeader(pHeader, "defines_header.h", 0, 0, 7);
    TestConstant(pHeader, 0, getType("tUInt64"), 5, "FIRST_CONSTANT");
    TestConstant(pHeader, 1, getType("tUInt64"), 6, "OTHER_CONSTANT");
    TestConstant(pHeader, 2, getType("tUInt64"), 7, "LAST_CONSTANT");
    TestConstant(pHeader, 3, getType("tUInt64"), 32, "RDB_SIZE_OBJECT_NAME");
    TestConstant(pHeader, 4, getType("tUInt64"), 64, "RDB_SIZE_SCP_NAME");
    TestConstant(pHeader, 5, getType("tUInt64"), 1024, "RDB_SIZE_FILENAME");
    TestConstant(pHeader, 6, getType("tUInt64"), 8, "RDB_SIZE_TRLIGHT_PHASES");

    delete pHeader;

    // Now check if a explicitly set default type is used
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setDefaultIntegerType(getType("tUInt32")));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.createNew());
    pHeader = oHeaderImporter.getHeader();    

    TestHeader(pHeader, "defines_header.h", 0, 0, 7);
    TestConstant(pHeader, 0, getType("tUInt32"), 5, "FIRST_CONSTANT");
    TestConstant(pHeader, 1, getType("tUInt32"), 6, "OTHER_CONSTANT");
    TestConstant(pHeader, 2, getType("tUInt32"), 7, "LAST_CONSTANT");
    TestConstant(pHeader, 3, getType("tUInt32"), 32, "RDB_SIZE_OBJECT_NAME");
    TestConstant(pHeader, 4, getType("tUInt32"), 64, "RDB_SIZE_SCP_NAME");
    TestConstant(pHeader, 5, getType("tUInt32"), 1024, "RDB_SIZE_FILENAME");
    TestConstant(pHeader, 6, getType("tUInt32"), 8, "RDB_SIZE_TRLIGHT_PHASES");
    delete pHeader;
}

/**
* @detail This test makes sure that wrong structures, typedefs and defines not parsed.
*/
TEST(cTesterHeaderRepresentation,
    FaultyHeader)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    // Correct defines will be ignored
    HeaderImporter oHeaderImporter;
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/faulty_header1.h"));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setKnownTypes(m_pVecADTFTypes));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.createNew());
    Header* pHeader = oHeaderImporter.getHeader();
    TestHeader(pHeader, "faulty_header1.h", 0, 0, 0);
    delete pHeader;

    // Unnamed struct
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/faulty_header2.h"));
    ASSERT_NE(a_util::result::SUCCESS,oHeaderImporter.createNew());
    pHeader = oHeaderImporter.getHeader();
    delete pHeader;

    // Member has unknown type
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/faulty_header3.h"));
    ASSERT_NE(a_util::result::SUCCESS,oHeaderImporter.createNew());
    pHeader = oHeaderImporter.getHeader();
    delete pHeader;

    // Unnamed struct
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/faulty_header4.h"));
    ASSERT_NE(a_util::result::SUCCESS,oHeaderImporter.createNew());
    pHeader = oHeaderImporter.getHeader();
    delete pHeader;

    // Weird typedefs
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/faulty_header5.h"));
    ASSERT_NE(a_util::result::SUCCESS,oHeaderImporter.createNew());
    pHeader = oHeaderImporter.getHeader();
    delete pHeader;
}

/**
* @detail This test makes sure that wrong settings will lead to structures, 
* typedefs and defines not being parsed.
*/
TEST(cTesterHeaderRepresentation,
    FaultySettings)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    // Test if define will not be parsed when no default type is set and tUInt64 is not a known type
    // Remove tUInt64 from known types
    for (ddl::HeaderTypesVec::iterator pIter = m_pVecADTFTypes->begin(); pIter != m_pVecADTFTypes->end(); pIter++)
    {
        if ((*pIter)->getName().compare("tUInt64") == 0)
        {
            delete *pIter;
            m_pVecADTFTypes->erase(pIter);
            break;
        }
    }
    HeaderImporter oHeaderImporter;
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/defines_header.h"));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setKnownTypes(m_pVecADTFTypes));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setDefaultIntegerType(NULL));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.createNew());
    Header* pHeader = oHeaderImporter.getHeader();
    TestHeader(pHeader, "defines_header.h", 0, 0, 0);
    delete pHeader;
    // Check importer with faulty filename
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/does_not_exist_header.h"));
    ASSERT_TRUE(a_util::result::isFailed(oHeaderImporter.createNew()));
}

/**
* @detail This test makes sure that comments in and around structures, 
* typedefs and defines will be ignored and everything is parsed successfully.
*/
TEST(cTesterHeaderRepresentation,
    CommentedHeader)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    HeaderImporter oHeaderImporter;
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/commented_header.h"));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setKnownTypes(m_pVecADTFTypes));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.createNew());
    Header* pHeader = oHeaderImporter.getHeader();
    TestHeader(pHeader, "commented_header.h", 3, 3, 3);

    //Check structures
    TestStruct(pHeader, 0, 1, "simple_struct1", getType("tUInt8")->getPacking());
    TestStructElement(pHeader, 0, 0, getType("tUInt8"), "a", 1);

    TestStruct(pHeader, 1, 1, "simple_struct2", getType("tUInt16")->getPacking());
    TestStructElement(pHeader, 1, 0, getType("tUInt16"), "b", 1);

    TestStruct(pHeader, 2, 1, "simple_struct3", getType("tUInt32")->getPacking());
    TestStructElement(pHeader, 2, 0, getType("tUInt32"), "c", 1);

    // Check typedefs
    TestTypedef(pHeader, 0,  getType("tUInt32"), "UINT_32", getType("tUInt32")->getPacking());
    TestTypedef(pHeader, 1,  getType("tUInt16"), "UINT_16", getType("tUInt16")->getPacking());
    if (pHeader->getStructs().size() >= 2)
    {
        TestTypedef(pHeader, 2, pHeader->getStructs()[2], "simple_struct4", pHeader->getStructs()[2]->getPacking());
    }

    // Check constants
    TestConstant(pHeader, 0, getType("tInt16"), 42, "d");
    TestConstant(pHeader, 1, getType("tInt32"), 43, "e");
    TestConstant(pHeader, 2, getType("tUInt64"), 6, "constant1");
    delete pHeader;
}

/**
* @detail This test makes sure that complex (nested) structures can be parsed successfully.
*/
TEST(cTesterHeaderRepresentation,
    ComplexStructs)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    HeaderImporter oHeaderImporter;
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/complex_header.h"));
    ASSERT_TRUE(a_util::result::isOk(oHeaderImporter.setKnownTypes(m_pVecADTFTypes)));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setDefaultIntegerType(NULL));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.createNew());
    Header* pHeader = oHeaderImporter.getHeader();

    TestHeader(pHeader, "complex_header.h", 5, 1, 0);

    TestStruct(pHeader, 0, 1, "simple_struct1", getType("tUInt8")->getPacking());
    TestStructElement(pHeader, 0, 0, getType("tUInt8"), "a", 1);

    TestStruct(pHeader, 1, 1, "simple_struct2", getType("tUInt16")->getPacking());
    TestStructElement(pHeader, 1, 0, getType("tUInt16"), "b", 1);

    TestStruct(pHeader, 2, 1, "simple_struct3", getType("tUInt32")->getPacking());
    TestStructElement(pHeader, 2, 0, getType("tUInt32"), "c", 1);

    if (pHeader->getStructs().size() >= 2)
    {
        TestStruct(pHeader, 3, 3, "complex_struct1", pHeader->getStructs()[2]->getPacking());
        TestStructElement(pHeader, 3, 0, getType("tUInt16"), "a", 1);
        TestStructElement(pHeader, 3, 1, pHeader->getStructs()[0], "b", 1);
        TestStructElement(pHeader, 3, 2, pHeader->getStructs()[2], "c", 1);

        TestStruct(pHeader, 4, 3, "complex_struct2", pHeader->getStructs()[2]->getPacking());
        TestStructElement(pHeader, 4, 0, pHeader->getStructs()[0], "d", 1);
        TestStructElement(pHeader, 4, 1, pHeader->getStructs()[2], "e", 1);
        TestStructElement(pHeader, 4, 2, pHeader->getStructs()[3], "f", 1);

        TestTypedef(pHeader, 0, pHeader->getStructs()[2], "simple_struct4", pHeader->getStructs()[2]->getPacking());
    }
    delete pHeader;
}

/**
* @detail This test makes sure that structures containing array elements can be parsed successfully.
*/
TEST(cTesterHeaderRepresentation,
    ArrayStructs)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    HeaderImporter oHeaderImporter;
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/array_header.h"));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setKnownTypes(m_pVecADTFTypes));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setDefaultIntegerType(NULL));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.createNew());
    Header* pHeader = oHeaderImporter.getHeader();

    TestHeader(pHeader, "array_header.h", 4, 0, 2);
    TestConstant(pHeader, 0, getType("tUInt64"), 8, "count1");
    TestConstant(pHeader, 1, getType("tUInt8"), 9, "count2");

    TestStruct(pHeader, 0, 1, "simple_array_struct", getType("tUInt8")->getPacking());
    TestStructElement(pHeader, 0, 0, getType("tUInt8"), "a", 5);

    TestStruct(pHeader, 1, 1, "simple_struct1", getType("tUInt8")->getPacking());
    TestStructElement(pHeader, 1, 0, getType("tUInt8"), "b", 1);

    TestStruct(pHeader, 2, 5, "complex_array_struct1", getType("tUInt8")->getPacking());
    TestStructElement(pHeader, 2, 0, getType("tUInt8"), "c", 1);

    if (pHeader->getStructs().size() >= 1)
    {
        TestStructElement(pHeader, 2, 1, pHeader->getStructs()[1], "d", 1);
        TestStructElement(pHeader, 2, 2, pHeader->getStructs()[0], "f", 1);
        TestStructElement(pHeader, 2, 3, pHeader->getStructs()[1], "g", 6);
        TestStructElement(pHeader, 2, 4, pHeader->getStructs()[0], "h", 7);

        TestStruct(pHeader, 3, 5, "complex_array_struct2", getType("tUInt8")->getPacking());
        TestStructElement(pHeader, 3, 0, getType("tUInt8"), "i", 8);
        TestStructElement(pHeader, 3, 1, pHeader->getStructs()[1], "j", 1);
        TestStructElement(pHeader, 3, 2, pHeader->getStructs()[0], "k", 1);
        TestStructElement(pHeader, 3, 3, pHeader->getStructs()[1], "l", 9);
        TestStructElement(pHeader, 3, 4, pHeader->getStructs()[0], "m", 10);
    }
    delete pHeader;
}

/**
* @detail This test makes sure that the packing information stated by #pragma pack can be parsed successfully.
*/
TEST(cTesterHeaderRepresentation,
    StructsPacking)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    HeaderImporter oHeaderImporter;
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/packing_header.h"));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setKnownTypes(m_pVecADTFTypes));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setDefaultIntegerType(NULL));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.createNew());
    Header* pHeader = oHeaderImporter.getHeader();

    TestHeader(pHeader, "packing_header.h", 5, 1, 0);

    TestStruct(pHeader, 0, 1, "simple_struct1", getType("tUInt8")->getPacking());
    TestStructElement(pHeader, 0, 0, getType("tUInt8"), "a", 1);

    TestStruct(pHeader, 1, 1, "simple_struct2", 1);
    TestStructElement(pHeader, 1, 0, getType("tUInt16"), "b", 1);

    TestStruct(pHeader, 2, 1, "simple_struct3", 2);
    TestStructElement(pHeader, 2, 0, getType("tUInt32"), "c", 1);

    TestStruct(pHeader, 3, 1, "simple_struct5", 2);
    TestStructElement(pHeader, 3, 0, getType("tUInt64"), "d", 1);

    TestStruct(pHeader, 4, 1, "simple_struct6", 1);
    TestStructElement(pHeader, 4, 0, getType("tUInt64"), "e", 1);

    if (pHeader->getStructs().size() >= 2)
    {
        TestTypedef(pHeader, 0, pHeader->getStructs()[2], "simple_struct4", pHeader->getStructs()[2]->getPacking());
    }
    delete pHeader;
}

/**
* @detail This test makes sure that a simple header can be printed.
*/
TEST(cTesterHeaderRepresentation,
    PrintSimpleHeaders)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    HeaderImporter oHeaderImporter;
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/simple_header.h"));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setKnownTypes(m_pVecADTFTypes));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.createNew());
    Header* pHeader = oHeaderImporter.getHeader();
    HeaderPrinter oPrinter;
    pHeader->accept(&oPrinter);
    std::string strExpected;
    a_util::filesystem::readTextFile(a_util::filesystem::Path("files/simple_header_expected.h"), strExpected);
    strExpected.erase(std::remove(strExpected.begin(), strExpected.end(), '\r'), strExpected.end());
    std::string test = oPrinter.getHeader();
    ASSERT_TRUE(strExpected.compare(oPrinter.getHeader()) == 0);
    delete pHeader;
}

/**
* @detail This test makes sure that a complex header can be printed.
*/
TEST(cTesterHeaderRepresentation,
    PrintComplexStructs)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    HeaderImporter oHeaderImporter;
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/complex_header.h"));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setKnownTypes(m_pVecADTFTypes));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.createNew());
    Header* pHeader = oHeaderImporter.getHeader();
    HeaderPrinter oPrinter;
    pHeader->accept(&oPrinter);
    std::string strExpected;
    a_util::filesystem::readTextFile(a_util::filesystem::Path("files/complex_header_expected.h"), strExpected);
    strExpected.erase(std::remove(strExpected.begin(), strExpected.end(), '\r'), strExpected.end());
    ASSERT_TRUE(strExpected.compare(oPrinter.getHeader()) == 0);
    delete pHeader;
}

/**
* @detail This test makes sure that a header with arrays can be printed.
*/
TEST(cTesterHeaderRepresentation,
    PrintArrayStructs)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    HeaderImporter oHeaderImporter;
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/array_header.h"));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setKnownTypes(m_pVecADTFTypes));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.createNew());
    Header* pHeader = oHeaderImporter.getHeader();
    HeaderPrinter oPrinter;
    pHeader->accept(&oPrinter);
    std::string strExpected;
    a_util::filesystem::readTextFile(a_util::filesystem::Path("files/array_header_expected.h"), strExpected);
    strExpected.erase(std::remove(strExpected.begin(), strExpected.end(), '\r'), strExpected.end());
    ASSERT_TRUE(strExpected.compare(oPrinter.getHeader()) == 0);
    delete pHeader;
}

/**
* @detail This test makes sure that a header with packing can be printed.
*/
TEST(cTesterHeaderRepresentation,
    PrintPackingStructs)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    HeaderImporter oHeaderImporter;
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/packing_header.h"));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setKnownTypes(m_pVecADTFTypes));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.createNew());
    Header* pHeader = oHeaderImporter.getHeader();
    HeaderPrinter oPrinter;
    pHeader->accept(&oPrinter);
    std::string strExpected;
    a_util::filesystem::readTextFile(a_util::filesystem::Path("files/packing_header_expected.h"), strExpected);
    strExpected.erase(std::remove(strExpected.begin(), strExpected.end(), '\r'), strExpected.end());
    ASSERT_TRUE(strExpected.compare(oPrinter.getHeader()) == 0);
    delete pHeader;
}

/**
* @detail This test makes sure that a header with typedefs can be printed.
*/
TEST(cTesterHeaderRepresentation,
    PrintTypedefs)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    HeaderImporter oHeaderImporter;
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/typedef_header.h"));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setKnownTypes(m_pVecADTFTypes));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.createNew());
    Header* pHeader = oHeaderImporter.getHeader();
    HeaderPrinter oPrinter;
    pHeader->accept(&oPrinter);
    std::string strExpected;
    a_util::filesystem::readTextFile(a_util::filesystem::Path("files/typedef_header_expected.h"), strExpected);
    strExpected.erase(std::remove(strExpected.begin(), strExpected.end(), '\r'), strExpected.end());
    ASSERT_TRUE(strExpected.compare(oPrinter.getHeader()) == 0);
    delete pHeader;
}

/**
* @detail This test makes sure that a simple header can be converted to DDL.
*/
TEST(cTesterHeaderRepresentation,
    ConvertSimpleHeaderToDDL)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    HeaderImporter oHeaderImporter;
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/simple_header.h"));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setKnownTypes(m_pVecADTFTypes));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.createNew());
    Header* pHeader = oHeaderImporter.getHeader();
    HeaderToDDLConverter oConverter;
    pHeader->accept(&oConverter);
    oConverter.createNew();
    TestDDL(oConverter.getDDL(), a_util::filesystem::Path("files/simple_header_expected.description"));
    
    delete pHeader;
    delete oConverter.getDDL();
}

/**
* @detail This test makes sure that a complex header can be converted to DDL.
*/
TEST(cTesterHeaderRepresentation,
    ConvertComplexHeaderToDDL)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    HeaderImporter oHeaderImporter;
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/complex_header.h"));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setKnownTypes(m_pVecADTFTypes));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.createNew());
    Header* pHeader = oHeaderImporter.getHeader();
    HeaderToDDLConverter oConverter;
    pHeader->accept(&oConverter);
    oConverter.createNew();
    TestDDL(oConverter.getDDL(), a_util::filesystem::Path("files/complex_header_expected.description"));

    delete pHeader;
    delete oConverter.getDDL();
}

/**
* @detail This test makes sure that a header with array information can be converted to DDL.
*/
TEST(cTesterHeaderRepresentation,
    ConvertArrayHeaderToDDL)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    HeaderImporter oHeaderImporter;
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/array_header.h"));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setKnownTypes(m_pVecADTFTypes));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.createNew());
    Header* pHeader = oHeaderImporter.getHeader();
    HeaderToDDLConverter oConverter;
    pHeader->accept(&oConverter);
    oConverter.createNew();
    TestDDL(oConverter.getDDL(), a_util::filesystem::Path("files/array_header_expected.description"));

    delete pHeader;
    delete oConverter.getDDL();
}

/**
* @detail This test makes sure that a header with packing information can be converted to DDL.
*/
TEST(cTesterHeaderRepresentation,
    ConvertPackingHeaderToDDL)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    HeaderImporter oHeaderImporter;
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/packing_header.h"));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setKnownTypes(m_pVecADTFTypes));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.createNew());
    Header* pHeader = oHeaderImporter.getHeader();
    HeaderToDDLConverter oConverter;
    pHeader->accept(&oConverter);
    oConverter.createNew();
    TestDDL(oConverter.getDDL(), a_util::filesystem::Path("files/packing_header_expected.description"));

    delete pHeader;
    delete oConverter.getDDL();
}

/**
* @detail This test makes sure that a VTD RDB header with can be converted to DDL.
*/
TEST(cTesterHeaderRepresentation,
    ConvertRDBHeaderToDDL)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    HeaderImporter oHeaderImporter;
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/viRDBIcd.h"));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setKnownTypes(m_pVecADTFTypes));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.createNew());
    Header* pHeader = oHeaderImporter.getHeader();
    HeaderToDDLConverter oConverter;
    pHeader->accept(&oConverter);
    oConverter.createNew();
    TestDDL(oConverter.getDDL(), a_util::filesystem::Path("files/viRDBIcd_expected.description"));

    delete pHeader;
    delete oConverter.getDDL();
}

/**
* @detail This test makes sure that a single struct and its dependencies can be converted to DDL.
*/
TEST(cTesterHeaderRepresentation,
    ConvertStructToDDL)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    HeaderImporter oHeaderImporter;
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/complex_header.h"));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setKnownTypes(m_pVecADTFTypes));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.createNew());
    Header* pHeader = oHeaderImporter.getHeader();
    HeaderToDDLConverter oConverter;
    TestHeader(pHeader, "complex_header.h", 5, 1, 0);
    if (pHeader->getStructs().size() >= 4)
    {
        pHeader->getStructs()[4]->accept(&oConverter);
    }
    oConverter.createNew();
    TestDDL(oConverter.getDDL(), a_util::filesystem::Path("files/complex_struct2_expected.description"));
    delete pHeader;
    delete oConverter.getDDL();
}

/**
* @detail This test makes sure that multiple structures and its dependencies can be converted to DDL.
*/
TEST(cTesterHeaderRepresentation,
    ConvertTwoStructsToDDL)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    HeaderImporter oHeaderImporter;
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/viRDBIcd.h"));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setKnownTypes(m_pVecADTFTypes));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.createNew());
    Header* pHeader = oHeaderImporter.getHeader();
    HeaderToDDLConverter oConverter;
    TestHeader(pHeader, "viRDBIcd.h", 55, 0, 221);
    if (pHeader->getStructs().size() >= 24)
    {
        pHeader->getStructs()[10]->accept(&oConverter);
        pHeader->getStructs()[24]->accept(&oConverter);
    }
    oConverter.createNew();
    TestDDL(oConverter.getDDL(), a_util::filesystem::Path("files/viRDBIcd_extract_expected.description"));
    delete pHeader;
    delete oConverter.getDDL();
}

/**
* @detail This test makes sure that simple DDL structures can be converted to a header.
*/
TEST(cTesterHeaderRepresentation,
    ConvertSimpleStructsToHeader)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    DDLImporter oImporter(true, false);
    oImporter.setFile("files/adtf2.description");
    oImporter.createNew();
    DDLToHeaderConverter oConverter;
    oConverter.setKnownTypes(m_pVecADTFTypes);
    ASSERT_TRUE(oImporter.getDDL());
    oImporter.getDDL()->accept(&oConverter);
    oConverter.createNew();
    oConverter.getHeader();
    Header* pHeader = oConverter.getHeader();
    HeaderPrinter oPrinter;
    pHeader->accept(&oPrinter);
    std::string strExpected;
    a_util::filesystem::readTextFile(a_util::filesystem::Path("files/adtf_expected.h"), strExpected);
    strExpected.erase(std::remove(strExpected.begin(), strExpected.end(), '\r'), strExpected.end());
    ASSERT_TRUE(strExpected.compare(oPrinter.getHeader()) == 0);
    delete pHeader;
    oImporter.destroyDDL();
}

/**
* @detail This test makes sure that complex DDL structures can be converted to a header.
*/
TEST(cTesterHeaderRepresentation,
    ConvertComplexStructsToHeader)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    DDLImporter oImporter(true, false);
    oImporter.setFile("files/viRDBIcd_expected.description");
    oImporter.createNew();
    DDLToHeaderConverter oConverter;
    oConverter.setKnownTypes(m_pVecADTFTypes);
    ASSERT_TRUE(oImporter.getDDL());
    oImporter.getDDL()->accept(&oConverter);
    oConverter.createNew();
    oConverter.getHeader();
    Header* pHeader = oConverter.getHeader();
    HeaderPrinter oPrinter;
    pHeader->accept(&oPrinter);
    std::string strExpected;
    a_util::filesystem::readTextFile(a_util::filesystem::Path("files/viRDBIcd_expected.h"), strExpected);
    strExpected.erase(std::remove(strExpected.begin(), strExpected.end(), '\r'), strExpected.end());
    ASSERT_TRUE(strExpected.compare(oPrinter.getHeader()) == 0);
    delete pHeader;
    oImporter.destroyDDL();
}

/**
* @detail This test makes sure that selected DDL structures can be converted to a header.
*/
TEST(cTesterHeaderRepresentation,
    ConvertTwoStructsToHeader)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    DDLImporter oImporter;
    oImporter.setFile("files/viRDBIcd_expected.description");
    oImporter.createNew();
    DDLToHeaderConverter oConverter;
    oConverter.setKnownTypes(m_pVecADTFTypes);
    ASSERT_TRUE(oImporter.getDDL());
    oImporter.getDDL()->getStructByName("RDB_DRIVETRAIN_t")->accept(&oConverter);
    oImporter.getDDL()->getStructByName("RDB_SENSOR_STATE_t")->accept(&oConverter);
    oConverter.createNew();
    oConverter.getHeader();
    Header* pHeader = oConverter.getHeader();
    HeaderPrinter oPrinter;
    pHeader->accept(&oPrinter);
    std::string strExpected;
    a_util::filesystem::readTextFile(a_util::filesystem::Path("files/viRDBIcd_extract_expected.h"), strExpected);
    strExpected.erase(std::remove(strExpected.begin(), strExpected.end(), '\r'), strExpected.end());
    ASSERT_TRUE(strExpected.compare(oPrinter.getHeader()) == 0);
    delete pHeader;
    oImporter.destroyDDL();
}

/**
* @detail This test makes sure that the default types defined in the importer have the correct packing.
*/
TEST(cTesterHeaderRepresentation,
    CheckDefaultTypes)
{
    TEST_REQ("ACORE-6822");

    Testinitializer oTestSetup;
    // Check default types
#ifdef __ADTF_WIN32
    ASSERT_TRUE(getType("float")->getPacking() == 8);
#endif
#ifdef __ADTF_WIN64
    ASSERT_TRUE(getType("float")->getPacking() == 8);
#endif
#ifdef __ADTF_LINUX32
    ASSERT_TRUE(getType("float")->getPacking() == 4);
#endif
#ifdef __ADTF_LINUX64
    ASSERT_TRUE(getType("float")->getPacking() == 8);
#endif
    // Check default packing (no #pragma pack stated)
    HeaderImporter oHeaderImporter;
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setFileName("files/default_types_header.h"));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.setKnownTypes(m_pVecADTFTypes));
    ASSERT_EQ(a_util::result::SUCCESS,oHeaderImporter.createNew());
    Header* pHeader = oHeaderImporter.getHeader();

    TestHeader(pHeader, "default_types_header.h", 1, 0, 0);

#ifdef __GNUC__
    #ifdef __ADTF32
        TestStruct(pHeader, 0, 2, "doubles", 4);
    #endif
    #ifdef __ADTF64
        TestStruct(pHeader, 0, 2, "doubles", 8);
    #endif
#endif
#ifdef _MSC_VER
    TestStruct(pHeader, 0, 2, "doubles", getType("double")->getPacking());
#endif
    delete pHeader;
}


