struct simple_array_struct
{
    tUInt8 a[5];
};

struct simple_struct1
{
    tUInt8 b;
};

struct complex_array_struct1
{
    tUInt8 c;
    simple_struct1 d;
    simple_array_struct f;
    simple_struct1 g[6];
    simple_array_struct h[7];
};

#define count1 8
const tUInt8 count2 = 9;

struct complex_array_struct2
{
    tUInt8 i[count1];
    simple_struct1 j;
    simple_array_struct k;
    simple_struct1 l[count2];
    simple_array_struct m[10];
};

