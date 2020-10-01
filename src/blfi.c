#include <stdint.h>
#include <windows.h>
#include "mex.h"
#include "blfi.h"

#define NFIELD_DATE sizeof(date)/sizeof(*date)

static char *filename = NULL;
static FILE *fp = NULL;
static LOGG_t logg;
static VBLFileStatisticsEx pStat;

extern int errno;


void blfInit(void){
    filename = NULL;
    fp = NULL;
}


void blfStatisticsFromLogg(void){
    pStat.mApplicationID = logg.mApplicationID;
    pStat.mApplicationMajor = logg.mApplicationMajor;
    pStat.mApplicationMinor = logg.mApplicationMinor;
    pStat.mApplicationBuild = logg.mApplicationBuild;
    pStat.mFileSize = logg.mFileSize;
    pStat.mUncompressedFileSize = logg.mUncompressedFileSize;
    pStat.mObjectCount = logg.mObjectCount;
    pStat.mMeasurementStartTime = logg.mMeasurementStartTime;
    pStat.mLastObjectTime = logg.mLastObjectTime;
}


void mexFunction(int nlhs,mxArray *plhs[],int nrhs,const mxArray *prhs[])
{
    const char* date[] = {"Year",
                          "Month",
                          "Day",
                          "Hour",
                          "Minute",
                          "Second",
                          "Milliseconds"};
    mxArray* temp_value;

    if (nrhs != 1) { 
	    mexErrMsgIdAndTxt( "MATLAB:blfi:invalidNumInputs", 
                "One input argument required.");
    }
    #if 0
    if (nlhs != 4) {
	    mexErrMsgIdAndTxt( "MATLAB:blfi:maxlhs",
                "The number of output arguments should be four.");
    }
    #endif
    if (!mxIsChar(prhs[0]) || (mxGetM(prhs[0]) != 1 ) )  {
	    mexErrMsgIdAndTxt( "MATLAB:blfi:invalidInput", 
                "The first argument must be a string.");
    }

    blfInit();

    filename = mxArrayToString(prhs[0]);
    
    fp = fopen(filename, "rb");
    if(fp==NULL){
        mexErrMsgIdAndTxt( "MATLAB:blfi:fileNotFound", 
                "%s: %s.",
                strerror(errno),
                filename);
    }

    fread(&logg, sizeof(LOGG_t), 1, fp);
    blfStatisticsFromLogg();
    fclose(fp);

    plhs[0] = mxCreateStructMatrix(2, 1, NFIELD_DATE, &date);

    temp_value = mxCreateDoubleMatrix(1, 1, mxREAL);
    *mxGetPr(temp_value) = (double)pStat.mMeasurementStartTime.wYear;
    mxSetFieldByNumber(plhs[0], 0, 0, temp_value);
    temp_value = mxCreateDoubleMatrix(1, 1, mxREAL);
    *mxGetPr(temp_value) = (double)pStat.mMeasurementStartTime.wMonth;
    mxSetFieldByNumber(plhs[0], 0, 1, temp_value);
    temp_value = mxCreateDoubleMatrix(1, 1, mxREAL);
    *mxGetPr(temp_value) = (double)pStat.mMeasurementStartTime.wDay;
    mxSetFieldByNumber(plhs[0], 0, 2, temp_value);
    temp_value = mxCreateDoubleMatrix(1, 1, mxREAL);
    *mxGetPr(temp_value) = (double)pStat.mMeasurementStartTime.wHour;
    mxSetFieldByNumber(plhs[0], 0, 3, temp_value);
    temp_value = mxCreateDoubleMatrix(1, 1, mxREAL);
    *mxGetPr(temp_value) = (double)pStat.mMeasurementStartTime.wMinute;
    mxSetFieldByNumber(plhs[0], 0, 4, temp_value);
    temp_value = mxCreateDoubleMatrix(1, 1, mxREAL);
    *mxGetPr(temp_value) = (double)pStat.mMeasurementStartTime.wSecond;
    mxSetFieldByNumber(plhs[0], 0, 5, temp_value);
    temp_value = mxCreateDoubleMatrix(1, 1, mxREAL);
    *mxGetPr(temp_value) = (double)pStat.mMeasurementStartTime.wMilliseconds;
    mxSetFieldByNumber(plhs[0], 0, 6, temp_value);

    temp_value = mxCreateDoubleMatrix(1, 1, mxREAL);
    *mxGetPr(temp_value) = (double)pStat.mLastObjectTime.wYear;
    mxSetFieldByNumber(plhs[0], 1, 0, temp_value);
    temp_value = mxCreateDoubleMatrix(1, 1, mxREAL);
    *mxGetPr(temp_value) = (double)pStat.mLastObjectTime.wMonth;
    mxSetFieldByNumber(plhs[0], 1, 1, temp_value);
    temp_value = mxCreateDoubleMatrix(1, 1, mxREAL);
    *mxGetPr(temp_value) = (double)pStat.mLastObjectTime.wDay;
    mxSetFieldByNumber(plhs[0], 1, 2, temp_value);
    temp_value = mxCreateDoubleMatrix(1, 1, mxREAL);
    *mxGetPr(temp_value) = (double)pStat.mLastObjectTime.wHour;
    mxSetFieldByNumber(plhs[0], 1, 3, temp_value);
    temp_value = mxCreateDoubleMatrix(1, 1, mxREAL);
    *mxGetPr(temp_value) = (double)pStat.mLastObjectTime.wMinute;
    mxSetFieldByNumber(plhs[0], 1, 4, temp_value);
    temp_value = mxCreateDoubleMatrix(1, 1, mxREAL);
    *mxGetPr(temp_value) = (double)pStat.mLastObjectTime.wSecond;
    mxSetFieldByNumber(plhs[0], 1, 5, temp_value);
    temp_value = mxCreateDoubleMatrix(1, 1, mxREAL);
    *mxGetPr(temp_value) = (double)pStat.mLastObjectTime.wMilliseconds;
    mxSetFieldByNumber(plhs[0], 1, 6, temp_value);

    mxFree(filename);
}
