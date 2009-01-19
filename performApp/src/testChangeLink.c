/* testChangeLink.c */

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
 
#include "epicsThread.h"
#include "dbDefs.h"
#include "dbBase.h"
#include "ellLib.h"
#include "dbAccess.h"
#include "dbStaticLib.h"
#include "dbConvert.h"
#include "dbCommon.h"
#include "dbLock.h"
#include "epicsPrint.h"
#include "dbFldTypes.h"
#include "errMdef.h"


int changeLinkPerform(int ntimes)
{
    DBADDR	from;
    long	status;
    epicsTimeStamp	start,end;
    double	dtime;
    int		itime;
    
    
    status = dbNameToAddr("mrkfanout.LNK2",&from);
    if(status) {
	errMessage(status,"mrkfanout.LNK2");
	return(-1);
    }
    epicsTimeGetCurrent(&start);
    for(itime=0; itime<ntimes; itime++) {
	status = dbPutField(&from,DBR_STRING,"",1);
	if(status) {
	    errMessage(status,"from dbPutField");
	    return(-1);
	}
	status = dbPutField(&from,DBR_STRING,"mrkf1",1);
	if(status) {
	    errMessage(status,"from dbPutField");
	    return(-1);
	}
    }
    epicsTimeGetCurrent(&end);
    dtime = epicsTimeDiffInSeconds(&end,&start);
    dtime = dtime/ntimes;
    printf("time per iteration %e\n",dtime);
    return(0);
}

int changeLinkContinuous(void)
{
    DBADDR	from;
    long	status;
    
    
    status = dbNameToAddr("mrkfanout.LNK2",&from);
    if(status) {
	errMessage(status,"mrkfanout.LNK2");
	return(-1);
    }
    while(1) {
	status = dbPutField(&from,DBR_STRING,"mrkf1 CP",1);
	if(status) {
	    errMessage(status,"from dbPutField");
	    return(-1);
	}
	status = dbPutField(&from,DBR_STRING,"",1);
	epicsThreadSleep(.5);
	if(status) {
	    errMessage(status,"from dbPutField");
	    return(-1);
	}
	epicsThreadSleep(.5);
	status = dbPutField(&from,DBR_STRING,"mrkf1",1);
	if(status) {
	    errMessage(status,"from dbPutField");
	    return(-1);
	}
	epicsThreadSleep(.5);
    }
    return(0);
}
