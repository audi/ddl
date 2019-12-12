// This is a generated file, changes to it may be overwritten in the future.

#pragma pack(push,4)
typedef struct
{
    tUInt32 playerId;
    tUInt8 gearBoxType;
    tUInt8 driveTrainType;
    tUInt8 gear;
    tUInt8 spare0;
    tUInt32 spare1[2];
} RDB_DRIVETRAIN_BASE_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tFloat32 torqueGearBoxIn;
    tFloat32 torqueCenterDiffOut;
    tFloat32 torqueShaft;
    tUInt32 spare1[2];
} RDB_DRIVETRAIN_EXT_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    RDB_DRIVETRAIN_BASE_t base;
    RDB_DRIVETRAIN_EXT_t ext;
} RDB_DRIVETRAIN_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tFloat64 x;
    tFloat64 y;
    tFloat64 z;
    tFloat32 h;
    tFloat32 p;
    tFloat32 r;
    tUInt8 flags;
    tUInt8 type;
    tUInt16 system;
} RDB_COORD_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt32 id;
    tUInt8 type;
    tUInt8 hostCategory;
    tUInt16 spare0;
    tUInt32 hostId;
    tChar name[32];
    tFloat32 fovHV[2];
    tFloat32 clipNF[2];
    RDB_COORD_t pos;
    RDB_COORD_t originCoordSys;
    tInt32 spare[4];
} RDB_SENSOR_STATE_t;
#pragma pack(pop)

typedef uint32_t tUInt32;

typedef uint8_t tUInt8;

typedef float tFloat32;

typedef double tFloat64;

typedef uint16_t tUInt16;

typedef char tChar;

typedef int32_t tInt32;

// The following type is assumed to be known:
// uint32_t

// The following type is assumed to be known:
// uint8_t

// The following type is assumed to be known:
// float

// The following type is assumed to be known:
// double

// The following type is assumed to be known:
// uint16_t

// The following type is assumed to be known:
// char

// The following type is assumed to be known:
// int32_t

