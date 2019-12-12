// This is a generated file, changes to it may be overwritten in the future.

#pragma pack(push,1)
typedef struct
{
    tUInt8 a[5];
} simple_array_struct;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
    tUInt8 b;
} simple_struct1;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
    tUInt8 c;
    simple_struct1 d;
    simple_array_struct f;
    simple_struct1 g[6];
    simple_array_struct h[7];
} complex_array_struct1;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
    tUInt8 i[8];
    simple_struct1 j;
    simple_array_struct k;
    simple_struct1 l[9];
    simple_array_struct m[10];
} complex_array_struct2;
#pragma pack(pop)

const tUInt64 count1 = 8;

const tUInt8 count2 = 9;

typedef uint8_t tUInt8;

typedef uint64_t tUInt64;

// The following type is assumed to be known:
// uint8_t

// The following type is assumed to be known:
// uint64_t

