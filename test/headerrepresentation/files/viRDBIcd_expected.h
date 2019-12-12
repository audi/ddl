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

#pragma pack(push,4)
typedef struct
{
    tFloat64 x;
    tFloat64 y;
    tFloat64 z;
    tUInt8 flags;
    tUInt8 type;
    tUInt16 system;
} RDB_POINT_t;
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
    tUInt16 id;
    tUInt16 spare;
    RDB_COORD_t pos;
} RDB_COORD_SYSTEM_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt32 playerId;
    tUInt16 roadId;
    tInt8 laneId;
    tUInt8 flags;
    tFloat32 roadS;
    tFloat32 roadT;
    tFloat32 laneOffset;
    tFloat32 hdgRel;
    tFloat32 pitchRel;
    tFloat32 rollRel;
    tUInt8 roadType;
    tUInt8 spare1;
    tUInt16 spare2;
    tFloat32 pathS;
} RDB_ROAD_POS_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt32 playerId;
    tInt8 id;
    tInt8 prevId;
    tInt8 nextId;
    tUInt8 spare0;
    tFloat32 lateralDist;
    tFloat32 yawRel;
    tFloat64 curvHor;
    tFloat64 curvHorDot;
    tFloat32 startDx;
    tFloat32 previewDx;
    tFloat32 width;
    tFloat32 height;
    tFloat64 curvVert;
    tFloat64 curvVertDot;
    tUInt8 type;
    tUInt8 color;
    tUInt16 noDataPoints;
    tUInt32 spare1[2];
} RDB_ROADMARK_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt16 roadId;
    tInt8 id;
    tUInt8 neighborMask;
    tInt8 leftLaneId;
    tInt8 rightLaneId;
    tUInt8 borderType;
    tUInt8 material;
    tUInt16 status;
    tUInt16 type;
    tFloat32 width;
    tFloat64 curvVert;
    tFloat64 curvVertDot;
    tFloat64 curvHor;
    tFloat64 curvHorDot;
    tUInt32 playerId;
    tUInt32 spare1;
} RDB_LANE_INFO_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt32 id;
    tUInt8 category;
    tUInt8 type;
    tInt16 modelId;
    tChar name[32];
    tChar modelName[32];
    tChar fileName[1024];
    tUInt16 flags;
    tUInt16 spare0;
    tUInt32 spare1;
} RDB_OBJECT_CFG_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tFloat32 dimX;
    tFloat32 dimY;
    tFloat32 dimZ;
    tFloat32 offX;
    tFloat32 offY;
    tFloat32 offZ;
} RDB_GEOMETRY_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt32 id;
    tUInt8 category;
    tUInt8 type;
    tUInt16 visMask;
    tChar name[32];
    RDB_GEOMETRY_t geo;
    RDB_COORD_t pos;
    tUInt32 parent;
    tUInt32 spare1;
} RDB_OBJECT_STATE_BASE_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    RDB_COORD_t speed;
    RDB_COORD_t accel;
    tUInt32 spare[4];
} RDB_OBJECT_STATE_EXT_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    RDB_OBJECT_STATE_BASE_t base;
    RDB_OBJECT_STATE_EXT_t ext;
} RDB_OBJECT_STATE_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt32 playerId;
    tFloat32 rps;
    tFloat32 load;
    tUInt32 spare1[2];
} RDB_ENGINE_BASE_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tFloat32 rpsStart;
    tFloat32 torque;
    tFloat32 torqueInner;
    tFloat32 torqueMax;
    tFloat32 torqueFriction;
    tFloat32 fuelCurrent;
    tFloat32 fuelAverage;
    tUInt32 spare[2];
} RDB_ENGINE_EXT_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    RDB_ENGINE_BASE_t base;
    RDB_ENGINE_EXT_t ext;
} RDB_ENGINE_t;
#pragma pack(pop)

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
    tUInt32 playerId;
    tUInt8 id;
    tUInt8 spare0[3];
    tFloat32 radiusStatic;
    tFloat32 springCompression;
    tFloat32 rotAngle;
    tFloat32 slip;
    tFloat32 steeringAngle;
    tUInt32 spare1[4];
} RDB_WHEEL_BASE_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tFloat32 vAngular;
    tFloat32 forceZ;
    tFloat32 forceLat;
    tFloat32 forceLong;
    tFloat32 forceTireWheelXYZ[3];
    tFloat32 radiusDynamic;
    tFloat32 brakePressure;
    tFloat32 torqueDriveShaft;
    tFloat32 damperSpeed;
    tUInt32 spare2[4];
} RDB_WHEEL_EXT_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    RDB_WHEEL_BASE_t base;
    RDB_WHEEL_EXT_t ext;
} RDB_WHEEL_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt32 playerId;
    tUInt32 lightMask;
    tFloat32 steering;
    tFloat32 steeringWheelTorque;
    tUInt8 accMask;
    tUInt8 accSpeed;
    tUInt8 batteryState;
    tInt8 batteryRate;
    tUInt16 displayLightMask;
    tUInt16 spare0;
    tUInt32 spare[5];
} RDB_VEHICLE_SYSTEMS_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt32 playerId;
    tFloat32 mass;
    tFloat32 wheelBase;
    tInt32 spare[4];
} RDB_VEHICLE_SETUP_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt32 id;
    tUInt16 width;
    tUInt16 height;
    tUInt8 pixelSize;
    tUInt8 pixelFormat;
    tUInt16 spare0;
    tUInt32 imgSize;
    tUInt8 color[4];
    tUInt32 spare1[3];
} RDB_IMAGE_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt32 id;
    tUInt8 type;
    tUInt8 dimension;
    tUInt16 spare;
    tUInt32 dataSize;
    tUInt32 spare1[4];
} RDB_FUNCTION_t;
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

#pragma pack(push,4)
typedef struct
{
    tUInt8 category;
    tUInt8 type;
    tUInt16 flags;
    tUInt32 id;
    tUInt32 sensorId;
    tFloat64 dist;
    RDB_COORD_t sensorPos;
    tInt8 occlusion;
    tUInt8 spare0[3];
    tUInt32 spare[3];
} RDB_SENSOR_OBJECT_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt16 id;
    tUInt16 width;
    tUInt16 height;
    tUInt16 spare0;
    tFloat32 clipNear;
    tFloat32 clipFar;
    tFloat32 focalX;
    tFloat32 focalY;
    tFloat32 principalX;
    tFloat32 principalY;
    RDB_COORD_t pos;
    tUInt32 spare1[4];
} RDB_CAMERA_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt16 id;
    tInt8 templateId;
    tUInt8 state;
    tInt32 playerId;
    RDB_COORD_t pos;
    tUInt16 flags;
    tUInt16 spare0;
    tInt32 spare1[2];
} RDB_LIGHT_SOURCE_BASE_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tFloat32 nearFar[2];
    tFloat32 frustumLRBT[4];
    tFloat32 intensity[3];
    tFloat32 atten[3];
    tInt32 spare1[3];
} RDB_LIGHT_SOURCE_EXT_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    RDB_LIGHT_SOURCE_BASE_t base;
    RDB_LIGHT_SOURCE_EXT_t ext;
} RDB_LIGHT_SOURCE_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt16 id;
    tUInt16 flags;
    RDB_COORD_t roadDataIn;
    tFloat32 friction;
    tInt32 playerId;
    tInt32 spare1;
} RDB_CONTACT_POINT_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt32 id;
    tUInt32 playerId;
    tFloat32 roadDist;
    RDB_COORD_t pos;
    tInt32 type;
    tInt32 subType;
    tFloat32 value;
    tUInt32 state;
    tInt8 readability;
    tInt8 occlusion;
    tUInt16 spare0;
    tUInt32 addOnId;
    tInt8 minLane;
    tInt8 maxLane;
    tUInt16 spare;
} RDB_TRAFFIC_SIGN_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt32 playerId;
    tInt8 wheelId;
    tUInt8 spare0;
    tUInt16 spare1;
    tUInt32 roadId;
    tFloat32 defaultSpeed;
    tFloat32 waterLevel;
    tUInt32 eventMask;
    tInt32 spare2[12];
} RDB_ROAD_STATE_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tFloat32 visibility;
    tUInt32 timeOfDay;
    tFloat32 brightness;
    tUInt8 precipitation;
    tUInt8 cloudState;
    tUInt16 flags;
    tUInt32 spare1[8];
} RDB_ENVIRONMENT_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt32 playerId;
    RDB_COORD_t pos;
    tUInt32 spare[4];
    tUInt32 noCoords;
    tUInt32 dataSize;
} RDB_PED_ANIMATION_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt32 playerId;
    tFloat32 pathS;
    tFloat32 roadS;
    tFloat32 fuelCurrent;
    tFloat32 fuelAverage;
    tUInt32 stateFlags;
    tFloat32 slip;
    tUInt32 spare[4];
} RDB_CUSTOM_SCORING_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tFloat32 deltaT;
    tUInt32 frameNo;
    tInt32 spare;
} RDB_TRIGGER_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt32 playerId;
    tFloat32 steeringWheel;
    tFloat32 steeringSpeed;
    tFloat32 throttlePedal;
    tFloat32 brakePedal;
    tFloat32 clutchPedal;
    tFloat32 accelTgt;
    tFloat32 steeringTgt;
    tFloat64 curvatureTgt;
    tFloat32 steeringTorque;
    tFloat32 engineTorqueTgt;
    tFloat32 speedTgt;
    tUInt8 gear;
    tUInt8 sourceId;
    tUInt8 spare0[2];
    tUInt32 validityFlags;
    tUInt32 flags;
    tUInt32 mockupInput0;
    tUInt32 mockupInput1;
    tUInt32 mockupInput2;
    tUInt32 spare;
} RDB_DRIVER_CTRL_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt32 playerId;
    tFloat32 speedFromRules;
    tFloat32 distToSpeed;
    tFloat32 spare0[4];
    tUInt32 flags;
    tUInt32 spare[4];
} RDB_DRIVER_PERCEPTION_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tInt32 id;
    tFloat32 state;
    tUInt32 stateMask;
} RDB_TRAFFIC_LIGHT_BASE_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tFloat32 duration;
    tUInt8 type;
    tUInt8 spare[3];
} RDB_TRAFFIC_LIGHT_PHASE_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tInt32 ctrlId;
    tFloat32 cycleTime;
    tUInt16 noPhases;
    tUInt32 dataSize;
} RDB_TRAFFIC_LIGHT_EXT_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    RDB_TRAFFIC_LIGHT_BASE_t base;
    RDB_TRAFFIC_LIGHT_EXT_t ext;
} RDB_TRAFFIC_LIGHT_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt32 mask;
    tUInt32 cmdMask;
    tUInt32 spare[2];
} RDB_SYNC_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt16 id;
    tUInt16 flags;
    tUInt16 spare[2];
    tFloat64 x;
    tFloat64 y;
} RDB_ROAD_QUERY_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt16 version;
    tUInt16 spare;
    tChar sender[64];
    tChar receiver[64];
    tUInt32 dataSize;
} RDB_SCP_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt32 playerId;
    tFloat64 spacing;
    tUInt16 flags;
    tUInt16 noDataPoints;
    tUInt32 spare[4];
} RDB_TRAJECTORY_t;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
} RDB_END_OF_FRAME_t;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
} RDB_START_OF_FRAME_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt16 magicNo;
    tUInt16 version;
    tUInt32 headerSize;
    tUInt32 dataSize;
    tUInt32 frameNo;
    tFloat64 simTime;
} RDB_MSG_HDR_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt32 headerSize;
    tUInt32 dataSize;
    tUInt32 elementSize;
    tUInt16 pkgId;
    tUInt16 flags;
} RDB_MSG_ENTRY_HDR_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    RDB_MSG_HDR_t hdr;
    RDB_MSG_ENTRY_HDR_t entryHdr;
} RDB_MSG_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt32 thisSize;
    tUInt32 bufferSize;
    tUInt16 id;
    tUInt16 spare0;
    tUInt32 flags;
    tUInt32 offset;
    tUInt32 spare1[4];
} RDB_SHM_BUFFER_INFO_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct
{
    tUInt32 headerSize;
    tUInt32 dataSize;
    tUInt8 noBuffers;
} RDB_SHM_HDR_t;
#pragma pack(pop)

typedef double tFloat64;

typedef uint8_t tUInt8;

typedef uint16_t tUInt16;

typedef float tFloat32;

typedef uint32_t tUInt32;

typedef int8_t tInt8;

typedef int16_t tInt16;

typedef char tChar;

typedef int32_t tInt32;

// The following type is assumed to be known:
// double

// The following type is assumed to be known:
// uint8_t

// The following type is assumed to be known:
// uint16_t

// The following type is assumed to be known:
// float

// The following type is assumed to be known:
// uint32_t

// The following type is assumed to be known:
// int8_t

// The following type is assumed to be known:
// int16_t

// The following type is assumed to be known:
// char

// The following type is assumed to be known:
// int32_t

