/**
 * @file
 * Test implementation.
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

void DumpElements(const StaticDecoder& oDecoder)
{
    const uint8_t* pFirstElement = static_cast<const uint8_t*>(oDecoder.getElementAddress(0));
    for (size_t nElement = 0; nElement < oDecoder.getElementCount(); ++nElement)
    {
        const StructElement* pElement;
        ASSERT_EQ(a_util::result::SUCCESS, oDecoder.getElement(nElement, pElement));
        std::cout << pElement->name.c_str()
            << " @ " << static_cast<const uint8_t*>(oDecoder.getElementAddress(nElement)) - pFirstElement
            << ": " << access_element::get_value_as_string(oDecoder, nElement) << "\n";
    }
}

namespace static_struct
{

    struct tChildStruct
    {
        int8_t nValueDummy;
        int32_t nValue[3];
        int8_t nAfter;
    };

    struct tTest
    {
        tChildStruct sChild[2];
    };

    const char* strTestDesc =
        "<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>"
        "<structs>"
        "<struct alignment=\"4\" name=\"child_struct\" version=\"2\">"
        "<element alignment=\"1\" arraysize=\"1\" byteorder=\"BE\" bytepos=\"0\" name=\"value_dummy\" type=\"tInt8\"/>"
        "<element alignment=\"4\" arraysize=\"3\" byteorder=\"BE\" bytepos=\"1\" name=\"value\" type=\"tInt32\"/>"
        "<element alignment=\"1\" arraysize=\"1\" byteorder=\"BE\" bytepos=\"13\" name=\"after\" type=\"tInt8\"/>"
        "</struct>"
        "<struct alignment=\"4\" name=\"test\" version=\"2\">"
        "<element alignment=\"4\" arraysize=\"2\" byteorder=\"LE\" bytepos=\"0\" name=\"child\" type=\"child_struct\"/>"
        "</struct>"
        "</structs>";

    const tTest sTestData =
    {
        {
            {0x01, {0x02, 0x03, 0x04}, 0x05},
            {0x06, {0x07, 0x08, 0x09}, 0x0A},
        }
    };

    namespace serialized
    {
#pragma pack(push, 1)

        struct tChildStruct
        {
            int8_t nValueDummy;
            int32_t nValue[3];
            int8_t nAfter;
        };

        struct tTest
        {
            tChildStruct sChild[2];
        };

        const tTest sTestData =
        {
            {
                {0x01, {0x02000000, 0x03000000, 0x04000000}, 0x05},
                {0x06, {0x07000000, 0x08000000, 0x09000000}, 0x0A},
            }
        };
#pragma pack(pop)
    }

}

template <typename T>
void test_static(CodecFactory& oFactory, const T& sTestData, DataRepresentation eRep)
{
    std::cout << a_util::strings::format("%d == %d", oFactory.getStaticBufferSize(eRep), sizeof(sTestData)).c_str();
    ASSERT_EQ(oFactory.getStaticBufferSize(eRep) , sizeof(sTestData));
    T sTest = sTestData;
    StaticDecoder oDecoder = oFactory.makeStaticDecoderFor(&sTest, sizeof(sTest), eRep);
    StaticCodec oCodec = oFactory.makeStaticCodecFor(&sTest, sizeof(sTest), eRep);

    ASSERT_EQ(a_util::result::SUCCESS, oFactory.isValid());
    ASSERT_EQ(a_util::result::SUCCESS, oDecoder.isValid());
    ASSERT_EQ(a_util::result::SUCCESS, oCodec.isValid());

    ASSERT_EQ(oDecoder.getElementCount() , 10);
    DumpElements(oDecoder);

    ASSERT_EQ(access_element::get_value(oDecoder, "child[0].value_dummy").getInt8() , 1);
    ASSERT_EQ(access_element::get_value(oDecoder, "child[0].value[0]").getInt32() , 2);
    ASSERT_EQ(access_element::get_value(oDecoder, "child[0].value[1]").getInt32() , 3);
    ASSERT_EQ(access_element::get_value(oDecoder, "child[0].value[2]").getInt32() , 4);
    ASSERT_EQ(access_element::get_value(oDecoder, "child[0].after").getInt8() , 5);
    ASSERT_EQ(access_element::get_value(oDecoder, "child[1].value_dummy").getInt8() , 6);
    ASSERT_EQ(access_element::get_value(oDecoder, "child[1].value[0]").getInt32() , 7);
    ASSERT_EQ(access_element::get_value(oDecoder, "child[1].value[1]").getInt32() , 8);
    ASSERT_EQ(access_element::get_value(oDecoder, "child[1].value[2]").getInt32() , 9);
    ASSERT_EQ(access_element::get_value(oDecoder, "child[1].after").getInt8() , 10);

    ASSERT_EQ(access_element::get_value(oDecoder, "child[0].value_dummy").getType() , a_util::variant::VT_Int8);
    ASSERT_EQ(access_element::get_value(oDecoder, "child[0].value[0]").getType() , a_util::variant::VT_Int32);

    ASSERT_EQ(access_element::get_value(oDecoder, 300).getInt32() , 0);
    ASSERT_EQ(access_element::get_value(oDecoder, 0).getInt8() , 1);

    int32_t nNewData = 0x10;
    ASSERT_EQ(a_util::result::SUCCESS, oCodec.setElementValue(1, &nNewData));
    ASSERT_TRUE(0x10 == sTest.sChild[0].nValue[0] ||
        0x10000000 == sTest.sChild[0].nValue[0]);

    ASSERT_EQ(a_util::result::SUCCESS, access_element::set_value(oCodec, "child[1].value[2]", (int32_t)0x20));
    ASSERT_TRUE(0x20 == sTest.sChild[1].nValue[2] ||
        0x20000000 == sTest.sChild[1].nValue[2]);
}

/**
* @detail Check deserialized codec static information
*/
TEST(CodecTest,
    TestSatic)
{
    TEST_REQ("ACORE-7704 ACORE-7997 CDPKGDDL-12");

    CodecFactory oFactory("test", static_struct::strTestDesc);

    size_t nIndex = 200;
    ASSERT_EQ(a_util::result::SUCCESS, access_element::find_index(oFactory, "child[0].after", nIndex));
    ASSERT_EQ(nIndex , 4);
    ASSERT_EQ(a_util::result::SUCCESS, access_element::find_struct_index(oFactory, "child[1]", nIndex));
    ASSERT_EQ(nIndex , 5);
    ASSERT_EQ(a_util::result::SUCCESS, access_element::find_array_index(oFactory, "child", nIndex));
    ASSERT_EQ(nIndex , 0);

    test_static(oFactory, static_struct::sTestData, deserialized);
}

/**
* @detail  Check serialized codec static information
*/
TEST(CodecTest,
    TestSaticSerialized)
{
    TEST_REQ("ACORE-7704 CDPKGDDL-13");

    CodecFactory oFactory("test", static_struct::strTestDesc);
    test_static(oFactory, static_struct::serialized::sTestData, serialized);
}

namespace simple
{
    struct tMain
    {
        int8_t nArraySize;
        int32_t aArray[4];
        int16_t nAfter;
    };

    const char* strTestDesc =
        "<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>"
        "<structs>"
        "<struct alignment=\"4\" name=\"main\" version=\"2\">"
        "<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"array_size\" type=\"tInt8\"/>"
        "<element alignment=\"4\" arraysize=\"array_size\" byteorder=\"LE\" bytepos=\"1\" name=\"array\" type=\"tInt32\"/>"
        "<element alignment=\"2\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"-1\" name=\"after\" type=\"tInt16\"/>"
        "</struct>"
        "</structs>";

    const tMain sTestData =
    {
        4,
        {1, 2, 3, 4},
        8
    };

    namespace serialized
    {
#pragma pack(push, 1)
        struct tMain
        {
            int8_t nArraySize;
            int32_t aArray[4];
            int16_t nAfter;
        };

        const tMain sTestData =
        {
            4,
            {1, 2, 3, 4},
            8
        };
#pragma pack(pop)
    }

}

template <typename T>
void TestDynamicSimple(CodecFactory& oFactory, const T& sTestData, DataRepresentation eRep)
{
    Decoder oDecoder = oFactory.makeDecoderFor(&sTestData, sizeof(sTestData), eRep);
    DumpElements(oDecoder);

    ASSERT_EQ(a_util::result::SUCCESS, oFactory.isValid());
    ASSERT_EQ(a_util::result::SUCCESS, oDecoder.isValid());

    ASSERT_EQ(sizeof(T) , oDecoder.getBufferSize(eRep));
    std::cout << a_util::strings::toString((uint64_t)oDecoder.getElementCount()).c_str();
    ASSERT_EQ(oDecoder.getElementCount() , 6);

    ASSERT_EQ(access_element::get_value(oDecoder, "array_size").getInt8() , 4);
    ASSERT_EQ(access_element::get_value(oDecoder, "array[0]").getInt32() , 1);
    ASSERT_EQ(access_element::get_value(oDecoder, "array[1]").getInt32() , 2);
    ASSERT_EQ(access_element::get_value(oDecoder, "array[2]").getInt32() , 3);
    ASSERT_EQ(access_element::get_value(oDecoder, "array[3]").getInt32() , 4);
    ASSERT_EQ(access_element::get_value(oDecoder, "after").getInt16() , 8);
}

/**
* @detail  Check deserialized codec dynamic information
*/
TEST(CodecTest,
    TestDynamicSimple)
{
    TEST_REQ("ACORE-7704 CDPKGDDL-12");

    CodecFactory oFactory("main", simple::strTestDesc);
    ::TestDynamicSimple(oFactory, simple::sTestData, deserialized);
}

/**
* @detail  Check serialized codec dynamic information
*/
TEST(CodecTest,
    TestDynamicSimpleSerialized)
{
    TEST_REQ("ACORE-7704 CDPKGDDL-13");

    CodecFactory oFactory("main", simple::strTestDesc);
    ::TestDynamicSimple(oFactory, simple::serialized::sTestData, serialized);
}

namespace complex
{

    struct tArrayChildStruct
    {
        int8_t nChildSize;
        int32_t aChildArray[3];
        int8_t nChildSize2;
        int32_t aChildArray2[2];
        int16_t aFixedArray[3];
    };

    struct tArrayTest
    {
        int8_t nArraySize;
        tArrayChildStruct aArray[2];
    };

    struct tMain
    {
        int8_t nBefore;
        tArrayTest sTest;
        int8_t nAfter;
    };

    const char* strTestDesc =
        "<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>"
        "<structs>"
        "<struct alignment=\"4\" name=\"child_struct\" version=\"2\">"
        "<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"child_size\" type=\"tInt8\"/>"
        "<element alignment=\"4\" arraysize=\"child_size\" byteorder=\"LE\" bytepos=\"1\" name=\"child_array\" type=\"tInt32\"/>"
        "<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"-1\" name=\"child_size2\" type=\"tInt8\"/>"
        "<element alignment=\"4\" arraysize=\"child_size2\" byteorder=\"LE\" bytepos=\"-1\" name=\"child_array2\" type=\"tInt32\"/>"
        "<element alignment=\"2\" arraysize=\"3\" byteorder=\"LE\" bytepos=\"-1\" name=\"fixed_array\" type=\"tInt16\"/>" // does not end at a multiple of 4
        "</struct>"
        "<struct alignment=\"4\" name=\"test\" version=\"2\">"
        "<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"array_size\" type=\"tInt8\"/>"
        "<element alignment=\"4\" arraysize=\"array_size\" byteorder=\"LE\" bytepos=\"1\" name=\"array\" type=\"child_struct\"/>"
        "</struct>"
        "<struct alignment=\"4\" name=\"main\" version=\"2\">"
        "<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"before\" type=\"tInt8\"/>"
        "<element alignment=\"4\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"1\" name=\"test\" type=\"test\"/>"
        "<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"-1\" name=\"after\" type=\"tInt8\"/>"
        "</struct>"
        "</structs>";

    const tMain sTestData =
    {
        4,
        {
            2,
            {
                {
                    3, {1, 2, 3},
                        2, {10, 20},
                    {40, 50, 60}
                },
                {
                    3, {11, 22, 33},
                        2, {110, 220},
                    {400, 500, 600}
                }
            }
        },
            8
    };

    namespace serialized
    {
#pragma pack(push, 1)
        struct tArrayChildStruct
        {
            int8_t nChildSize;
            int32_t aChildArray[3];
            int8_t nChildSize2;
            int32_t aChildArray2[2];
            int16_t aFixedArray[3];
        };

        struct tArrayTest
        {
            int8_t nArraySize;
            tArrayChildStruct aArray[2];
        };

        struct tMain
        {
            int8_t nBefore;
            tArrayTest sTest;
            int8_t nAfter;
        };

        const tMain sTestData =
        {
            4,
            {
                2,
                {
                    {
                        3, {1, 2, 3},
                            2, {10, 20},
                        {40, 50, 60}
                    },
                    {
                        3, {11, 22, 33},
                            2, {110, 220},
                        {400, 500, 600}
                    }
                }
            },
                8
        };

#pragma pack(pop)
    }

}

template <typename T>
void TestDynamicComplex(CodecFactory& oFactory, const T& sTestData, DataRepresentation eRep)
{
    Decoder oDecoder = oFactory.makeDecoderFor(&sTestData, sizeof(sTestData), eRep);

    DumpElements(oDecoder);

    ASSERT_EQ(a_util::result::SUCCESS, oFactory.isValid());
    ASSERT_EQ(a_util::result::SUCCESS, oDecoder.isValid());

    ASSERT_EQ(sizeof(T) , oDecoder.getBufferSize(eRep));
    std::cout << a_util::strings::toString((uint64_t)oDecoder.getElementCount()).c_str();
    ASSERT_EQ(oDecoder.getElementCount() , 23);

    ASSERT_EQ(access_element::get_value(oDecoder, "before").getInt8() , 4);
    ASSERT_EQ(access_element::get_value(oDecoder, "test.array_size").getInt8() , 2);
    ASSERT_EQ(access_element::get_value(oDecoder, "test.array[0].child_size").getInt8() , 3);
    ASSERT_EQ(access_element::get_value(oDecoder, "test.array[0].child_array[0]").getInt32() , 1);
    ASSERT_EQ(access_element::get_value(oDecoder, "test.array[0].child_array[1]").getInt32() , 2);
    ASSERT_EQ(access_element::get_value(oDecoder, "test.array[0].child_array[2]").getInt32() , 3);
    ASSERT_EQ(access_element::get_value(oDecoder, "test.array[0].child_size2").getInt8() , 2);
    ASSERT_EQ(access_element::get_value(oDecoder, "test.array[0].child_array2[0]").getInt32() , 10);
    ASSERT_EQ(access_element::get_value(oDecoder, "test.array[0].child_array2[1]").getInt32() , 20);
    ASSERT_EQ(access_element::get_value(oDecoder, "test.array[0].fixed_array[0]").getInt16() , 40);
    ASSERT_EQ(access_element::get_value(oDecoder, "test.array[0].fixed_array[1]").getInt16() , 50);
    ASSERT_EQ(access_element::get_value(oDecoder, "test.array[0].fixed_array[2]").getInt16() , 60);
    ASSERT_EQ(access_element::get_value(oDecoder, "test.array[1].child_size").getInt8() , 3);
    ASSERT_EQ(access_element::get_value(oDecoder, "test.array[1].child_array[0]").getInt32() , 11);
    ASSERT_EQ(access_element::get_value(oDecoder, "test.array[1].child_array[1]").getInt32() , 22);
    ASSERT_EQ(access_element::get_value(oDecoder, "test.array[1].child_array[2]").getInt32() , 33);
    ASSERT_EQ(access_element::get_value(oDecoder, "test.array[1].child_size2").getInt8() , 2);
    ASSERT_EQ(access_element::get_value(oDecoder, "test.array[1].child_array2[0]").getInt32() , 110);
    ASSERT_EQ(access_element::get_value(oDecoder, "test.array[1].child_array2[1]").getInt32() , 220);
    ASSERT_EQ(access_element::get_value(oDecoder, "test.array[1].fixed_array[0]").getInt16() , 400);
    ASSERT_EQ(access_element::get_value(oDecoder, "test.array[1].fixed_array[1]").getInt16() , 500);
    ASSERT_EQ(access_element::get_value(oDecoder, "test.array[1].fixed_array[2]").getInt16() , 600);
    ASSERT_EQ(access_element::get_value(oDecoder, "after").getInt8() , 8);
}

/**
* @detail  Check deserialized codec dynamic information with complex data
*/
TEST(CodecTest,
    TestDynamicComplex)
{
    TEST_REQ("ACORE-7704 CDPKGDDL-12");

    CodecFactory oFactory("main", complex::strTestDesc);
    ::TestDynamicComplex(oFactory, complex::sTestData, deserialized);
}

/**
* @detail  Check serialized codec dynamic information with complex data
*/
TEST(CodecTest,
    TestDynamicComplexSerialized)
{
    TEST_REQ("ACORE-7704 CDPKGDDL-13");

    CodecFactory oFactory("main", complex::strTestDesc);
    ::TestDynamicComplex(oFactory, complex::serialized::sTestData, serialized);
}

class cTestPerf
{
public:
    virtual inline ~cTestPerf() {};
    virtual const char* getName() = 0;
    virtual void CreateCodec(const char* strStructName, const char* strDescription, void* pData, size_t nDataSize) = 0;
    virtual timestamp_t TestAccessAllElements(size_t nRepeats, const char* strStructName, const char* strDescription, void* pData, size_t nDataSize) = 0;
};

class cTestNewPerf: public cTestPerf
{
public:
    const char* getName()
    {
        return "New";
    }

    void CreateCodec(const char* strStructName, const char* strDescription, void* pData, size_t nDataSize)
    {
        CodecFactory oFactory(strStructName, strDescription);
        Decoder oCodec = oFactory.makeDecoderFor(pData, nDataSize);
    }

    timestamp_t TestAccessAllElements(size_t nRepeats, const char* strStructName, const char* strDescription, void* pData, size_t nDataSize)
    {
        CodecFactory oFactory(strStructName, strDescription);
        Decoder oCodec = oFactory.makeDecoderFor(pData, nDataSize);

        size_t nCount = oCodec.getElementCount();
        std::cout << a_util::strings::format("new elements: %d", nCount).c_str();
        timestamp_t now = a_util::system::getCurrentMicroseconds();
        for (size_t nRound = 0; nRound < nRepeats; ++nRound)
        {
            for (size_t nElement = 0; nElement < nCount; ++nElement)
            {
                uint64_t nBuffer;
                oCodec.getElementValue(nElement, &nBuffer);
            }
        }
        return a_util::system::getCurrentMicroseconds() - now;
    }
};

void TestPerf(const char* strStructName, const char* strDescription,
    void* pData, size_t nDataSize)
{
    size_t nRepeats = 10000;

    std::list<cTestPerf*> oTesters;
    oTesters.push_back(new cTestNewPerf());
#ifdef DDL_ENABLE_DEPRECATED_CODER
    oTesters.push_back(new cTestOldPerf());
#endif

    for (std::list<cTestPerf*>::iterator it = oTesters.begin(); it != oTesters.end(); ++it)
    {
        timestamp_t now = a_util::system::getCurrentMicroseconds();
        {
            for (size_t nRound = 0; nRound < nRepeats; ++nRound)
            {
                (*it)->CreateCodec(strStructName, strDescription, pData, nDataSize);
            }
        }
        timestamp_t nTime = a_util::system::getCurrentMicroseconds() - now;
        std::cout << a_util::strings::format("%s: create %lld", (*it)->getName(), nTime).c_str();
    }

    for (std::list<cTestPerf*>::iterator it = oTesters.begin(); it != oTesters.end(); ++it)
    {
        timestamp_t nTime = (*it)->TestAccessAllElements(nRepeats, strStructName, strDescription, pData, nDataSize);
        std::cout << a_util::strings::format("%s: access %lld", (*it)->getName(), nTime).c_str();
    }

    while (!oTesters.empty())
    {
        delete oTesters.front();
        oTesters.pop_front();
    }
}

/**
* @detail  Test codec performances for static information
*/
TEST(CodecTest,
    TestStaticPerf)
{
    TEST_REQ("ACORE-7704");

    static_struct::tTest sTest = static_struct::sTestData;
    TestPerf("test", static_struct::strTestDesc, &sTest, sizeof(sTest));
}

/**
* @detail  Test codec performances for dynamic information
*/
TEST(CodecTest,
    TestDynamicPerf)
{
    TEST_REQ("ACORE-7704");

#if 1
    simple::tMain sTest = simple::sTestData;
    TestPerf("main", simple::strTestDesc, &sTest, sizeof(sTest));
#else
    complex::tMain sTest = complex::sTestData;
    TestPerf("main", complex::strTestDesc, &sTest, sizeof(sTest));
#endif
}

namespace enums
{

    struct tMain
    {
        int32_t nStatic;
        int8_t nArraySize;
        int32_t aArray[3];
        int32_t nAfter;
    };

    const char* strTestDesc =
        "<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>"
        "<adtf:ddl>"
        "<enums>"
        "<enum name=\"tTest\" type=\"tInt32\">"
        "<element name=\"A\" value=\"1\"/>"
        "<element name=\"B\" value=\"2\"/>"
        "<element name=\"C\" value=\"3\"/>"
        "</enum>"
        "<enum name=\"tTest2\" type=\"tInt32\">"
        "<element name=\"D\" value=\"1\"/>"
        "<element name=\"E\" value=\"2\"/>"
        "<element name=\"F\" value=\"3\"/>"
        "</enum>"
        "</enums>"
        "<structs>"
        "<struct alignment=\"4\" name=\"main\" version=\"2\">"
        "<element alignment=\"4\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"static\" type=\"tTest\"/>"
        "<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"4\" name=\"array_size\" type=\"tInt8\"/>"
        "<element alignment=\"4\" arraysize=\"array_size\" byteorder=\"LE\" bytepos=\"5\" name=\"array\" type=\"tTest2\"/>"
        "<element alignment=\"4\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"-1\" name=\"after\" type=\"tTest2\"/>"
        "</struct>"
        "</structs>"
        "</adtf:ddl>";

    const tMain sTestData =
    {
        2,
        3,
        {1, 2, 3},
        3
    };

}

/**
* @detail  Check decoder functionalities for enums
*/
TEST(CodecTest,
    TestEnums)
{
    TEST_REQ("ACORE-7704");

    CodecFactory oFactory("main", enums::strTestDesc);
    ASSERT_EQ(a_util::result::SUCCESS, oFactory.isValid());
    Decoder oDecoder = oFactory.makeDecoderFor(&enums::sTestData, sizeof(enums::sTestData));
    DumpElements(oDecoder);
    ASSERT_EQ(a_util::result::SUCCESS, oDecoder.isValid());

    ASSERT_EQ(access_element::get_value_as_string(oDecoder, "static") , "B");
    ASSERT_EQ(access_element::get_value_as_string(oDecoder, "array_size") , "3");
    ASSERT_EQ(access_element::get_value_as_string(oDecoder, "array[0]") , "D");
    ASSERT_EQ(access_element::get_value_as_string(oDecoder, "array[1]") , "E");
    ASSERT_EQ(access_element::get_value_as_string(oDecoder, "array[2]") , "F");
    ASSERT_EQ(access_element::get_value_as_string(oDecoder, "after") , "F");
}

namespace constants
{

    struct tMain
    {
        int32_t nStatic;
        int8_t nArraySize;
        int32_t aArray[3];
        int32_t nAfter;
    };

    const char* strTestDesc =
        "<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>"
        "<adtf:ddl>"
        "<enums>"
        "<enum name=\"tTest\" type=\"tInt32\">"
        "<element name=\"A\" value=\"1\"/>"
        "<element name=\"B\" value=\"2\"/>"
        "<element name=\"C\" value=\"3\"/>"
        "</enum>"
        "<enum name=\"tTest2\" type=\"tInt32\">"
        "<element name=\"D\" value=\"1\"/>"
        "<element name=\"E\" value=\"2\"/>"
        "<element name=\"F\" value=\"3\"/>"
        "</enum>"
        "</enums>"
        "<structs>"
        "<struct alignment=\"4\" name=\"main\" version=\"2\">"
        "<element alignment=\"4\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"static\" type=\"tTest\" value=\"A\"/>"
        "<element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"4\" name=\"array_size\" type=\"tInt8\"/>"
        "<element alignment=\"4\" arraysize=\"array_size\" byteorder=\"LE\" bytepos=\"5\" name=\"array\" type=\"tTest2\" value=\"D\"/>"
        "<element alignment=\"4\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"-1\" name=\"after\" type=\"tTest2\" value=\"E\"/>"
        "</struct>"
        "</structs>"
        "</adtf:ddl>";

    const tMain sTestData =
    {
        0,
        3,
        {0, 0, 0},
        0
    };

}

/**
* @detail  Check decoder functionalities for constants
*/
TEST(CodecTest,
    TestConstants)
{
    TEST_REQ("ACORE-7704");

    CodecFactory oFactory("main", constants::strTestDesc);
    ASSERT_EQ(a_util::result::SUCCESS, oFactory.isValid());
    constants::tMain sTestData = constants::sTestData;
    Codec oCodec = oFactory.makeCodecFor(&sTestData, sizeof(sTestData));
    DumpElements(oCodec);
    ASSERT_EQ(a_util::result::SUCCESS, oCodec.isValid());

    ASSERT_EQ(access_element::get_value(oCodec, "static").asInt32() , 0);
    ASSERT_EQ(access_element::get_value(oCodec, "array_size").asInt32() , 3);
    ASSERT_EQ(access_element::get_value(oCodec, "array[0]").asInt32() , 0);
    ASSERT_EQ(access_element::get_value(oCodec, "array[1]").asInt32() , 0);
    ASSERT_EQ(access_element::get_value(oCodec, "array[2]").asInt32() , 0);
    ASSERT_EQ(access_element::get_value(oCodec, "after").asInt32() , 0);

    ASSERT_EQ(a_util::result::SUCCESS, oCodec.setConstants());

    ASSERT_EQ(access_element::get_value(oCodec, "static").asInt32() , 1);
    ASSERT_EQ(access_element::get_value(oCodec, "array_size").asInt32() , 3);
    ASSERT_EQ(access_element::get_value(oCodec, "array[0]").asInt32() , 1);
    ASSERT_EQ(access_element::get_value(oCodec, "array[1]").asInt32() , 1);
    ASSERT_EQ(access_element::get_value(oCodec, "array[2]").asInt32() , 1);
    ASSERT_EQ(access_element::get_value(oCodec, "after").asInt32() , 2);
}

namespace all_types
{

#ifdef WIN32
#define ALIGNED_ELEM(__align, __def) \
    __declspec(align(__align)) __def;
#else
#define ALIGNED_ELEM(__align, __def)\
    __def __attribute__((aligned(__align)));
#endif

    struct tMain
    {
        ALIGNED_ELEM(1, bool bBool)
            ALIGNED_ELEM(1, int8_t nInt8)
            ALIGNED_ELEM(1, uint8_t nUInt8)
            ALIGNED_ELEM(2, int16_t nInt16)
            ALIGNED_ELEM(2, uint16_t nUInt16)
            ALIGNED_ELEM(4, int32_t nInt32)
            ALIGNED_ELEM(4, uint32_t nUInt32)
            ALIGNED_ELEM(8, int64_t nInt64)
            ALIGNED_ELEM(8, uint64_t nUInt64)
            ALIGNED_ELEM(4, float fFloat32)
            ALIGNED_ELEM(8, double fFloat64)
            ALIGNED_ELEM(1, char nChar)
    };

    const char* strTestDesc =
        "<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>"
        "<struct alignment=\"4\" name=\"main\" version=\"2\">"
        "<element alignment=\"1\" arraysize=\"1\" byteorder=\"BE\" bitpos=\"1\" bytepos=\"0\" name=\"bBool\" type=\"tBool\"/>"
        "<element alignment=\"1\" arraysize=\"1\" byteorder=\"BE\" bitpos=\"1\" bytepos=\"1\" name=\"nInt8\" type=\"tInt8\"/>"
        "<element alignment=\"1\" arraysize=\"1\" byteorder=\"BE\" bitpos=\"1\" bytepos=\"2\" name=\"nUInt8\" type=\"tUInt8\"/>"
        "<element alignment=\"2\" arraysize=\"1\" byteorder=\"BE\" bitpos=\"1\" bytepos=\"3\" name=\"nInt16\" type=\"tInt16\"/>"
        "<element alignment=\"2\" arraysize=\"1\" byteorder=\"BE\" bitpos=\"1\" bytepos=\"5\" name=\"nUInt16\" type=\"tUInt16\"/>"
        "<element alignment=\"4\" arraysize=\"1\" byteorder=\"BE\" bitpos=\"1\" bytepos=\"7\" name=\"nInt32\" type=\"tInt32\"/>"
        "<element alignment=\"4\" arraysize=\"1\" byteorder=\"BE\" bitpos=\"1\" bytepos=\"11\" name=\"nUInt32\" type=\"tUInt32\"/>"
        "<element alignment=\"8\" arraysize=\"1\" byteorder=\"BE\" bitpos=\"1\" bytepos=\"15\" name=\"nInt64\" type=\"tInt64\"/>"
        "<element alignment=\"8\" arraysize=\"1\" byteorder=\"BE\" bitpos=\"1\" bytepos=\"23\" name=\"nUInt64\" type=\"tUInt64\"/>"
        "<element alignment=\"4\" arraysize=\"1\" byteorder=\"BE\" bitpos=\"1\" bytepos=\"31\" name=\"fFloat32\" type=\"tFloat32\"/>"
        "<element alignment=\"8\" arraysize=\"1\" byteorder=\"BE\" bitpos=\"1\" bytepos=\"35\" name=\"fFloat64\" type=\"tFloat64\"/>"
        "<element alignment=\"1\" arraysize=\"1\" byteorder=\"BE\" bitpos=\"1\" bytepos=\"43\" name=\"nChar\" type=\"tChar\"/>"
        "</struct>";

    const tMain sTestData =
    {
        true,
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        (float)3.1415,
        2.7182,
        'x'
    };

}

/**
* @detail  Check decoder functionalities for all types
*/
TEST(CodecTest,
    TestAllTypes)
{
    TEST_REQ("ACORE-7704 CDPKGDDL-12 CDPKGDDL-13");

    using namespace all_types;

    CodecFactory oFactory("main", strTestDesc);
    ASSERT_EQ(a_util::result::SUCCESS, oFactory.isValid());

    Decoder oDecoder = oFactory.makeDecoderFor(&sTestData, sizeof(sTestData));
    a_util::memory::MemoryBuffer oSerializedBuffer;
    ASSERT_EQ(a_util::result::SUCCESS, serialization::transform_to_buffer(oDecoder, oSerializedBuffer, true));

    oDecoder = oFactory.makeDecoderFor(oSerializedBuffer.getPtr(), oSerializedBuffer.getSize(),
        serialized);
    a_util::memory::MemoryBuffer oBuffer;
    ASSERT_EQ(a_util::result::SUCCESS, serialization::transform_to_buffer(oDecoder, oBuffer, true));

    const tMain* pRes = reinterpret_cast<tMain*>(oBuffer.getPtr());
    ASSERT_EQ(pRes->bBool , sTestData.bBool);
    ASSERT_EQ(pRes->nInt8 , sTestData.nInt8);
    ASSERT_EQ(pRes->nUInt8 , sTestData.nUInt8);
    ASSERT_EQ(pRes->nInt16 , sTestData.nInt16);
    ASSERT_EQ(pRes->nUInt16 , sTestData.nUInt16);
    ASSERT_EQ(pRes->nInt32 , sTestData.nInt32);
    ASSERT_EQ(pRes->nUInt32 , sTestData.nUInt32);
    ASSERT_EQ(pRes->nInt64 , sTestData.nInt64);
    ASSERT_EQ(pRes->nUInt64 , sTestData.nUInt64);
    ASSERT_EQ(pRes->fFloat32 , sTestData.fFloat32);
    ASSERT_EQ(pRes->fFloat64 , sTestData.fFloat64);
    ASSERT_EQ(pRes->nChar , sTestData.nChar);
}

namespace alignment_of_substructs
{

#pragma pack(push, 2)
    struct tSubStruct
    {
        int32_t nValue1;
        int32_t nValue2;
    };

#pragma pack(pop)

#pragma pack(push, 1)
    struct tMain
    {
        int8_t nPadding;
        tSubStruct aSubStructs[4];
    };
#pragma pack(pop)

    const char* strTestDesc =
        "<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>"
        "<structs>"
        "<struct alignment=\"2\" name=\"tSubStruct\" version=\"2\">"
        "<element alignment=\"2\" arraysize=\"1\" byteorder=\"BE\" bitpos=\"1\" bytepos=\"0\" name=\"nValue1\" type=\"tInt32\"/>"
        "<element alignment=\"2\" arraysize=\"1\" byteorder=\"BE\" bitpos=\"1\" bytepos=\"0\" name=\"nValue2\" type=\"tInt32\"/>"
        "</struct>"
        "<struct alignment=\"1\" name=\"tMain\" version=\"2\">"
        "<element alignment=\"1\" arraysize=\"1\" byteorder=\"BE\" bitpos=\"1\" bytepos=\"0\" name=\"nPadding\" type=\"tInt8\"/>"
        "<element alignment=\"1\" arraysize=\"4\" byteorder=\"BE\" bitpos=\"1\" bytepos=\"0\" name=\"aSubStructs\" type=\"tSubStruct\"/>"
        "</struct>"
        "</structs>";

    const tMain sTestData =
    {
        0,
        {
            { 1, 2 },
            { 3, 4 },
            { 5, 6 },
            { 7, 8 }
        }
    };

}

/**
* @detail  Check alignment of substructures
*/
TEST(CodecTest,
    TestAlignmentOfSubstructs)
{
    TEST_REQ("CDDDL-54");

    using namespace alignment_of_substructs;

    CodecFactory oFactory("tMain", strTestDesc);
    ASSERT_EQ(a_util::result::SUCCESS, oFactory.isValid());
    ASSERT_EQ(oFactory.getStaticBufferSize(), sizeof(tMain));

    Decoder oDecoder = oFactory.makeDecoderFor(&sTestData, sizeof(sTestData));
    DumpElements(oDecoder);

    ASSERT_EQ(access_element::get_value(oDecoder, "aSubStructs[0].nValue1").asInt8(), sTestData.aSubStructs[0].nValue1);
    ASSERT_EQ(access_element::get_value(oDecoder, "aSubStructs[0].nValue2").asInt8(), sTestData.aSubStructs[0].nValue2);

    ASSERT_EQ(access_element::get_value(oDecoder, "aSubStructs[1].nValue1").asInt8(), sTestData.aSubStructs[1].nValue1);
    ASSERT_EQ(access_element::get_value(oDecoder, "aSubStructs[1].nValue2").asInt8(), sTestData.aSubStructs[1].nValue2);

    ASSERT_EQ(access_element::get_value(oDecoder, "aSubStructs[2].nValue1").asInt8(), sTestData.aSubStructs[2].nValue1);
    ASSERT_EQ(access_element::get_value(oDecoder, "aSubStructs[2].nValue2").asInt8(), sTestData.aSubStructs[2].nValue2);

    ASSERT_EQ(access_element::get_value(oDecoder, "aSubStructs[3].nValue1").asInt8(), sTestData.aSubStructs[3].nValue1);
    ASSERT_EQ(access_element::get_value(oDecoder, "aSubStructs[3].nValue2").asInt8(), sTestData.aSubStructs[3].nValue2);
}

void TestError(const CodecFactory& oFactory, void* pData, size_t nDataSize, size_t nIndex, bool bValid)
{
    {
        StaticDecoder oTest = oFactory.makeStaticDecoderFor(pData, nDataSize);
        if (bValid)
        {
            ASSERT_EQ(a_util::result::SUCCESS, oTest.isValid());
        }
        else
        {
            ASSERT_NE(a_util::result::SUCCESS, oTest.isValid());
        }
        uint64_t* nBuffer;
        ASSERT_NE(a_util::result::SUCCESS, oTest.getElementValue(10, &nBuffer));
        a_util::variant::Variant oValue;
        ASSERT_NE(a_util::result::SUCCESS, oTest.getElementValue(10, oValue));
    }

    {
        Decoder oTest = oFactory.makeDecoderFor(pData, nDataSize);
        if (bValid)
        {
            ASSERT_EQ(a_util::result::SUCCESS, oTest.isValid());
        }
        else
        {
            ASSERT_NE(a_util::result::SUCCESS, oTest.isValid());
        }
        uint64_t* nBuffer;
        ASSERT_NE(a_util::result::SUCCESS, oTest.getElementValue(10, &nBuffer));
        a_util::variant::Variant oValue;
        ASSERT_NE(a_util::result::SUCCESS, oTest.getElementValue(10, oValue));
    }

    {
        StaticCodec oTest = oFactory.makeStaticCodecFor(pData, nDataSize);
        if (bValid)
        {
            ASSERT_EQ(a_util::result::SUCCESS, oTest.isValid());
        }
        else
        {
            ASSERT_NE(a_util::result::SUCCESS, oTest.isValid());
        }
        uint64_t* nBuffer;
        ASSERT_NE(a_util::result::SUCCESS, oTest.setElementValue(10, &nBuffer));
        a_util::variant::Variant oValue;
        ASSERT_NE(a_util::result::SUCCESS, oTest.setElementValue(10, oValue));
    }

    {
        Codec oTest = oFactory.makeCodecFor(pData, nDataSize);
        if (bValid)
        {
            ASSERT_EQ(a_util::result::SUCCESS, oTest.isValid());
        }
        else
        {
            ASSERT_NE(a_util::result::SUCCESS, oTest.isValid());
        }
        uint64_t* nBuffer;
        ASSERT_NE(a_util::result::SUCCESS, oTest.setElementValue(10, &nBuffer));
        a_util::variant::Variant oValue;
        ASSERT_NE(a_util::result::SUCCESS, oTest.setElementValue(10, oValue));
    }
}

/**
* @detail  Test error handling in decoder
*/
TEST(CodecTest,
    TestErrorHandling)
{
    TEST_REQ("ACORE-7704 ACORE-8004");
    using namespace static_struct;

    // invalid index
    {
        CodecFactory oFactory = CodecFactory("test", strTestDesc);
        ASSERT_EQ(a_util::result::SUCCESS, oFactory.isValid());
        tTest sTest;
        TestError(oFactory, &sTest, sizeof(sTest), 10, true);
    }

    // invalid data
    {
        CodecFactory oFactory = CodecFactory("test", strTestDesc);
        ASSERT_EQ(a_util::result::SUCCESS, oFactory.isValid());
        TestError(oFactory, NULL, 0, 0, false);
    }

    // invalid struct name
    {
        CodecFactory oFactory = CodecFactory("", strTestDesc);
        ASSERT_NE(a_util::result::SUCCESS, oFactory.isValid());
        tTest sTest;
        TestError(oFactory, &sTest, sizeof(sTest), 0, false);
    }

    // invalid description
    {
        CodecFactory oFactory = CodecFactory("test", "");
        ASSERT_NE(a_util::result::SUCCESS, oFactory.isValid());
        tTest sTest;
        TestError(oFactory, &sTest, sizeof(sTest), 0, false);
    }
}


