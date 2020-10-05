#include <stdint.h>
#include <windows.h>
#include "mex.h"
#include "blfw.h"
#include "zlib.h"

static char *filename = NULL;
static FILE *fp = NULL;
static LOGG_t logg;

static uint8_t contFlag = 0;
static VBLObjectHeaderContainer container;
static VBLCANMessage message;

static uint32_t rcnt = 0;
static uint32_t objectCounts[4] = {0, 0, 0, 0};

static uint8_t compressedData[BL_CHUNK];
static uint32_t compressedSize = 0;
static uint8_t unCompressedData[BL_CHUNK];
static uint32_t unCompressedSize = 0;
static uint32_t thisSize = 0;
static uint32_t restSize = 0;
static uint8_t paddingBytes[3] = {0, 0, 0};
static uint32_t paddingSize = 0;

static uint8_t i;

static double *candata, *cantime, *canmsgid, *canchannel;

extern int errno;


void loggInit(void){
    logg.mSignature = BL_LOGG_SIGNATURE;
    logg.mHeaderSize = BL_LOGG_SIZE;
    logg.mCRC = 0;
    logg.mApplicationID = BL_APPID_BLFLOAD;
    logg.mdwCompression = 0;
    logg.mApplicationMajor = BL_Major;
    logg.mApplicationMinor = BL_Minor;
    logg.mApplicationBuild = BL_AppBuild;
    //
    logg.mFileSize = BL_LOGG_SIZE;
    logg.mUncompressedFileSize = BL_LOGG_SIZE;
    logg.mObjectCount = 0;
}


void messageInit(void){
    message.mHeader.mBase.mSignature = BL_OBJ_SIGNATURE;
    message.mHeader.mBase.mHeaderSize = BL_HEADER_SIZE;
    message.mHeader.mBase.mHeaderVersion = 1;
    message.mHeader.mBase.mObjectSize = BL_MESSAGE_SIZE;
    message.mHeader.mBase.mObjectType = BL_OBJ_TYPE_CAN_MESSAGE;
    //
    message.mHeader.mObjectFlags = BL_OBJ_FLAG_TIME_ONE_NANS;
    //
    message.mDLC = 8;
}


void containerInit(void){
    container.mBase.mSignature = BL_OBJ_SIGNATURE;
    container.mBase.mHeaderSize = BL_HEADER_BASE_SIZE;
    container.mBase.mHeaderVersion = 1;
    container.mBase.mObjectType = BL_OBJ_TYPE_LOG_CONTAINER;
    //
    container.mCompressedflag = 2;
}


void blfInit(void){
    filename = NULL;
    fp = NULL;
    //
    memset(&logg, 0, BL_LOGG_SIZE);
    loggInit();
    //
    memset(&message, 0, BL_MESSAGE_SIZE);
    messageInit();
    //
    memset(&container, 0, BL_HEADER_CONTAINER_SIZE);
    containerInit();
    //
    contFlag = 0;
    rcnt = 0;
    objectCounts[0] = 0;
    objectCounts[1] = 0;
    objectCounts[2] = 0;
    objectCounts[3] = 0;
    //
    compressedSize = 0;
    unCompressedSize = 0;
    thisSize = 0;
    restSize = 0;
    paddingSize = 0;
}


void blfWriteObjectInternal(void){
    compressedSize = BL_CHUNK;
    compress(compressedData, &compressedSize, unCompressedData, unCompressedSize);
    container.mBase.mObjectSize = BL_HEADER_CONTAINER_SIZE + compressedSize;
    container.mDeflatebuffersize = unCompressedSize;
    //
    fwrite(&container, BL_HEADER_CONTAINER_SIZE, 1, fp);
    fwrite(compressedData, compressedSize, 1, fp);
    paddingSize = compressedSize & 3;
    if(paddingSize > 0){
        fwrite(paddingBytes, paddingSize, 1, fp);
    }
    //
    logg.mFileSize += BL_HEADER_CONTAINER_SIZE + compressedSize + paddingSize;
    logg.mUncompressedFileSize += BL_HEADER_CONTAINER_SIZE + unCompressedSize;
    //
    compressedSize = 0;
    unCompressedSize = 0;
}


uint8_t blfWriteObject(void){
    while(rcnt < objectCounts[0]){
        if(contFlag){
            memcpy(((uint8_t *)unCompressedData) + unCompressedSize, &message + thisSize, restSize);
            contFlag = 0;
            rcnt++;
        }else{
            message.mHeader.mObjectTimeStamp = (uint64_t)((*(cantime + rcnt))*1000000000);
            message.mChannel = (uint16_t)(*(canchannel + rcnt));
            message.mID = (uint32_t)(*(canmsgid + rcnt));
            for(i=0;i<8;i++){
                message.mData[i] = (uint8_t)(*(candata + rcnt*8 +i));
            }
            if(unCompressedSize + BL_MESSAGE_SIZE <= BL_CHUNK){
                memcpy(((uint8_t *)unCompressedData) + unCompressedSize, &message, BL_MESSAGE_SIZE);
                unCompressedSize += BL_MESSAGE_SIZE;
                rcnt++;
            }else{
                thisSize = BL_CHUNK - unCompressedSize;
                restSize = unCompressedSize + BL_MESSAGE_SIZE - BL_CHUNK;
                if(thisSize==0){
                    contFlag = 1;
                    continue;
                }
                memcpy(((uint8_t *)unCompressedData) + unCompressedSize, &message, thisSize);
                contFlag = 1;
            }
        }
        if(contFlag){
            blfWriteObjectInternal();
        }
    }
    //
    if(unCompressedSize > 0){
        blfWriteObjectInternal();
    }
}


void
mexFunction(int nlhs,mxArray *plhs[],int nrhs,const mxArray *prhs[])
{
    if (nrhs != 5) { 
      mexErrMsgIdAndTxt( "MATLAB:blfc:invalidNumInputs", 
                "Five inputs argument required.");
    } 
    if (nlhs > 1) {
      mexErrMsgIdAndTxt( "MATLAB:blfc:maxlhs",
                "The number of output arguments should be zero or one.");
    }
    if (!mxIsChar(prhs[0]) || (mxGetM(prhs[0]) != 1 ) )  {
      mexErrMsgIdAndTxt( "MATLAB:blfc:invalidInput", 
                "The first argument must be a string.");
    }
    
    mexPrintf("\n\t%s %s\n\n",
             "BlfLoad2 by Shen, Chenghao", "<snc6si@gmail.com>.");
    mexEvalString("drawnow;");
    
    blfInit();

    filename = mxArrayToString(prhs[0]);

    candata = mxGetPr(prhs[1]);
    canmsgid = mxGetPr(prhs[2]);
    canchannel = mxGetPr(prhs[3]);
    cantime = mxGetPr(prhs[4]);

    objectCounts[0] = (uint32_t)mxGetN(prhs[1]);
    objectCounts[1] = (uint32_t)mxGetN(prhs[2]);
    objectCounts[2] = (uint32_t)mxGetN(prhs[3]);
    objectCounts[3] = (uint32_t)mxGetN(prhs[4]);

    fp = fopen(filename, "wb");
    if(fp==NULL){
        mexErrMsgIdAndTxt( "MATLAB:blfc:fileCannotWrite", 
                "%s: %s.",
                strerror(errno),
                filename);
    }

    fseek(fp, BL_LOGG_SIZE, SEEK_SET);
    blfWriteObject();

    logg.mObjectCount = rcnt;
    fseek(fp, 0, SEEK_SET);
    fwrite(&logg, BL_LOGG_SIZE, 1, fp);

    fclose(fp);
    mxFree(filename);
}
