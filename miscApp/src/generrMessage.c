#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
 
#include <errlog.h>
#include <epicsExport.h>
#include <epicsThread.h>
#include <iocsh.h>
#define epicsExportSharedSymbols

static void generrMessage(void *pvt)
{
    int okToBlock = *(int *)pvt;
    int number=0;

    epicsThreadSetOkToBlock(okToBlock);
    while(1) {
	errMessage(number,"errMessage");
	epicsThreadSleep(1.0);
	++number;
    }
}

static const iocshArg generrMessageArg0 = {"okToBlock", iocshArgInt};
static const iocshArg *const generrMessageArgs[] = {&generrMessageArg0};
static const iocshFuncDef generrMessageDef = {
    "generrMessage",1,generrMessageArgs};
static void generrMessageCall(const iocshArgBuf * args)
{
    int *pint;

    pint = calloc(1,sizeof(int));
    *pint = args[0].ival;
    epicsThreadCreate("generrMessage",
        epicsThreadPriorityLow,epicsThreadGetStackSize(epicsThreadStackSmall),
        (EPICSTHREADFUNC)generrMessage,pint);
}
static void generrMessageRegister(void)
{
    static int firstTime = 1;
    if(!firstTime) return;
    firstTime = 0;
    iocshRegister(&generrMessageDef,generrMessageCall);
}
epicsExportRegistrar(generrMessageRegister);
