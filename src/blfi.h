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

