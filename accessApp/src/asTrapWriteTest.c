/*asTrapWriteTest */
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include <epicsThread.h>
#include <epicsEvent.h>
#include <cantProceed.h>
#include <iocsh.h>
#include <dbAccess.h>
#include <ellLib.h>
#include <asLib.h>
#include <asDbLib.h>
#include <asTrapWrite.h>
#include <asLib.h>
#include <dbCommon.h>
#include <dbTest.h>

#include <epicsExport.h>

#define NTIMES 5

typedef struct asTrapWritePvt {
    asTrapWriteId id;
    int           whichListener;
    int           ncalls;
    int           listener;
    FILE          *fp;
}asTrapWritePvt;

static asTrapWritePvt asTrapWritePvt1;
static asTrapWritePvt asTrapWritePvt2;
static int nactive = 2;
epicsEventId done;

static void listener(asTrapWritePvt *pasTrapWritePvt,
    asTrapWriteMessage *pmessage,int after)
{
    dbAddr *pdbAddr = (dbAddr *)pmessage->serverSpecific;
    dbCommon  *precord = (dbCommon *)pdbAddr->precord;
    FILE *fp = pasTrapWritePvt->fp;
    double    value = 0.0;
    long      status;

    status = dbGet(pdbAddr,DBR_DOUBLE,&value,0,0,0);
    if(status) fprintf(fp,"dbGet failed\n");
    fprintf(fp,"listener%d %s %s %s %s %f\n",
        pasTrapWritePvt->whichListener,
        (after ? "after" : "before"),
        precord->name,pmessage->userid,pmessage->hostid,value);
    if(!after) {
        pmessage->userPvt = pasTrapWritePvt;
        return;
    }
    if(pmessage->userPvt!=pasTrapWritePvt) {
        fprintf(fp,"after but pmessage->userPvt!=pasTrapWritePvt\n");
    }
    pasTrapWritePvt->ncalls++;
    if(pasTrapWritePvt->ncalls==NTIMES) {
       if(--nactive<=0) epicsEventSignal(done);
    }
}

static void listener1(asTrapWriteMessage *pmessage,int after)
{
    listener(&asTrapWritePvt1,pmessage,after);
}

static void listener2(asTrapWriteMessage *pmessage,int after)
{
    listener(&asTrapWritePvt2,pmessage,after);
}

static int asTrapWriteTestThread(char *filename)
{
    FILE *fp;

    asSetFilename("../../accessApp/acf/allowall.acf");
    dbpf("mrkLOCALrestartAS","1");
    epicsThreadSleep(1.0);
    dbpf("mrkLOCALprotected","0.0");
    epicsThreadSleep(1.0);
    errno = 0;
    fp = fopen(filename,"w");
    if(!fp) {
         fprintf(stderr,"file %s could not be opened for output %s\n",
             filename,strerror(errno));
         return 0;
    }
    done = epicsEventMustCreate(epicsEventEmpty);
    asTrapWritePvt1.fp = fp;
    asTrapWritePvt1.ncalls = 0;
    asTrapWritePvt2.fp = fp;
    asTrapWritePvt2.ncalls = 0;
    asTrapWritePvt1.whichListener = 1;
    asTrapWritePvt2.whichListener = 2;
    nactive = 2;
    asTrapWritePvt1.id = asTrapWriteRegisterListener(listener1);
    asTrapWritePvt2.id = asTrapWriteRegisterListener(listener2);
    epicsEventMustWait(done);
    fprintf(fp,"asTrapWriteTest done\n");
    asTrapWriteUnregisterListener(asTrapWritePvt1.id);
    asTrapWriteUnregisterListener(asTrapWritePvt2.id);
    fclose(fp);
    epicsEventDestroy(done);
    free(filename);
    return 0;
}

static const iocshArg asTrapWriteTestArg0 = {"filename", iocshArgString};
static const iocshArg *const asTrapWriteTestArgs[] = {&asTrapWriteTestArg0};
static const iocshFuncDef asTrapWriteTestDef = {"asTrapWriteTest", 1,asTrapWriteTestArgs};

int asTrapWriteTest(char *filename)
{
    char *filenamecopy;
    epicsThreadId id;

    filenamecopy = callocMustSucceed(1,strlen(filename)+1,"asTrapWriteTestCall");
    strcpy(filenamecopy,filename);
    id = epicsThreadCreate("trapWrite",
        epicsThreadPriorityLow,epicsThreadGetStackSize(epicsThreadStackMedium),
        (EPICSTHREADFUNC)asTrapWriteTestThread,(void *)filenamecopy);
    if(!id) printf("asTrapWriteTestCall epicsThreadCreate failed\n");
    return 0;
}

static void asTrapWriteTestCall(const iocshArgBuf * args) {
    asTrapWriteTest(args[0].sval);
}

static void asTrapWriteTestRegister(void)
{
    static int firstTime = 1;
    if(!firstTime) return;
    firstTime = 0;
    iocshRegister(&asTrapWriteTestDef,asTrapWriteTestCall);
}
epicsExportRegistrar(asTrapWriteTestRegister);

