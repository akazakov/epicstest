/* accessLocalMain.c */
/* Author:  Marty Kraimer Date:    23MAR2000 */

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

/* asinittest */
extern "C" {
extern int asinittest(char *asfile1,char *asfile2);
static iocshArg const asinittestArg0 = { "asf1",iocshArgString};
static iocshArg const asinittestArg1 = { "asf2",iocshArgString};
static iocshArg const *asinittestArgs[2] = {&asinittestArg0,&asinittestArg1};
static iocshFuncDef asinittestFuncDef = {"asinittest",2,asinittestArgs};
static void asinittestCallFunc(const iocshArgBuf *args)
{
    asinittest(args[0].sval,args[1].sval);
}

}/*extern "C" */

int main(int argc,char *argv[])
{
    iocshRegister(&asinittestFuncDef,asinittestCallFunc);
    if(argc>=2) {
        if (iocsh(argv[1]) < 0)
            return(1);
        epicsThreadSleep(.2);
    }
    iocsh(NULL);
    epicsExit(0);
    return(0);
}
