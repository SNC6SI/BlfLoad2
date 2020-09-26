#include <stdint.h>
#include <windows.h>
#include "mex.h"
#include "blfc.h"

//#define BLFDEBUG

static FILE *fp=NULL;
static long int filelen=0;
static long int minx=0;
static LOGG_t logg;

void
mexFunction(int nlhs,mxArray *plhs[],int nrhs,const mxArray *prhs[])
{
    (void) plhs;
    char *filename;
    
    if (nrhs != 1) { 
	    mexErrMsgIdAndTxt( "MATLAB:mxmalloc:invalidNumInputs", 
                "One input argument required.");
    } 
    if (nlhs > 1) {
	    mexErrMsgIdAndTxt( "MATLAB:MXMALLOC:maxlhs",
                "Too many output arguments.");
    }
    if (!mxIsChar(prhs[0]) || (mxGetM(prhs[0]) != 1 ) )  {
	    mexErrMsgIdAndTxt( "MATLAB:mxmalloc:invalidInput", 
                "Input argument must be a string.");
    }


    filename=mxArrayToString(prhs[0]);
    

    fp = fopen(filename, "rb");
    fseek(fp, 0, SEEK_END);
    filelen = ftell(fp);

    fseek(fp, 0, SEEK_SET);
    fread(&logg, sizeof(LOGG_t), 1, fp);


    fclose(fp);
    mxFree(filename);
    

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
}
