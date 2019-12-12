/**
 * @file
 * Compat header containing structs and defines needed for the ddlcoder (mediacoder) test
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

#ifndef _ADTF_TEST_COMPAT_HEADER_
#define _ADTF_TEST_COMPAT_HEADER_

/**
* This Macro is the type name of the tCanMessage to describe media samples submitted on a pin (@ref adtf::IPin).
* set these description on the Media Type (@ref adtf::IMediaTypeDescription of the IMediatype set on pin).
*/
#define ADTF_MEDIA_DESC_CANMESSAGE                   "tCanMessage"

/**
* This Macro defines the description of tCanMessage structure used within ADTF @ref adtf::cMediaSampleCan.
* used for @ref ADTF_MEDIA_DESC_CANMESSAGE_DESCRIPTION in Version 1.0+.
*/
#define ADTF_MEDIA_DESC_CANMESSAGE_DESCRIPTION_DDL1  "<struct name=\"" ADTF_MEDIA_DESC_CANMESSAGE "\" alignment=\"1\" version=\"1\">" \
                                                     "    <element type=\"tUInt16\"  name=\"ui16Id\"     bytepos=\"0\" arraysize=\"1\" byteorder=\"LE\" alignment=\"1\"/>" \
                                                     "    <element type=\"tUInt8\"   name=\"ui8Channel\" bytepos=\"2\" arraysize=\"1\" byteorder=\"LE\" alignment=\"1\"/>" \
                                                     "    <element type=\"tUInt8\"   name=\"ui8Length\"  bytepos=\"3\" arraysize=\"1\" byteorder=\"LE\" alignment=\"1\"/>" \
                                                     "    <element type=\"tUInt8\"   name=\"pui8Data\"   bytepos=\"4\" arraysize=\"8\" byteorder=\"LE\" alignment=\"1\"/>" \
                                                     "</struct>"

/**
* This Macro defines the description of a tCanMessage structure used within ADTF @ref adtf::cMediaSampleCan.
* See @ref ADTF_MEDIA_DESC_CANMESSAGE_DESCRIPTION_DDL1.
*/
#define ADTF_MEDIA_DESC_CANMESSAGE_DESCRIPTION       ADTF_MEDIA_DESC_CANMESSAGE_DESCRIPTION_DDL1


typedef struct
{
    /// id of can message
    std::uint16_t     ui16Id;
    /// channel to send or transmit can data
    std::uint8_t      ui8Channel;
    /// length of data
    std::uint8_t      ui8Length;
    /// data field
    std::uint8_t      pui8Data[8];
}  tCanMessage;

#define ADTF_MEDIA_DESC_MEDIA_TYPEINFO_DDL1          "  <struct name=\"tMediaTypeInfo\" alignment=\"1\" version=\"1\">" \
                                                     "    <element type=\"tUInt32\" name=\"ui32MajorType\" bytepos=\"0\" arraysize=\"1\" byteorder=\"LE\" alignment=\"1\"/>" \
                                                     "    <element type=\"tUInt32\" name=\"ui32SubType\"   bytepos=\"4\" arraysize=\"1\" byteorder=\"LE\" alignment=\"1\"/>" \
                                                     "    <element type=\"tUInt32\" name=\"ui32Flags\"     bytepos=\"8\" arraysize=\"1\" byteorder=\"LE\" alignment=\"1\"/>" \
                                                     "  </struct>"

#define ADTF_MEDIA_DESC_MEDIATYPE_DDL1               "<structs>" \
                                                     ADTF_MEDIA_DESC_MEDIA_TYPEINFO_DDL1 \
                                                     "  <struct name=\"" OID_ADTF_MEDIA_TYPE "\" alignment=\"1\" version=\"1\">" \
                                                     "    <element type=\"tMediaTypeInfo\" name=\"mediatype\" bytepos=\"0\" arraysize=\"1\" byteorder=\"LE\" alignment=\"1\"/>" \
                                                     "  </struct>" \
                                                     "</structs>"

#define ADTF_MEDIA_DESC_MEDIA_TYPEINFO_DDL2          "  <struct name=\"tMediaTypeInfo\" alignment=\"1\" version=\"1\">" \
                                                     "    <element type=\"tUInt32\" name=\"ui32MajorType\" bytepos=\"0\" arraysize=\"1\" byteorder=\"LE\" alignment=\"1\"/>" \
                                                     "    <element type=\"tUInt32\" name=\"ui32SubType\"   bytepos=\"4\" arraysize=\"1\" byteorder=\"LE\" alignment=\"1\"/>" \
                                                     "    <element type=\"tUInt32\" name=\"ui32Flags\"     bytepos=\"8\" arraysize=\"1\" byteorder=\"LE\" alignment=\"1\"/>" \
                                                     "  </struct>"


#define ADTF_MEDIA_DESC_MEDIATYPE_DDL2               "<structs>" \
                                                     ADTF_MEDIA_DESC_MEDIA_TYPEINFO_DDL2 \
                                                     "  <struct name=\"" OID_ADTF_MEDIA_TYPE "\" alignment=\"1\" version=\"1\">" \
                                                     "    <element type=\"tMediaTypeInfo\" name=\"mediatype\" bytepos=\"0\" arraysize=\"1\" byteorder=\"LE\" alignment=\"1\"/>" \
                                                     "  </struct>" \
                                                     "</structs>"

#define ADTF_MEDIA_DESC_MEDIA_TYPEINFO               ADTF_MEDIA_DESC_MEDIA_TYPEINFO_DDL2
#define ADTF_MEDIA_DESC_MEDIATYPE                   ADTF_MEDIA_DESC_MEDIATYPE_DDL2

#pragma pack ( push, 1 )
typedef struct
{
    ///The major type.
    std::uint32_t             ui32MajorType;
    ///The minor sub type.
    std::uint32_t             ui32SubType;
    ///Flags.
    std::uint32_t             ui32Flags;
} tMediaTypeInfo;

typedef struct
{
    /// media type information
    tMediaTypeInfo mediatype;
} tADTFCoreMediaType;


#pragma pack ( pop )


class IMediaDescription
{
public:
    typedef enum
    {
        MDNT_FindOutSelf  = 0x00,
        MDNT_StreamType   = 0x01,
        MDNT_StructType   = 0x02,
        MDNT_DataType     = 0x03,
        MDNT_UnitType     = 0x04,
        MDNT_BaseUnitType = 0x05
    } tMediaNameType;

};

class IMediaDescriptionManager
{
    public:
        typedef enum
        {
            MDMF_ForceOverwrite = 0x0001000,
            MDMF_FilenameRef = 0x0002000,
            MDMF_StreamStructType = 0x0004000,
            MDMF_StreamDataType = 0x0008000,
            MDMF_ForceStreamDescription = 0x0010000,
            MDMF_ForceStructDescription = 0x0020000,
            MDMF_ForceDatatypeDescription = 0x0040000
        } tMediaDecriptionManagerFlags;

};

#endif // _ADTF_TEST_COMPAT_HEADER_
