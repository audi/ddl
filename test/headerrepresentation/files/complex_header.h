struct simple_struct1
{
    tUInt8 a;
};

typedef struct
{
    tUInt16 b;
} simple_struct2;

typedef struct simple_struct3
{
    tUInt32 c;
} simple_struct4;

typedef struct
{
    tUInt16 a;
    simple_struct1 b;
    simple_struct3 c;
} complex_struct1;

typedef struct
{
    simple_struct1 d;
    simple_struct3 e;
    complex_struct1 f;
} complex_struct2;