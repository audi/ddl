/**
 * @file
 * Headerfile for the tester for the DDL representation.
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

#ifndef _TESTER_DDL_DEFINITIONS_HEADER_
#define _TESTER_DDL_DEFINITIONS_HEADER_

static const std::string DDL_TEST_STRING =
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
</adtf:ddl>";

// Test DDL that differs in the type of tBitmapFormat::nBytesPerLine
static const std::string DDL_TEST_STRING_DIFF =
"<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\n \
<adtf:ddl xmlns:adtf=\"adtf\">\n \
 <structs>\n \
     <!-- TYPE DESCRIPTIONS-->\n \
     <struct alignment=\"1\" name=\"tMediaTypeInfo\" version=\"1\">\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"ui32MajorType\" type=\"tUInt32\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"4\" name=\"ui32SubType\" type=\"tUInt32\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"8\" name=\"ui32Flags\" type=\"tUInt32\" />\n \
     </struct>\n \
     <struct alignment=\"1\" name=\"tBitmapFormat\" version=\"1\">\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"32\" name=\"nWidth\" type=\"tInt32\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"18\" name=\"nHeight\" type=\"tInt32\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"12\" name=\"nBitsPerPixel\" type=\"tInt16\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"26\" name=\"nPixelFormat\" type=\"tInt16\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"14\" name=\"nBytesPerLine\" type=\"tFloat32\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"28\" name=\"nSize\" type=\"tInt32\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"22\" name=\"nPaletteSize\" type=\"tInt32\" />\n \
     </struct>\n \
     <struct name=\"adtf.type.video\" version=\"1\">\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"sMediatype\" type=\"tMediaTypeInfo\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"12\" name=\"sBitmapFormat\" type=\"tBitmapFormat\" />\n \
     </struct>\n \
 </structs>\n \
</adtf:ddl>";

// Test DDL that differs by names
static const std::string DDL_TEST_STRING_DIFF_NAMES =
"<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\n \
<adtf:ddl xmlns:adtf=\"adtf\">\n \
 <structs>\n \
     <!-- TYPE DESCRIPTIONS-->\n \
     <struct alignment=\"1\" name=\"tMediaTypeInfo2\" version=\"1\">\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"ui32MajorType\" type=\"tUInt32\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"4\" name=\"ui32SubType\" type=\"tUInt32\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"8\" name=\"ui32Flags\" type=\"tUInt32\" />\n \
     </struct>\n \
     <struct alignment=\"1\" name=\"tBitmapFormat\" version=\"1\">\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"32\" name=\"nWidth\" type=\"tInt32\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"18\" name=\"nHeight\" type=\"tInt32\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"12\" name=\"nBitsPerPixel\" type=\"tInt16\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"26\" name=\"nPixelFormat\" type=\"tInt16\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"14\" name=\"nBytesPerLine34\" type=\"tInt32\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"28\" name=\"nSize\" type=\"tInt32\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"22\" name=\"nPaletteSize\" type=\"tInt32\" />\n \
     </struct>\n \
     <struct name=\"adtf.type.video\" version=\"1\">\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"sMediatype\" type=\"tMediaTypeInfo2\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"12\" name=\"sBitmapFormat\" type=\"tBitmapFormat\" />\n \
     </struct>\n \
 </structs>\n \
</adtf:ddl>";


// Test DDL that differs by version
static const std::string DDL_TEST_STRING_DIFF_VERSION =
"<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\n \
<adtf:ddl xmlns:adtf=\"adtf\">\n \
 <structs>\n \
     <!-- TYPE DESCRIPTIONS-->\n \
     <struct alignment=\"1\" name=\"tMediaTypeInfo2\" version=\"3\">\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"ui32MajorType\" type=\"tUInt32\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"4\" name=\"ui32SubType\" type=\"tUInt32\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"8\" name=\"ui32Flags\" type=\"tUInt32\" />\n \
     </struct>\n \
     <struct alignment=\"1\" name=\"tBitmapFormat\" version=\"3\">\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"32\" name=\"nWidth\" type=\"tInt32\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"18\" name=\"nHeight\" type=\"tInt32\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"12\" name=\"nBitsPerPixel\" type=\"tInt16\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"26\" name=\"nPixelFormat\" type=\"tInt16\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"14\" name=\"nBytesPerLine34\" type=\"tInt32\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"28\" name=\"nSize\" type=\"tInt32\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"22\" name=\"nPaletteSize\" type=\"tInt32\" />\n \
     </struct>\n \
     <struct name=\"adtf.type.video\" version=\"3\">\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"sMediatype\" type=\"tMediaTypeInfo2\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"12\" name=\"sBitmapFormat\" type=\"tBitmapFormat\" />\n \
     </struct>\n \
 </structs>\n \
</adtf:ddl>";

// Test DDL that differs by version
static const std::string DDL_TEST_STRING_DIFF_ARRAY =
"<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\n \
<adtf:ddl xmlns:adtf=\"adtf\">\n \
 <structs>\n \
     <!-- TYPE DESCRIPTIONS-->\n \
     <struct alignment=\"1\" name=\"tMediaTypeInfo2\" version=\"3\">\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"ui32MajorType\" type=\"tUInt32\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"4\" name=\"ui32SubType\" type=\"tUInt32\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"8\" name=\"ui32Flags\" type=\"tUInt32\" />\n \
     </struct>\n \
     <struct alignment=\"1\" name=\"tBitmapFormat\" version=\"3\">\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"32\" name=\"nWidth\" type=\"tInt32\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"18\" name=\"nHeight\" type=\"tInt32\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"12\" name=\"nBitsPerPixel\" type=\"tInt16\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"26\" name=\"nPixelFormat\" type=\"tInt16\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"14\" name=\"nBytesPerLine34\" type=\"tInt32\" />\n \
         <element alignment=\"1\" arraysize=\"100\" byteorder=\"LE\" bytepos=\"28\" name=\"nSize\" type=\"tInt32\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"22\" name=\"nPaletteSize\" type=\"tInt32\" />\n \
     </struct>\n \
     <struct name=\"adtf.type.video\" version=\"3\">\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"sMediatype\" type=\"tMediaTypeInfo2\" />\n \
         <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"12\" name=\"sBitmapFormat\" type=\"tBitmapFormat\" />\n \
     </struct>\n \
 </structs>\n \
</adtf:ddl>";


// Test DDL where tBitmapFormat is only a subset
static const std::string DDL_TEST_STRING_SUBSET =
   "<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\n \
   <adtf:ddl xmlns:adtf=\"adtf\">\n \
       <structs>\n \
           <!-- TYPE DESCRIPTIONS-->\n \
           <struct alignment=\"1\" name=\"tMediaTypeInfo\" version=\"1\">\n \
               <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"ui32MajorType\" type=\"tUInt32\" />\n \
               <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"4\" name=\"ui32SubType\" type=\"tUInt32\" />\n \
               <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"8\" name=\"ui32Flags\" type=\"tUInt32\" />\n \
           </struct>\n \
           <struct alignment=\"1\" name=\"tBitmapFormat\" version=\"1\">\n \
               <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"32\" name=\"nWidth\" type=\"tInt32\" />\n \
               <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"18\" name=\"nHeight\" type=\"tInt32\" />\n \
               <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"12\" name=\"nBitsPerPixel\" type=\"tInt16\" />\n \
               <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"26\" name=\"nPixelFormat\" type=\"tInt16\" />\n \
           </struct>\n \
           <struct name=\"adtf.type.video\" version=\"1\">\n \
               <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"sMediatype\" type=\"tMediaTypeInfo\" />\n \
               <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"12\" name=\"sBitmapFormat\" type=\"tBitmapFormat\" />\n \
           </struct>\n \
       </structs>\n \
   </adtf:ddl>";


// Test DDL where tMediaTypInfo is one element short
static const std::string DDL_TEST_STRING_INVALID_SUBSET =
  "<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\n \
  <adtf:ddl xmlns:adtf=\"adtf\">\n \
      <structs>\n \
          <!-- TYPE DESCRIPTIONS-->\n \
          <struct alignment=\"1\" name=\"tMediaTypeInfo\" version=\"1\">\n \
              <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"ui32MajorType\" type=\"tUInt32\" />\n \
              <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"4\" name=\"ui32SubType\" type=\"tUInt32\" />\n \
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
          <struct name=\"adtf.type.video\" version=\"1\">\n \
              <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"sMediatype\" type=\"tMediaTypeInfo\" />\n \
              <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"12\" name=\"sBitmapFormat\" type=\"tBitmapFormat\" />\n \
          </struct>\n \
      </structs>\n \
  </adtf:ddl>";

// Test DDL with dynamic content
static const std::string DDL_TEST_STRING_DYNAMIC =
 "<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\n \
 <adtf:ddl xmlns:adtf=\"adtf\">\n \
     <structs>\n \
         <!-- TYPE DESCRIPTIONS-->\n \
         <struct alignment=\"1\" name=\"tMediaTypeInfo\" version=\"1\">\n \
             <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"ui32MajorType\" type=\"tUInt32\" />\n \
             <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"4\" name=\"ui32SubType\" type=\"tUInt32\" />\n \
             <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"8\" name=\"ui32Flags\" type=\"tUInt32\" />\n \
         </struct>\n \
         <struct alignment=\"1\" name=\"tBitmapFormat\" version=\"1\">\n \
             <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"32\" name=\"nWidth\" type=\"tInt32\" />\n \
             <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"18\" name=\"nHeight\" type=\"tInt32\" />\n \
             <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"12\" name=\"nBitsPerPixel\" type=\"tInt16\" />\n \
             <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"26\" name=\"nPixelFormat\" type=\"tInt16\" />\n \
         </struct>\n \
         <struct name=\"adtf.type.video\" version=\"1\">\n \
             <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"nCount\" type=\"tUInt32\" />\n \
             <element alignment=\"1\" arraysize=\"nCount\" byteorder=\"LE\" bytepos=\"0\" name=\"sMediatype\" type=\"tMediaTypeInfo\" />\n \
             <element alignment=\"1\" arraysize=\"nCount\" byteorder=\"LE\" bytepos=\"12\" name=\"sBitmapFormat\" type=\"tBitmapFormat\" />\n \
         </struct>\n \
     </structs>\n \
 </adtf:ddl>";

// Test DDL with dynamic content but with different naming
static const std::string DDL_TEST_STRING_DYNAMIC_NAME_DIFF =
     "<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\n \
     <adtf:ddl xmlns:adtf=\"adtf\">\n \
         <structs>\n \
             <!-- TYPE DESCRIPTIONS-->\n \
             <struct alignment=\"1\" name=\"tMediaTypeInfo2\" version=\"1\">\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"ui32MajorType2\" type=\"tUInt32\" />\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"4\" name=\"ui32SubType2\" type=\"tUInt32\" />\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"8\" name=\"ui32Flags2\" type=\"tUInt32\" />\n \
             </struct>\n \
             <struct alignment=\"1\" name=\"tBitmapFormat\" version=\"1\">\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"32\" name=\"nWidth\" type=\"tInt32\" />\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"18\" name=\"nHeight\" type=\"tInt32\" />\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"12\" name=\"nBitsPerPixel\" type=\"tInt16\" />\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"26\" name=\"nPixelFormat\" type=\"tInt16\" />\n \
             </struct>\n \
             <struct name=\"adtf.type.video\" version=\"1\">\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"nCount\" type=\"tUInt32\" />\n \
                 <element alignment=\"1\" arraysize=\"nCount\" byteorder=\"LE\" bytepos=\"0\" name=\"sMediatype\" type=\"tMediaTypeInfo2\" />\n \
                 <element alignment=\"1\" arraysize=\"nCount\" byteorder=\"LE\" bytepos=\"12\" name=\"sBitmapFormat\" type=\"tBitmapFormat\" />\n \
             </struct>\n \
         </structs>\n \
     </adtf:ddl>";

// Test DDL with dynamic content that is not binary equal (tBitmapFormat::nPixelFormat2)
static const std::string DDL_TEST_STRING_DYNAMIC_BIN_DIFF =
     "<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\n \
     <adtf:ddl xmlns:adtf=\"adtf\">\n \
         <structs>\n \
             <!-- TYPE DESCRIPTIONS-->\n \
             <struct alignment=\"1\" name=\"tMediaTypeInfo\" version=\"1\">\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"ui32MajorType\" type=\"tUInt32\" />\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"4\" name=\"ui32SubType\" type=\"tUInt32\" />\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"8\" name=\"ui32Flags\" type=\"tUInt32\" />\n \
             </struct>\n \
             <struct alignment=\"1\" name=\"tBitmapFormat\" version=\"1\">\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"32\" name=\"nWidth\" type=\"tInt32\" />\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"18\" name=\"nHeight\" type=\"tInt32\" />\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"12\" name=\"nBitsPerPixel\" type=\"tInt16\" />\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"26\" name=\"nPixelFormat\" type=\"tInt16\" />\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"26\" name=\"nPixelFormat2\" type=\"tInt16\" />\n \
             </struct>\n \
             <struct name=\"adtf.type.video\" version=\"1\">\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"nCount\" type=\"tUInt32\" />\n \
                 <element alignment=\"1\" arraysize=\"nCount\" byteorder=\"LE\" bytepos=\"0\" name=\"sMediatype\" type=\"tMediaTypeInfo\" />\n \
                 <element alignment=\"1\" arraysize=\"nCount\" byteorder=\"LE\" bytepos=\"12\" name=\"sBitmapFormat\" type=\"tBitmapFormat\" />\n \
             </struct>\n \
         </structs>\n \
     </adtf:ddl>";

// Test DDL with dynamic content that is not binary equal in dynamic part (tMediaTypeInfo::ui32Flags has different type)
static const std::string DDL_TEST_STRING_DYNAMIC_BIN_DIFF_2 =
     "<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\n \
     <adtf:ddl xmlns:adtf=\"adtf\">\n \
         <structs>\n \
             <!-- TYPE DESCRIPTIONS-->\n \
             <struct alignment=\"1\" name=\"tMediaTypeInfo\" version=\"1\">\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"ui32MajorType\" type=\"tUInt32\" />\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"4\" name=\"ui32SubType\" type=\"tUInt32\" />\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"8\" name=\"ui32Flags\" type=\"tUInt64\" />\n \
             </struct>\n \
             <struct alignment=\"1\" name=\"tBitmapFormat\" version=\"1\">\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"32\" name=\"nWidth\" type=\"tInt32\" />\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"18\" name=\"nHeight\" type=\"tInt32\" />\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"12\" name=\"nBitsPerPixel\" type=\"tInt16\" />\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"26\" name=\"nPixelFormat\" type=\"tInt16\" />\n \
             </struct>\n \
             <struct name=\"adtf.type.video\" version=\"1\">\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"nCount\" type=\"tUInt32\" />\n \
                 <element alignment=\"1\" arraysize=\"nCount\" byteorder=\"LE\" bytepos=\"0\" name=\"sMediatype\" type=\"tMediaTypeInfo\" />\n \
                 <element alignment=\"1\" arraysize=\"nCount\" byteorder=\"LE\" bytepos=\"12\" name=\"sBitmapFormat\" type=\"tBitmapFormat\" />\n \
             </struct>\n \
         </structs>\n \
     </adtf:ddl>";

// Test DDL to where the orignial struct layout is not the same (tTest1 vs tTest2) but the binary positions match
static const std::string DDL_TEST_STRING_LAYOUT_TEST =
     "<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\n \
     <adtf:ddl xmlns:adtf=\"adtf\">\n \
         <structs>\n \
             <!-- TYPE DESCRIPTIONS-->\n \
             <struct alignment=\"1\" name=\"sub1\" version=\"1\">\n \
                 <element alignment=\"1\" arraysize=\"3\" byteorder=\"LE\" bytepos=\"0\" name=\"ui32MajorType\" type=\"tUInt32\" />\n \
             </struct>\n \
             <struct alignment=\"1\" name=\"tTest1\" version=\"1\">\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"sSub1\" type=\"sub1\" />\n \
                 <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"sSub2\" type=\"sub1\" />\n \
             </struct>\n \
             <struct alignment=\"1\" name=\"tTest2\" version=\"1\">\n \
                 <element alignment=\"1\" arraysize=\"6\" byteorder=\"LE\" bytepos=\"0\" name=\"ui32MajorType\" type=\"tUInt32\" />\n \
             </struct>\n \
         </structs>\n \
     </adtf:ddl>";

// Test DDL to where the array size element is different
static const std::string DDL_TEST_STRING_DYNAMIC_DIFF_ARRAY_SPEC =
  "<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\n \
  <adtf:ddl xmlns:adtf=\"adtf\">\n \
      <structs>\n \
          <!-- TYPE DESCRIPTIONS-->\n \
          <struct alignment=\"1\" name=\"tTest1\" version=\"1\">\n \
              <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"nCount1\" type=\"tUInt32\" />\n \
              <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"nCount2\" type=\"tUInt32\" />\n \
              <element alignment=\"1\" arraysize=\"nCount1\" byteorder=\"LE\" bytepos=\"0\" name=\"aArray\" type=\"tUInt32\" />\n \
          </struct>\n \
          <struct alignment=\"1\" name=\"tTest2\" version=\"1\">\n \
              <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"nCount1\" type=\"tUInt32\" />\n \
              <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"nCount2\" type=\"tUInt32\" />\n \
              <element alignment=\"1\" arraysize=\"nCount2\" byteorder=\"LE\" bytepos=\"0\" name=\"aArray\" type=\"tUInt32\" />\n \
          </struct>\n \
      </structs>\n \
  </adtf:ddl>";


static const std::string DDL_TEST_STRING_UNITS =
"<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\n \
<adtf:ddl xmlns:adtf=\"adtf\">\n \
  <header>\n \
      <language_version>3.0</language_version>\n \
      <author>AUDI Electronics Venture GmbH</author>\n \
      <date_creation>20100407</date_creation>\n \
      <date_change />\n \
      <description>ADTF Common Description File</description>\n \
  </header>\n \
  <units> \
          <baseunit description=\"Fundamental unit for electric current\" name=\"Ampere\" symbol=\"A\" /> \
          <baseunit description=\"Fundamental unit for luminous intensity\" name=\"Candela\" symbol=\"cd\" /> \
          <baseunit description=\"Non-SI standard unit for angle\" name=\"Degree\" symbol=\"deg\" />  \
          <baseunit description=\"Fundamental unit for thermodynamic temperature\" name=\"Kelvin\" symbol=\"K\" /> \
          <baseunit description=\"Fundamental unit for mass\" name=\"Kilogram\" symbol=\"kg\" /> \
          <baseunit description=\"Fundamental unit for length\" name=\"Metre\" symbol=\"m\" /> \
          <baseunit description=\"Fundamental unit for amount of substance\" name=\"Mole\" symbol=\"mol\" /> \
          <baseunit description=\"Non-SI standard unit for angle\" name=\"Radiant\" symbol=\"rad\" /> \
          <baseunit description=\"Fundamental unit for time\" name=\"Second\" symbol=\"s\" /> \
          <baseunit description=\"No SI, but needed for own unit definitions\" name=\"Unitless\" symbol=\"\" /> \
   \
          <prefixes name=\"atto\" power=\"-18\" symbol=\"a\" /> \
          <prefixes name=\"centi\" power=\"-2\" symbol=\"c\" /> \
          <prefixes name=\"deca\" power=\"1\" symbol=\"da\" /> \
          <prefixes name=\"deci\" power=\"-1\" symbol=\"d\" /> \
          <prefixes name=\"exa\" power=\"18\" symbol=\"E\" /> \
          <prefixes name=\"femto\" power=\"-15\" symbol=\"f\" /> \
          <prefixes name=\"giga\" power=\"9\" symbol=\"G\" /> \
          <prefixes name=\"hecto\" power=\"2\" symbol=\"h\" /> \
          <prefixes name=\"kilo\" power=\"3\" symbol=\"k\" /> \
          <prefixes name=\"mega\" power=\"6\" symbol=\"M\" /> \
          <prefixes name=\"micro\" power=\"-6\" symbol=\"u\" /> \
          <prefixes name=\"milli\" power=\"-3\" symbol=\"m\" /> \
          <prefixes name=\"nano\" power=\"-9\" symbol=\"n\" /> \
          <prefixes name=\"peta\" power=\"15\" symbol=\"P\" /> \
          <prefixes name=\"pico\" power=\"-12\" symbol=\"p\" /> \
          <prefixes name=\"tera\" power=\"12\" symbol=\"T\" /> \
          <prefixes name=\"yocto\" power=\"-24\" symbol=\"y\" /> \
          <prefixes name=\"yotta\" power=\"24\" symbol=\"Y\" /> \
          <prefixes name=\"zepto\" power=\"-21\" symbol=\"z\" /> \
          <prefixes name=\"zetta\" power=\"21\" symbol=\"Z\" /> \
                          <unit name=\"bar_unit\"> \
                                      <numerator>1.0</numerator> \
                                      <denominator>1.0</denominator> \
                                      <offset>1.0</offset> \
                                      <refUnit name=\"Ampere\" power=\"10\" prefix=\"deca\" /> \
                         </unit> \
                         <unit name=\"bar_unit2\"> \
                                     <numerator>1.0</numerator> \
                                     <denominator>1.0</denominator> \
                                     <offset>1.0</offset> \
                                     <refUnit name=\"Ampere\" power=\"10\" prefix=\"deca\" /> \
                        </unit> \
      </units> \
  <datatypes /> \
  <structs /> \
  <streams /> \
</adtf:ddl>";

static const std::string DDL_TEST_STRING_ENUMS =
"<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\n \
<adtf:ddl xmlns:adtf=\"adtf\">\n \
  <header>\n \
      <language_version>3.0</language_version>\n \
      <author>AUDI Electronics Venture GmbH</author>\n \
      <date_creation>20100407</date_creation>\n \
      <date_change />\n \
      <description>ADTF Common Description File</description>\n \
  </header>\n \
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
  <enums> \
        <enum name=\"tValueDefinitions\" type=\"tUInt32\"> \
              <element name=\"ELEMENT_ONE\" value=\"10\"/> \
              <element name=\"ELEMENT_TWO\" value=\"20\"/> \
              <element name=\"ELEMENT_THREE\" value=\"5\"/> \
        </enum> \
         <enum name=\"tValueDefinitions2\" type=\"tUInt32\"> \
               <element name=\"ELEMENT_ONE2\" value=\"10\"/> \
               <element name=\"ELEMENT_TWO2\" value=\"20\"/> \
               <element name=\"ELEMENT_THREE2\" value=\"5\"/> \
         </enum> \
   </enums> \
  <structs /> \
  <streams /> \
</adtf:ddl>";

static const std::string DDL_TEST_STRING_HEADER =
"<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\n \
<adtf:ddl xmlns:adtf=\"adtf\">\n \
<header>\n \
    <language_version>3.0</language_version>\n \
    <author>AUDI Electronics Venture GmbH</author>\n \
    <date_creation>20100407</date_creation>\n \
    <date_change />\n \
    <description>ADTF Common Description File</description>\n \
    <ext_declaration key=\"AnyKey\" value=\"Any value for this key\"/> \
    <ext_declaration key=\"AnyKey2\" value=\"Any value for this key2\"/> \
</header>\n \
<datatypes /> \
<structs /> \
<streams /> \
</adtf:ddl>";


static const std::string DDL_TEST_STRING_USER_BASE_UNITS =
"<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\n \
<adtf:ddl xmlns:adtf=\"adtf\">\n \
    <header>\n \
        <language_version>3.0</language_version>\n \
        <author>AUDI Electronics Venture GmbH</author>\n \
        <date_creation>20100407</date_creation>\n \
        <date_change />\n \
        <description>ADTF Common Description File</description>\n \
    </header>\n \
    <units> \
        <baseunit description=\"test\" name=\"UNIT_HZ\" symbol=\"Hz\" /> \
    </units> \
        <datatypes>\n \
           <datatype description=\"predefined ADTF tUInt8 datatype\"  size=\"8\"   name=\"tUInt8\" />\n \
        </datatypes>\n \
    <structs> \
        <struct name=\"test_struct\" version=\"1\" > \
            <element name=\"test\" unit=\"UNIT_HZ\" alignment=\"1\" bytepos=\"0\" arraysize=\"1\" byteorder=\"BE\" type=\"tUInt8\" /> \
        </struct> \
    </structs> \
    <streams /> \
</adtf:ddl>";

static const std::string DDL_TEST_STRING_STREAM_TO_PREFIX =
"<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>\
<adtf:ddl xmlns:adtf=\"adtf\">\
   <header>\
       <language_version>1.02</language_version>\
       <author>AUDI Electronics Venture GmbH</author>\
       <date_creation>07.04.2010</date_creation>\
       <date_change>03.04.2014</date_change>\
       <description>ADTF Common Description File</description>\
   </header>\
   <units>\
       <baseunit description=\"this is the no unit class\" name=\"nou\" symbol=\"\" />\
       <prefixes name=\"TestPrefix\" power=\"1\" symbol=\"p\" />\
       <unit name=\"TestUnit\">\
           <numerator>1</numerator>\
           <denominator>1</denominator>\
           <offset>1.000000</offset>\
           <refUnit name=\"nou\" power=\"1\" prefix=\"TestPrefix\" />\
       </unit>\
   </units>\
   <datatypes>\
       <datatype description=\"predefined ADTF tBool datatype\" name=\"tBool\" size=\"8\" />\
       <datatype description=\"predefined ADTF tChar datatype\" name=\"tChar\" size=\"8\" />\
       <datatype description=\"predefined ADTF tUInt8 datatype\" name=\"tUInt8\" size=\"8\" />\
       <datatype description=\"predefined ADTF tInt8 datatype\" name=\"tInt8\" size=\"8\" />\
       <datatype description=\"predefined ADTF tUInt16 datatype\" name=\"tUInt16\" size=\"16\" />\
       <datatype description=\"predefined ADTF tInt16 datatype\" name=\"tInt16\" size=\"16\" />\
       <datatype description=\"predefined ADTF tUInt32 datatype\" name=\"tUInt32\" size=\"32\" />\
       <datatype description=\"predefined ADTF tInt32 datatype\" name=\"tInt32\" size=\"32\" />\
       <datatype description=\"predefined ADTF tUInt64 datatype\" name=\"tUInt64\" size=\"64\" />\
       <datatype description=\"predefined ADTF tInt64 datatype\" name=\"tInt64\" size=\"64\" />\
       <datatype description=\"predefined ADTF tFloat32 datatype\" name=\"tFloat32\" size=\"32\" />\
       <datatype description=\"predefined ADTF tFloat64 datatype\" name=\"tFloat64\" size=\"64\" />\
   </datatypes>\
   <enums />\
   <structs>\
       <struct alignment=\"1\" name=\"tSimpleStruct\" version=\"1\">\
           <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"ui8Val\" type=\"tUInt8\" />\
           <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"1\" name=\"ui16Val\" type=\"tUInt16\" />\
           <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"3\" name=\"ui32Val\" type=\"tUInt32\" />\
           <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"7\" name=\"i32Val\" type=\"tInt32\" />\
           <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"11\" name=\"i64Val\" type=\"tInt64\" />\
           <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"19\" name=\"f64Val\" type=\"tFloat64\" />\
           <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"27\" name=\"f32Val\" type=\"tFloat32\" />\
       </struct>\
       <struct alignment=\"1\" name=\"tHeaderStruct\" version=\"1\">\
           <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"ui32HeaderVal\" type=\"tUInt32\" />\
           <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"4\" name=\"f64HeaderVal\" type=\"tFloat64\" />\
       </struct>\
       <struct alignment=\"1\" name=\"tNestedStruct\" version=\"1\">\
           <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"0\" name=\"sHeaderStruct\" type=\"tHeaderStruct\" unit=\"TestUnit\" />\
           <element alignment=\"1\" arraysize=\"1\" byteorder=\"LE\" bytepos=\"12\" name=\"sSimpleStruct\" type=\"tSimpleStruct\" />\
       </struct>        \
   </structs>\
   <streams>\
       <stream name=\"a\" type=\"tNestedStruct\" />\
   </streams>\
</adtf:ddl>\
";

#endif
