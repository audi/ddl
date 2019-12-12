#pragma pack(push, 4)
struct simple_struct1
{
    tUInt8 a;
};
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
    tUInt16 b;
} simple_struct2;
#pragma pack(pop)

#pragma pack(push, 2)
typedef struct simple_struct3
{
    tUInt32 c;
} simple_struct4;

typedef struct
{
    tUInt64 d;
} simple_struct5;
#pragma pack(pop)

#pragma pack(1)
typedef struct
{
    tUInt64 e;
} simple_struct6;
