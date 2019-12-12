/**
 * @file
 * Bitserializer Test implementation.
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

#include <gtest/gtest.h>
#include "../../_common/adtf_compat.h"
#include "../../_common/compat.h"
#include "codec/bitserializer.h"

using namespace a_util::memory;

/**
* @detail  Read values using the bit serializer
*/
TEST(CodecTest,
    BitSerializerTestReadValue)
{
    TEST_REQ("");

    uint8_t aui8Buffer[10] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x80 };

    BitSerializer *pBits = new BitSerializer(aui8Buffer, 10);

    uint64_t value = 0;


    // Read complete aligned bytes

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 64, &value, bit_little_endian));

    ASSERT_TRUE(value == 0x7766554433221100ULL);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 56, &value, bit_little_endian));

    ASSERT_TRUE(value == 0x0066554433221100ULL);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 32, &value, bit_little_endian));

    ASSERT_TRUE(value == 0x33221100);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 24, &value, bit_little_endian));

    ASSERT_TRUE(value == 0x00221100);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 16, &value, bit_little_endian));

    ASSERT_TRUE(value == 0x1100);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 8, &value, bit_little_endian));

    ASSERT_TRUE(value == 0x00);



    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 64, &value, bit_big_endian));

    ASSERT_TRUE(value == 0x0011223344556677ULL);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 56, &value, bit_big_endian));

    ASSERT_TRUE(value == 0x0000112233445566ULL);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 32, &value, bit_big_endian));

    ASSERT_TRUE(value == 0x00112233);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 24, &value, bit_big_endian));

    ASSERT_TRUE(value == 0x00001122);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 16, &value, bit_big_endian));

    ASSERT_TRUE(value == 0x0011);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 8, &value, bit_big_endian));

    ASSERT_TRUE(value == 0x00);



    // Read complete unaligned bytes

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(16, 64, &value, bit_little_endian));

    ASSERT_TRUE(value == 0x8088776655443322ULL);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(4, 16, &value, bit_little_endian));

    ASSERT_TRUE(value == 0x2110);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(40, 16, &value, bit_little_endian));

    ASSERT_TRUE(value == 0x6655);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(16, 16, &value, bit_little_endian));

    ASSERT_TRUE(value == 0x3322);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(12, 64, &value, bit_little_endian)); // Read value from 9 buffer bytes

    ASSERT_TRUE(value == 0x0887766554433221ULL);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(4, 64, &value, bit_little_endian)); // Read value from 9 buffer bytes

    ASSERT_TRUE(value == 0x8776655443322110ULL);



    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(4, 16, &value, bit_big_endian));

    ASSERT_TRUE(value == 0x1021);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(40, 16, &value, bit_big_endian));

    ASSERT_TRUE(value == 0x5566);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(16, 16, &value, bit_big_endian));

    ASSERT_TRUE(value == 0x2233);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(4, 64, &value, bit_big_endian)); // Read value from 9 buffer bytes

    ASSERT_TRUE(value == 0x1021324354657687ULL);



    // Read partial unaligned bytes

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(4, 13, &value, bit_little_endian));

    ASSERT_TRUE(value == 0x0110);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(40, 17, &value, bit_little_endian));

    ASSERT_TRUE(value == 0x00016655);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(11, 10, &value, bit_little_endian));

    ASSERT_TRUE(value == 0x0042);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(1, 61, &value, bit_little_endian));

    ASSERT_TRUE(value == 0x1BB32AA219910880ULL);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(4, 63, &value, bit_little_endian)); // Read value from 9 buffer bytes

    ASSERT_TRUE(value == 0x0776655443322110ULL);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(12, 61, &value, bit_little_endian)); // Read value from 9 buffer bytes

    ASSERT_TRUE(value == 0x0887766554433221ULL);



    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(4, 13, &value, bit_big_endian));

    ASSERT_TRUE(value == 0x1008);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(40, 17, &value, bit_big_endian));

    ASSERT_TRUE(value == 0x00012ab3);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(10, 10, &value, bit_big_endian));

    ASSERT_TRUE(value == 0x0021);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(5, 61, &value, bit_big_endian)); // Read value from 9 buffer bytes

    ASSERT_TRUE(value == 0x0884C80C5195D91DULL);

    delete pBits;

}


/**
* @detail  Write values using the bit serializer
*/
TEST(CodecTest,
    BitSerializerTestWriteValue)
{
    TEST_REQ("");

    uint8_t aui8Buffer[10] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x80 };

    BitSerializer *pBits = new BitSerializer(&aui8Buffer, 10);



    // Test LE Signal Writing

    uint64_t value = 0x1101;

    uint64_t result = 0;



    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(0, 10, value, bit_little_endian));

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 10, &result, bit_little_endian));

    ASSERT_TRUE(0x0101 == result);



    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(10, 10, value, bit_little_endian));

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(10, 10, &result, bit_little_endian));

    ASSERT_TRUE(0x0101 == result);



    // Write and then read adjacent values inside buffer

    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(0, 14, value, bit_little_endian));

    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(14, 16, value, bit_little_endian));

    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(30, 16, value, bit_little_endian));



    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 14, &result, bit_little_endian));

    ASSERT_TRUE(value == result);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(14, 16, &result, bit_little_endian));

    ASSERT_TRUE(value == result);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(30, 16, &result, bit_little_endian));

    ASSERT_TRUE(value == result);



    value = 0x1122334455667788ULL;



    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(0, 64, value, bit_little_endian));

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 64, &result, bit_little_endian));

    ASSERT_TRUE(0x1122334455667788ULL == result);



    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(15, 64, value, bit_little_endian)); // Write value into 9 buffer bytes

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(15, 64, &result, bit_little_endian));

    ASSERT_TRUE(0x1122334455667788ULL == result);





    // Test BE Signal Writing

    value = 0x1101;



    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(0, 10, value, bit_big_endian));

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 10, &result, bit_big_endian));

    ASSERT_TRUE(0x0101 == result);



    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(10, 10, value, bit_big_endian));

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(10, 10, &result, bit_big_endian));

    ASSERT_TRUE(0x0101 == result);



    // Write and then read adjacent values inside buffer

    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(0, 14, value, bit_big_endian));

    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(14, 16, value, bit_big_endian));

    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(30, 16, value, bit_big_endian));



    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 14, &result, bit_big_endian));

    ASSERT_TRUE(value == result);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(14, 16, &result, bit_big_endian));

    ASSERT_TRUE(value == result);

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(30, 16, &result, bit_big_endian));

    ASSERT_TRUE(value == result);



    value = 0x1122334455667788ULL;



    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(0, 64, value, bit_big_endian));

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 64, &result, bit_big_endian));

    ASSERT_TRUE(0x1122334455667788ULL == result);



    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(15, 64, value, bit_big_endian)); // Write value into 9 buffer bytes

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(15, 64, &result, bit_big_endian));

    ASSERT_TRUE(0x1122334455667788ULL == result);



    // Test Float values

    double valuef = 846.73648f;

    double resultf = 0.0f;

    double result_other_endianf = 0.0f;

    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(0, 64, valuef, bit_little_endian));

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 64, &resultf, bit_little_endian));

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 64, &result_other_endianf, bit_big_endian));

    ASSERT_TRUE(valuef == resultf);

    ASSERT_TRUE(valuef != result_other_endianf);



    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(15, 64, valuef, bit_little_endian)); // Write value into 9 buffer bytes

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(15, 64, &resultf, bit_little_endian));

    ASSERT_TRUE(valuef == resultf);



    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(0, 64, valuef, bit_big_endian));

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 64, &resultf, bit_big_endian));

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 64, &result_other_endianf, bit_little_endian));

    ASSERT_TRUE(valuef == resultf);

    ASSERT_TRUE(valuef != result_other_endianf);



    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(15, 64, valuef, bit_big_endian)); // Write value into 9 buffer bytes

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(15, 64, &resultf, bit_big_endian));

    ASSERT_TRUE(valuef == resultf);



    // Test Signed values

    int64_t sValue = -45872ll;

    int64_t sResult = 0;



    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(0, 62, sValue, bit_little_endian));

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 62, &sResult, bit_little_endian));


    ASSERT_TRUE(sValue == sResult);



    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(15, 64, sValue, bit_little_endian)); // Write value into 9 buffer bytes

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(15, 64, &sResult, bit_little_endian));


    ASSERT_TRUE(sValue == sResult);



    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(0, 62, sValue, bit_big_endian));

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 62, &sResult, bit_big_endian));

    ASSERT_TRUE(sValue == sResult);



    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(15, 64, sValue, bit_big_endian)); // Write value into 9 buffer bytes

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(15, 64, &sResult, bit_big_endian));

    ASSERT_TRUE(sValue == sResult);



    int16_t sValue2 = -358ll;

    int16_t sResult2 = 0;



    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(0, 10, sValue2, bit_little_endian));

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 10, &sResult2, bit_little_endian));

    ASSERT_TRUE(sValue2 == sResult2);



    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(15, 10, sValue2, bit_little_endian));

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(15, 10, &sResult2, bit_little_endian));

    ASSERT_TRUE(sValue2 == sResult2);



    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(0, 10, sValue2, bit_big_endian));

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(0, 10, &sResult2, bit_big_endian));

    ASSERT_TRUE(sValue2 == sResult2);



    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(15, 10, sValue2, bit_big_endian));

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(15, 10, &sResult2, bit_big_endian));

    ASSERT_TRUE(sValue2 == sResult2);



    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(17, 13, sValue2, bit_little_endian));

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(17, 13, &sResult2, bit_little_endian));

    ASSERT_TRUE(sValue2 == sResult2);



    ASSERT_EQ(a_util::result::SUCCESS, pBits->write(17, 13, sValue2, bit_big_endian));

    ASSERT_EQ(a_util::result::SUCCESS, pBits->read(17, 13, &sResult2, bit_big_endian));

    ASSERT_TRUE(sValue2 == sResult2);

}