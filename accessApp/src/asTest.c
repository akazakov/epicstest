/* asTest */

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include <epicsThread.h>
#include <iocsh.h>
#include <dbBase.h>
#include <dbAccess.h>
#include <cantProceed.h>
#include <ellLib.h>
#include <asLib.h>
#include <asDbLib.h>
#include <dbCommon.h>
#include <dbTest.h>

#include <epicsExport.h>

typedef struct asTestPvt {
    FILE     *fp;
    dbAddr   dbaddr;
    ASCLIENTPVT asclientpvt;
    char     *user;
    char     *location;
}asTestPvt;

typedef struct test {
    char *pvname;
    char *user;
    char *location;
    asTestPvt *pasTestPvt;
}test;

/*host must be calloc memory because tolower is performed on it*/

const char *host[] = {"uranus","uranus","xyz","uranus","uranus"};
static test testArray[] = {
    {"mrkLOCALprotected","anj",0,0},
    {"mrkLOCALprotected.HIHI","anj",0,0},
    {"mrkLOCALprotected","xyz",0,0},
    {"mrkLOCALasg3","anj",0,0},
    {"mrkLOCALdefault","anj",0,0},
};


static void astacCallback(ASCLIENTPVT clientPvt,asClientStatus status)
{
    asTestPvt *pasTestPvt = (asTestPvt *)asGetClientPvt(clientPvt);
    dbAddr    *pdbAddr = &pasTestPvt->dbaddr;
    dbCommon  *precord = (dbCommon *)pdbAddr->precord;
    dbFldDes  *pfldDes = (dbFldDes *)pdbAddr->pfldDes;
    FILE      *fp = pasTestPvt->fp;

    fprintf(fp,"%s.%s %s %s status=%d",
        precord->name,pfldDes->name,
        pasTestPvt->user,pasTestPvt->location,status);
    fprintf(fp," get %s put %s\n",(asCheckGet(clientPvt) ? "Yes" : "No"),
        (asCheckPut(clientPvt) ? "Yes" : "No"));
}

static void addClient(asTestPvt *pasTestPvt,char *pname,char *user,char *location,FILE *fp)
{
    dbAddr      *pdbAddr;
    long        status;
    ASCLIENTPVT *pasclientpvt;
    dbCommon    *precord;
    dbFldDes    *pflddes;

    pasTestPvt->fp = fp;
    pasTestPvt->user = user;
    pasTestPvt->location = location;
    pdbAddr = &pasTestPvt->dbaddr;
    pasclientpvt = &pasTestPvt->asclientpvt;
    status=dbNameToAddr(pname,pdbAddr);
    if(status) {
        errMessage(status,"dbNameToAddr error");
        return;
    }
    precord = pdbAddr->precord;
    pflddes = pdbAddr->pfldDes;
    status = asAddClient(pasclientpvt,(ASMEMBERPVT)precord->asp,
        (int)pflddes->as_level,user,location);
    if(status) {
        errMessage(status,"asAddClient error");
        return;
    } else {
        asPutClientPvt(*pasclientpvt,(void *)pasTestPvt);
        asRegisterClientCallback(*pasclientpvt,astacCallback);
    }
    return;
}

static int asTestThread(char *filename)
{
    FILE *fp;
    int  fd;
    int  i,n;

    asSetFilename("../../accessApp/acf/syntax.acf");
    dbpf("mrkLOCALrestartAS","1");
    epicsThreadSleep(1.0);
    errno = 0;
    fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644 );
    if(fd<0) {
        fprintf(stderr,"%s could not be created\n",filename);
        return(0);
    }
    fp = fdopen(fd,"w");
    if(!fp) {
         fprintf(stderr,"file %s could not be opened for output %s\n",
             filename,strerror(errno));
         return 0;
    }

    fprintf(fp,"\n#Test Syntax with syntax.acf\n");
    n = sizeof(testArray)/sizeof(test);
    for(i=0; i<n; i++) {
        test *ptest = &testArray[i];
        const char *phost = host[i];
        asTestPvt *pasTestPvt;

        pasTestPvt = callocMustSucceed(1,sizeof(asTestPvt), "asTestThread");
        ptest->location = callocMustSucceed(strlen(phost)+1,sizeof(char),
            "asTestThread");
        strcpy(ptest->location ,phost);
        ptest->pasTestPvt = pasTestPvt;
        addClient(pasTestPvt,ptest->pvname,ptest->user,ptest->location,fp);
    }
    epicsThreadSleep(1.0);
    asdbdumpFP(fp);

    fprintf(fp,"\n#Test default.acf\n");
    asSetFilename("../../accessApp/acf/default.acf");
    dbpf("mrkLOCALrestartAS","1");
    epicsThreadSleep(1.0);
    asdbdumpFP(fp);

    fprintf(fp,"\n#Test local.acf\n");
    asSetFilename("../../accessApp/acf/local.acf");
    dbpf("mrkLOCALrestartAS","1");
    epicsThreadSleep(1.0);
    asdbdumpFP(fp);

    fprintf(fp,"\n#Set mrkLOCALhostAccess No Access\n");
    dbpf("mrkLOCALhostAccess","No Access");
    epicsThreadSleep(1.0);
    asprulesFP(fp,"protected");
    aspmemFP(fp,"protected",1);

    fprintf(fp,"\n#Set mrkLOCALhostAccess Read Access\n");
    dbpf("mrkLOCALhostAccess","Read Access");
    epicsThreadSleep(1.0);
    asprulesFP(fp,"protected");
    aspmemFP(fp,"protected",1);

    fprintf(fp,"\n#Set mrkLOCALhostAccess L0 Write Access\n");
    dbpf("mrkLOCALhostAccess","L0 Write Access");
    epicsThreadSleep(1.0);
    asprulesFP(fp,"protected");
    aspmemFP(fp,"protected",1);

    fprintf(fp,"\n#Set mrkLOCALhostAccess L1 Write Access\n");
    dbpf("mrkLOCALhostAccess","L1 Write Access");
    epicsThreadSleep(1.0);
    asprulesFP(fp,"protected");
    aspmemFP(fp,"protected",1);
    epicsThreadSleep(1.0);
    for(i=0; i<n; i++) {
        long status;

        test *ptest = &testArray[i];
        status = asRemoveClient(&ptest->pasTestPvt->asclientpvt);
        if(status) errMessage(status,"asRemoveClient error");
        free(ptest->location);
        free(ptest->pasTestPvt);
    }
    fclose(fp);
    free(filename);
    return(0);
}

static const iocshArg asTestArg0 = {"filename", iocshArgString};
static const iocshArg *const asTestArgs[] = {&asTestArg0};
static const iocshFuncDef asTestDef = {"asTest", 1,asTestArgs};

int asTest(char *filename)
{
    char *filenamecopy;
    epicsThreadId id;

    filenamecopy = callocMustSucceed(1,strlen(filename)+1,"asTestCall");
    strcpy(filenamecopy,filename);
    id = epicsThreadCreate("asTest",
        epicsThreadPriorityLow,epicsThreadGetStackSize(epicsThreadStackMedium),
        (EPICSTHREADFUNC)asTestThread,(void *)filenamecopy);
    if(!id) printf("asTestCall epicsThreadCreate failed\n");
    return 0;
}

static void asTestCall(const iocshArgBuf * args) {
    asTest(args[0].sval);
}

static void asTestRegister(void)
{
    static int firstTime = 1;
    if(!firstTime) return;
    firstTime = 0;
    iocshRegister(&asTestDef,asTestCall);
}
epicsExportRegistrar(asTestRegister);
