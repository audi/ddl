/**
 * @file
 * This is a generated file, changes to it may be overwritten in the future.
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

typedef tInt16 tPixelFormat;

#pragma pack(push,1)
typedef struct
{
    tUInt32 ui32MajorType;
    tUInt32 ui32SubType;
    tUInt32 ui32Flags;
} tMediaTypeInfo;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
    tMediaTypeInfo mediatype;
} adtf_core_media_type;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
    tInt32 nFormatType;
    tInt32 nChannels;
    tInt32 nSamplesPerSec;
    tInt32 nBitsPerSample;
    tInt32 nNumSamples;
    tInt32 nSize;
} tWaveFormat;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
    tMediaTypeInfo sMediatype;
    tWaveFormat sWaveFormat;
} adtf_type_audio;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
    tInt32 nWidth;
    tInt32 nHeight;
    tInt16 nBitsPerPixel;
    tInt16 nPixelFormat;
    tInt32 nBytesPerLine;
    tInt32 nSize;
    tInt32 nPaletteSize;
} tBitmapFormat;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
    tMediaTypeInfo sMediatype;
    tBitmapFormat sBitmapFormat;
} adtf_type_video;
#pragma pack(pop)

// The following types are assumed to be known:
// tInt16
// tUInt32
// tInt32

