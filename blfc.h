typedef struct LOGG{
  uint32_t mSignature;
  uint32_t mHeaderSize;
  uint32_t mCRC;
  uint8_t  appID;
  uint8_t  dwCompression;
  uint8_t  appMajor;
  uint8_t  appMinor;
  uint64_t fileSize;
  uint64_t uncompressedFileSize;
  uint32_t objectCount;
  uint8_t  appBuild;
  uint8_t  mReserved1;
  uint8_t  mReserved2;
  uint8_t  mReserved3;
  SYSTEMTIME mMeasurementStartTime;
  SYSTEMTIME mMeasurementEndTime;
  uint8_t  mReserved4[72];
} LOGG_t;

typedef struct VBLObjectHeaderBase_t {
  uint32_t  mSignature;
  uint16_t  mHeaderSize;
  uint16_t  mHeaderVersion;
  uint32_t  mObjectSize;
  uint32_t  mObjectType;
} VBLObjectHeaderBase;

typedef struct VBLObjectHeaderBaseLOGG_t {
  VBLObjectHeaderBase base;
  uint32_t            compressedflag;
  uint32_t            reserved1;
  uint32_t            deflatebuffersize;
  uint32_t            reserved2;
} VBLObjectHeaderBaseLOGG;

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