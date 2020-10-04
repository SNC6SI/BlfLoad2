#define BL_OBJ_TYPE_CAN_MESSAGE 1
#define BL_OBJ_TYPE_LOG_CONTAINER 10

#define BL_OBJ_FLAG_TIME_ONE_NANS 2

#define BL_HEADER_BASE_SIZE 16
#define BL_HEADER_SIZE 32
#define BL_HEADER_CONTAINER_SIZE 32
#define BL_MESSAGE_SIZE 48
#define BL_LOGG_SIZE 144

#define BL_OBJ_SIGNATURE  0x4A424F4C
#define BL_LOGG_SIGNATURE 0x47474F4C

#define BL_APPID_BLFLOAD 22

#define BL_Major 0
#define BL_Minor 1
#define BL_AppBuild 1

#define BL_CHUNK 0x20000
#define BL_ZLIB_LEVEL 9


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

typedef struct VBLObjectHeaderBase_t {
  uint32_t  mSignature;
  uint16_t  mHeaderSize;
  uint16_t  mHeaderVersion;
  uint32_t  mObjectSize;
  uint32_t  mObjectType;
} VBLObjectHeaderBase;

typedef struct VBLObjectHeaderContainer_t {
  VBLObjectHeaderBase mBase;
  uint32_t            mCompressedflag;
  uint32_t            mReserved1;
  uint32_t            mDeflatebuffersize;
  uint32_t            mReserved2;
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