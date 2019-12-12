#ifndef RESULT_H
#define RESULT_H

// This is a generated file, changes to it may be overwritten in the future.

typedef enum {
    PF_UNKNOWN=0,
    PF_8BIT=10,
    PF_GREYSCALE_8=11,
    PF_RGB_8=12,
    PF_16BIT=20,
    PF_GREYSCALE_10=21,
    PF_GREYSCALE_12=22,
    PF_GREYSCALE_14=23,
    PF_GREYSCALE_16=24,
    PF_RGB_444=25,
    PF_RGB_555=26,
    PF_RGB_565=27,
    PF_RGBA_4444=28,
    PF_ABGR_4444=29,
    PF_RIII_10=30,
    PF_RIII_12=31,
    PF_RIII_14=32,
    PF_RIII_16=33,
    PF_BGR_555=34,
    PF_BGR_565=35,
    PF_24BIT=40,
    PF_GREYSCALE_18=41,
    PF_GREYSCALE_20=42,
    PF_GREYSCALE_22=43,
    PF_GREYSCALE_24=44,
    PF_RGB_888=45,
    PF_BGR_888=46,
    PF_32BIT=50,
    PF_ARGB_8888=51,
    PF_ABGR_8888=52,
    PF_RGBA_8888=53,
    PF_BGRA_8888=54,
    PF_GREYSCALE_32=55,
    PF_GREYSCALE_FLOAT32=56,
    PF_YUV420P_888=60,
    PF_CUSTOM=1000
} tPixelFormat;

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

typedef int32_t tInt32;

// The following type is assumed to be known:
// int32_t


#endif //RESULT_H
