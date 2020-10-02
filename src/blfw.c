#include <stdint.h>
#include <windows.h>
#include "mex.h"
#include "blfw.h"
#include "zlib.h"

static char *filename = NULL;
static FILE *fp = NULL;
static long int midx = 0;
static long int lidx = 0;
static LOGG_t logg;
//static VBLObjectHeaderBase Base;
static VBLObjectHeaderContainer container;
static VBLCANMessage message;
static uint32_t rcnt = 0;

static uint8_t *compressedData = NULL;
static uint32_t compressedSize = 0;
static uint8_t *unCompressedData = NULL;
static uint32_t unCompressedSize = 0;
static uint8_t *restData = NULL;
static uint32_t restSize = 0;

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
    //container.mBase.mObjectSize = ???;
}


void blfInit(void){
    filename = NULL;
    fp = NULL;
    //
    memset(&logg, 0, BL_LOGG_SIZE);
    loggInit();
    //
    memset(&logg, 0, BL_MESSAGE_SIZE);
    messageInit();
    //
    memset(&container, 0, BL_HEADER_CONTAINER_SIZE);
    containerInit();
    //
    rcnt = 0;
    compressedData = NULL;
    compressedSize = 0;
    unCompressedData = NULL;
    unCompressedSize = 0;
    restData = NULL;
    restSize = 0;
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
    
    fp = fopen(filename, "wb");
    if(fp==NULL){
        mexErrMsgIdAndTxt( "MATLAB:blfc:fileCannotWrite", 
                "%s: %s.",
                strerror(errno),
                filename);
    }

    candata = mxGetPr(prhs[1]);
    cantime = mxGetPr(prhs[2]);
    canmsgid = mxGetPr(prhs[3]);
    canchannel = mxGetPr(prhs[4]);

    fclose(fp);
    mxFree(filename);
}
