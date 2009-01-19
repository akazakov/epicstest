#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
 
#include "epicsThread.h"
#include "dbDefs.h"
#include "errlog.h"

int main()
{
    int i;
    char  badmsg[300];

    errMessage(2,"test of errMessage");

    for(i=0; i<300; i++) sprintf(&badmsg[i],"%1.1d",i%10);
    for(i=0; i<=errlogFatal; i++) {
	    errlogSevPrintf(i,"errlogTest");
    }
    errlogMessage(badmsg);
    errlogPrintf("errlogPrintf %d\n",1);
    epicsThreadExitMain();
    return(0);
}
