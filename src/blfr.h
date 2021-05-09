#define BL_OBJ_TYPE_LOG_CONTAINER 10

#define BL_OBJ_TYPE_CAN_MESSAGE 1
#define BL_OBJ_TYPE_CAN_MESSAGE2 86

#define BL_OBJ_TYPE_CAN_FD_MESSAGE 100
#define BL_OBJ_TYPE_CAN_FD_MESSAGE_64 101

#define BL_OBJ_FLAG_TIME_TEN_MICS 0x00000001 /* 10 micro second timestamp */
#define BL_OBJ_FLAG_TIME_ONE_NANS 0x00000002 /* 1 nano second timestamp */

#define BL_HEADER_BASE_SIZE 16
#define BL_HEADER_SIZE 32
#define BL_HEADER_CONTAINER_SIZE 32
#define BL_MESSAGE_SIZE 48
#define BL_MESSAGEFD_SIZE 116 //sizeof => 120 because default pack(8)
#define BL_MESSAGEFD64_SIZE sizeof(VBLCANFDMessage64) //144

#define DEFAULT_CONTAINER_FACTOR 8

typedef struct LOGG{
  uint32_t mSignature;
  uint32_t mHeaderSize;
  uint32_t mCRC;
  uint8_t  mApplicationID;
  uint8_t  mdwCompression;
  uint8_t  mApplicationMajor;
  uint8_t  mApplicationMinor;
  uint64_t mFileSize;
  uint64_t mUncompressedFileSize;
  uint32_t mObjectCount;
  uint8_t  mApplicationBuild;
  uint8_t  mReserved1;
  uint8_t  mReserved2;
  uint8_t  mReserved3;
  SYSTEMTIME mMeasurementStartTime;
  SYSTEMTIME mLastObjectTime;
  uint8_t  mReserved4[72];
} LOGG_t;

typedef struct VBLFileStatisticsEx_t {
  uint32_t      mStatisticsSize;               /* sizeof (VBLFileStatisticsEx) */
  uint8_t       mApplicationID;                /* application ID */
  uint8_t       mApplicationMajor;             /* application major number */
  uint8_t       mApplicationMinor;             /* application minor number */
  uint8_t       mApplicationBuild;             /* application build number */
  uint64_t      mFileSize;                     /* file size in bytes */
  uint64_t      mUncompressedFileSize;         /* uncompressed file size in bytes */
  uint32_t      mObjectCount;                  /* number of objects */
  uint32_t      mObjectsRead;                  /* number of objects read */
  SYSTEMTIME    mMeasurementStartTime;         /* measurement start time */
  SYSTEMTIME    mLastObjectTime;               /* last object time */
  uint32_t      mReserved[18];                 /* reserved */
} VBLFileStatisticsEx;

typedef struct VBLObjectHeaderBase_t {
  uint32_t  mSignature;
  uint16_t  mHeaderSize;
  uint16_t  mHeaderVersion;
  uint32_t  mObjectSize;
  uint32_t  mObjectType;
} VBLObjectHeaderBase;

typedef struct VBLObjectHeaderContainer_t {
  VBLObjectHeaderBase base;
  uint32_t            compressedflag;
  uint32_t            reserved1;
  uint32_t            deflatebuffersize;
  uint32_t            reserved2;
} VBLObjectHeaderContainer;

typedef struct VBLObjectHeader_t {
  VBLObjectHeaderBase mBase;
  uint32_t  mObjectFlags;
  uint16_t  mReserved;
  uint16_t  mObjectVersion;
  uint64_t  mObjectTimeStamp;
} VBLObjectHeader;

typedef struct VBLCANMessage_t {
  VBLObjectHeader mHeader;
  uint16_t        mChannel;
  uint8_t         mFlags;
  uint8_t         mDLC;
  uint32_t        mID;
  uint8_t         mData[8];
} VBLCANMessage;

typedef struct VBLCANMessage2_t {
  VBLObjectHeader mHeader;
  uint16_t        mChannel;
  uint8_t         mFlags;
  uint8_t         mDLC;
  uint32_t        mID;
  uint8_t         mData[8];
  uint32_t        mFrameLength;
  uint8_t         mBitCount;
  uint8_t         mReserved1;
  uint16_t        mReserved2;
} VBLCANMessage2;

typedef struct VBLCANFDMessage_t {
  VBLObjectHeader mHeader;
  uint16_t           mChannel;
  uint8_t            mFlags;
  uint8_t            mDLC;
  uint32_t           mID;
  uint32_t           mFrameLength;
  uint8_t            mArbBitCount;
  uint8_t            mCANFDFlags;
  uint8_t            mValidDataBytes;
  uint8_t            mReserved1;
  uint32_t           mReserved2;
  uint8_t            mData[64];
} VBLCANFDMessage;

typedef struct VBLCANFDExtFrameData_t{
  uint32_t mBTRExtArb;
  uint32_t mBTRExtData;
  //may be extended in future versions
} VBLCANFDExtFrameData;

typedef struct VBLCANFDMessage64_t
{
  VBLObjectHeader mHeader;                     /* object header */
  uint8_t            mChannel;                    /* application channel */
  uint8_t            mDLC;                        /* CAN dlc */
  uint8_t            mValidDataBytes;             /* Valid payload length of mData */
  uint8_t            mTxCount;                    /* TXRequiredCount (4 bits), TxReqCount (4 Bits) */
  uint32_t           mID;                         /* CAN ID */
  uint32_t           mFrameLength;                /* message length in ns - without 3 inter frame space bits */
                                               /* and by Rx-message also without 1 End-Of-Frame bit */
  uint32_t           mFlags;                      /* flags */
  uint32_t           mBtrCfgArb;                  /* bit rate used in arbitration phase */
  uint32_t           mBtrCfgData;                 /* bit rate used in data phase */
  uint32_t           mTimeOffsetBrsNs;            /* time offset of brs field */
  uint32_t           mTimeOffsetCrcDelNs;         /* time offset of crc delimiter field */
  uint16_t            mBitCount;                   /* complete message length in bits */
  uint8_t            mDir;
  uint8_t            mExtDataOffset;
  uint32_t           mCRC;                        /* CRC for CAN */
  uint8_t            mData[64];                   /* CAN FD data */
  VBLCANFDExtFrameData mExtFrameData;
} VBLCANFDMessage64;