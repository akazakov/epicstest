/* performMain.c */
/* Author:  Marty Kraimer Date:    09JUN2000 */

/********************COPYRIGHT NOTIFICATION**********************************
This software was developed under a United States Government license
described on the COPYRIGHT_UniversityOfChicago file included as part
of this distribution.
****************************************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "epicsExit.h"
#include "epicsThread.h"
#include "dbAccess.h"
#include "errlog.h"
#include "asLib.h"
#include "asDbLib.h"
#include "asCa.h"
#include "dbTest.h"
#include "registryRecordType.h"
#include "iocsh.h"
#include "iocshRegisterCommon.h"
/*#include "registerRecordDeviceDriverRegister.h"*/

extern "C" {
/* changeLinkPerform */
int changeLinkPerform(int ntimes);
static iocshArg const changeLinkPerformArg0 = { "ntimes",iocshArgInt};
static iocshArg const*changeLinkPerformArgs[1] = {&changeLinkPerformArg0};
static const iocshFuncDef changeLinkPerformFuncDef =
    {"changeLinkPerform",1,changeLinkPerformArgs};
static void changeLinkPerformCallFunc(const iocshArgBuf *args)
{
    changeLinkPerform(args[0].ival);
}

/* changeLinkContinuous */
int changeLinkContinuous(void);
static iocshFuncDef changeLinkContinuousFuncDef = {"changeLinkContinuous",0,0};
static void changeLinkContinuousCallFunc(const iocshArgBuf *args)
{
    changeLinkContinuous();
}

/* timeSem */
int timeSem(void);
static iocshFuncDef timeSemFuncDef = {"timeSem",0,0};
static void timeSemCallFunc(const iocshArgBuf *args)
{
    timeSem();
}

/* timedbNameToAddr */
int timedbNameToAddr(char *filename,int ntime);
static iocshArg const timedbNameToAddrArg0 = { "pvname",iocshArgString};
static iocshArg const timedbNameToAddrArg1 = { "ntimes",iocshArgInt};
static iocshArg const*timedbNameToAddrArgs[2] = {&timedbNameToAddrArg0,&timedbNameToAddrArg1};
static iocshFuncDef timedbNameToAddrFuncDef = {"timedbNameToAddr",2,timedbNameToAddrArgs};
static void timedbNameToAddrCallFunc(const iocshArgBuf *args)
{
    timedbNameToAddr(args[0].sval,args[1].ival);
}

/* timeproc */
int timeproc(char *pvname,int ntime);
static iocshArg const timeprocArg0 = { "pvname",iocshArgString};
static iocshArg const timeprocArg1 = { "ntimes",iocshArgInt};
static iocshArg const*timeprocArgs[2] = {&timeprocArg0,&timeprocArg1};
static iocshFuncDef timeprocFuncDef = {"timeproc",2,timeprocArgs};
static void timeprocCallFunc(const iocshArgBuf *args)
{
    timeproc(args[0].sval,args[1].ival);
}
} // extern "C"

int main(int argc,char *argv[])
{
    iocshRegister(&changeLinkPerformFuncDef,changeLinkPerformCallFunc);
    iocshRegister(&changeLinkContinuousFuncDef,changeLinkContinuousCallFunc);
    iocshRegister(&timeSemFuncDef,timeSemCallFunc);
    iocshRegister(&timedbNameToAddrFuncDef,timedbNameToAddrCallFunc);
    iocshRegister(&timeprocFuncDef,timeprocCallFunc);
    if(argc>=2) {
        if (iocsh(argv[1]) < 0)
            return(1);
        epicsThreadSleep(.2);
    }
    iocsh(NULL);
    epicsExit(0);
    return(0);
}
