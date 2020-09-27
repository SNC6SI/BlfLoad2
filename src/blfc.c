#include <stdint.h>
#include <windows.h>
#include "mex.h"
#include "blfc.h"
#include "zlib.h"

//#define BLFDEBUG


#define BL_OBJ_TYPE_CAN_MESSAGE 1
#define BL_OBJ_TYPE_LOG_CONTAINER 10
#define BL_OBJ_TYPE_CAN_MESSAGE2 86

#define BL_OBJ_FLAG_TIME_TEN_MICS 0x00000001 /* 10 micro second timestamp */
#define BL_OBJ_FLAG_TIME_ONE_NANS 0x00000002 /* 1 nano second timestamp */

#define BL_HEADER_BASE_SIZE 16
#define BL_HEADER_SIZE 32
#define BL_HEADER_CONTAINER_SIZE 32
#define BL_MESSAGE_SIZE 48
//#define BL_MESSAGE2_SIZE 56

static FILE *fp=NULL;
static long int filelen=0;
static long int midx=0;
static long int lidx=0;
static LOGG_t logg;
static VBLObjectHeaderBase Base;
static VBLObjectHeaderContainer Container;
static VBLCANMessage message;
static uint8_t needFree = 0;
static uint8_t peekFlag=1;
static uint8_t contFlag=0;
static uint32_t rcnt=0;

static uint8_t *compressedData;
static uint32_t compressedSize;
static uint8_t *unCompressedData;
static uint32_t unCompressedSize;
static uint8_t *restData;
static uint32_t restSize;


static double *candata, *cantime, *canmsgid, *canchannel;

void blfInit(void){
    filelen = 0;
    midx = 0;
    lidx = 0;
    needFree = 0;
    peekFlag = 1;
    contFlag = 0;
    rcnt = 0;
}


int memUncompress(uint8_t  *next_out,
     uint32_t  avail_out,
     uint8_t  *next_in,
     uint32_t  avail_in,
     uint32_t *total_out_ptr)
{
  int zres;
  z_stream stream;

  stream.next_in = next_in;
  stream.avail_in = avail_in;
  stream.next_out = next_out;
  stream.avail_out = avail_out;
  stream.total_out = 0;
  stream.state = NULL;
  stream.zalloc = NULL;
  stream.zfree = NULL;

  zres = inflateInit_(&stream, ZLIB_VERSION, sizeof(stream));
  if(zres == Z_OK) zres = inflate(&stream, Z_FINISH);
  if(zres == Z_STREAM_END) zres = Z_OK;
  if(zres == Z_OK) {
    inflateEnd(&stream);
    if(total_out_ptr != NULL) {
      *total_out_ptr = stream.total_out;
    }
  }
  return zres == Z_OK;
}

uint8_t blfPeekObject(){
    uint8_t success = 1;
    uint32_t paddingBytes;
    long int midx_;
    while(peekFlag){
        midx_ = midx + BL_HEADER_BASE_SIZE;
        if(midx_ >= filelen){
            success = 0;
            return success;
        }
        //
        fread(&Base, BL_HEADER_BASE_SIZE, 1, fp);
        //
        paddingBytes = Base.mObjectSize & 3;
        midx_ = midx + Base.mObjectSize + paddingBytes;
        if(midx_ > filelen){
            success = 0;
            return success;
        }
        //
        fseek(fp, -BL_HEADER_BASE_SIZE, SEEK_CUR);
        //
        if(Base.mObjectType == BL_OBJ_TYPE_LOG_CONTAINER){
            if(contFlag){
                restSize = unCompressedSize - lidx;
                if(restSize>0){
                    restData = mxMalloc(restSize);
                    memcpy(restData, unCompressedData+lidx, restSize);
                }
            }
            if(needFree){
                mxFree(unCompressedData);
            }
            fread(&Container, BL_HEADER_CONTAINER_SIZE, 1, fp);
            
            compressedSize = Container.base.mObjectSize - BL_HEADER_CONTAINER_SIZE;
            compressedData = mxMalloc(compressedSize);
            fread(compressedData, compressedSize, 1, fp);
            if(paddingBytes>0){
                fseek(fp, paddingBytes, SEEK_CUR);
            }
            //
            unCompressedSize = Container.deflatebuffersize + restSize;
            unCompressedData = mxMalloc(unCompressedSize);
            memUncompress(unCompressedData+restSize, unCompressedSize-restSize, compressedData, compressedSize, 0);
            memcpy(unCompressedData, restData, restSize);
            //
            mxFree(compressedData);
            if(restSize>0){
                mxFree(restData);
            }
            //
            needFree = 1;
            lidx = 0;
            peekFlag = 0;
        }
        midx = midx_;
    }
    return success;
}

uint8_t blfReadObjectSecure(){
    long int lidx_;
    int i;
    uint32_t paddingBytes;
    lidx_ = lidx + BL_HEADER_BASE_SIZE;
    if(lidx_ >= unCompressedSize){
        peekFlag = 1;
        contFlag = 1;
        return 0;
    }
    //
    //fread(&Base, BL_HEADER_BASE_SIZE, 1, fp);
    //memcpy(void *dest, const void *src, size_t n)
    memcpy(&Base, unCompressedData+lidx, BL_HEADER_BASE_SIZE);
    //
    paddingBytes = Base.mObjectSize & 3;
    lidx_ = lidx + Base.mObjectSize + paddingBytes;
    if(lidx_ > unCompressedSize){
        peekFlag = 1;
        contFlag = 1;
        return 0;
    }
    //
    contFlag = 0;
    switch (Base.mObjectType){
        case BL_OBJ_TYPE_CAN_MESSAGE:
        case BL_OBJ_TYPE_CAN_MESSAGE2:
            memcpy(&message, unCompressedData+lidx, BL_MESSAGE_SIZE);
            for(i=0;i<8;i++) *(candata + rcnt*8 + i) = (double)message.mData[i];
            *(canmsgid + rcnt) = (double)message.mID;
            *(canchannel + rcnt) = (double)message.mChannel;
            if(message.mHeader.mObjectFlags==BL_OBJ_FLAG_TIME_ONE_NANS)
            *(cantime + rcnt) = ((double)message.mHeader.mObjectTimeStamp)/1000000000;
            else
            *(cantime + rcnt) = ((double)message.mHeader.mObjectTimeStamp)/100000;
            rcnt++;
            break;
        default:
            break;
    }
    lidx = lidx_;
    return 1;
}


void
mexFunction(int nlhs,mxArray *plhs[],int nrhs,const mxArray *prhs[])
{
    char *filename;
    
    if (nrhs != 1) { 
	    mexErrMsgIdAndTxt( "MATLAB:mxmalloc:invalidNumInputs", 
                "One input argument required.");
    } 
    if (nlhs != 4) {
	    mexErrMsgIdAndTxt( "MATLAB:MXMALLOC:maxlhs",
                "The number of output arguments should be four.");
    }
    if (!mxIsChar(prhs[0]) || (mxGetM(prhs[0]) != 1 ) )  {
	    mexErrMsgIdAndTxt( "MATLAB:mxmalloc:invalidInput", 
                "Input argument must be a string.");
    }

    blfInit();

    filename=mxArrayToString(prhs[0]);
    
    fp = fopen(filename, "rb");
    fseek(fp, 0, SEEK_END);
    filelen = ftell(fp);

    fseek(fp, 0, SEEK_SET);
    fread(&logg, sizeof(LOGG_t), 1, fp);

    
    // plhs[0]: candata
    plhs[0] = mxCreateDoubleMatrix (8,logg.objectCount , mxREAL);
    candata = mxGetPr(plhs[0]);
    
    // plhs[1]: canmsgid
    plhs[1] = mxCreateDoubleMatrix (1,logg.objectCount , mxREAL);
    canmsgid = mxGetPr(plhs[1]);
    
    // plhs[2]: canchannel
    plhs[2] = mxCreateDoubleMatrix (1,logg.objectCount , mxREAL);
    canchannel = mxGetPr(plhs[2]);
    
    // plhs[3]: camtime
    plhs[3] = mxCreateDoubleMatrix (1,logg.objectCount , mxREAL);
    cantime = mxGetPr(plhs[3]);

    while(blfPeekObject())
        blfReadObjectSecure();
    
    #ifdef BLFDEBUG
    mexPrintf("The input string is:  %s\n", filename);
    mexPrintf("The file length is:  %u\n", filelen);
    mexPrintf("mSignature:  %X\n", logg.mSignature);
    mexPrintf("mHeaderSize:  %u\n", logg.mHeaderSize);
    mexPrintf("mCRC:  %u\n", logg.mCRC);
    mexPrintf("appID:  %u\n", logg.appID);
    mexPrintf("dwCompression:  %u\n", logg.dwCompression);
    mexPrintf("appMajor:  %u\n", logg.appMajor);
    mexPrintf("appMinor:  %u\n", logg.appMinor);
    mexPrintf("fileSize:  %u\n", logg.fileSize);
    mexPrintf("uncompressedFileSize:  %u\n", logg.uncompressedFileSize);
    mexPrintf("objectCount:  %u\n", logg.objectCount);
    mexPrintf("appBuild:  %u\n", logg.appBuild);
    #endif

    mxSetN(plhs[0], rcnt);
    mxSetN(plhs[1], rcnt);
    mxSetN(plhs[2], rcnt);
    mxSetN(plhs[3], rcnt);

    fclose(fp);
    mxFree(filename);
}
