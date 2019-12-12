// This is a generated file, changes to it may be overwritten in the future.

#pragma pack(push,4)
typedef struct
{
    tUInt32 c;
} simple_struct3;
#pragma pack(pop)

typedef simple_struct3 simple_struct4;

#pragma pack(push,1)
typedef struct
{
    tUInt8 a;
} simple_struct1;
#pragma pack(pop)

#pragma pack(push,2)
typedef struct
{
    tUInt16 b;
} simple_struct2;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt16 a;
    simple_struct1 b;
    simple_struct3 c;
} complex_struct1;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    simple_struct1 d;
    simple_struct3 e;
    complex_struct1 f;
} complex_struct2;
#pragma pack(pop)

typedef uint32_t tUInt32;

typedef uint8_t tUInt8;

typedef uint16_t tUInt16;

// The following type is assumed to be known:
// uint32_t

// The following type is assumed to be known:
// uint8_t

// The following type is assumed to be known:
// uint16_t

