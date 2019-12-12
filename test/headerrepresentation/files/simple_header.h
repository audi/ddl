// For type # see implementation of cHeaderImporter::BuildStructs()

// Type #1
struct simple_struct1
{
    uint8_t a;
};

// Type #2
typedef struct
{
    tUInt16 b;
} simple_struct2;

// Type #3
typedef struct simple_struct3
{
    unsigned int c;
} simple_struct4;