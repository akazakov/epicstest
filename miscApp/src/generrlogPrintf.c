#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <errlog.h>
#include <epicsExport.h>
#include <epicsThread.h>
#include <iocsh.h>
#define epicsExportSharedSymbols

static void generrlogPrintf(void *pvt)
{
    int delay = *(int *)pvt;
    int number=0;

    if(delay<=.1) delay = .1;
    while(1) {
	errlogPrintf("errlogPrintf %d\n",number);
	epicsThreadSleep((double)delay);
	++number;
    }
}

static const iocshArg generrlogPrintfArg0 = {"okToBlock", iocshArgInt};
static const iocshArg *const generrlogPrintfArgs[] = {&generrlogPrintfArg0};
static const iocshFuncDef generrlogPrintfDef = {
    "generrlogPrintf",1,generrlogPrintfArgs};
static void generrlogPrintfCall(const iocshArgBuf * args)
{
    int *pint;

    pint = calloc(1,sizeof(int));
    *pint = args[0].ival;
    epicsThreadCreate("generrlogPrintf",
        epicsThreadPriorityLow,epicsThreadGetStackSize(epicsThreadStackSmall),
        (EPICSTHREADFUNC)generrlogPrintf,pint);
}
static void generrlogPrintfRegister(void)
{
    static int firstTime = 1;
    if(!firstTime) return;
    firstTime = 0;
    iocshRegister(&generrlogPrintfDef,generrlogPrintfCall);
}
epicsExportRegistrar(generrlogPrintfRegister);
