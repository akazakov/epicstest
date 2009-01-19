#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <errlog.h>
#include <epicsExport.h>
#include <epicsThread.h>
#include <iocsh.h>
#define epicsExportSharedSymbols


static void errlogTest(void *pvt)
{
    int okToBlock = *(int *)pvt;
    int i;
    char  badmsg[300];

    epicsThreadSetOkToBlock(okToBlock);
    for(i=0; i<300; i++) sprintf(&badmsg[i],"%1.1d",i%10);
    while(1) {
	for(i=0; i<=errlogFatal; i++) {
	    errlogSevPrintf(i,"errlogTest");
	}
	errlogMessage(badmsg);
	epicsThreadSleep(1.0);
    }
}

static const iocshArg errlogTestArg0 = {"okToBlock", iocshArgInt};
static const iocshArg *const errlogTestArgs[] = {&errlogTestArg0};
static const iocshFuncDef errlogTestDef = {
    "errlogTest",1,errlogTestArgs};
static void errlogTestCall(const iocshArgBuf * args)
{
    int *pint;

    pint = calloc(1,sizeof(int));
    *pint = args[0].ival;
    epicsThreadCreate("errlogTest",
        epicsThreadPriorityLow,epicsThreadGetStackSize(epicsThreadStackSmall),
        (EPICSTHREADFUNC)errlogTest,pint);
}
static void errlogTestRegister(void)
{
    static int firstTime = 1;
    if(!firstTime) return;
    firstTime = 0;
    iocshRegister(&errlogTestDef,errlogTestCall);
}
epicsExportRegistrar(errlogTestRegister);

