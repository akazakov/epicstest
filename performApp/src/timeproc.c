/* timeproc.c */

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>

#include "epicsTime.h"
#include "dbDefs.h"
#include "dbBase.h"
#include "ellLib.h"
#include "dbAccess.h"
#include "dbStaticLib.h"
#include "dbCommon.h"

int timeproc(char *pvname,int ntime)
{
	epicsTimeStamp stime,etime;
	int i;
	double totalTime,recordsPerSecond;
	struct dbAddr dbaddr;

	
	if(dbNameToAddr(pvname,&dbaddr)) {
		printf("%s dbNameToAddr failed\n",pvname);
		return(-1);
	}
        epicsTimeGetCurrent(&stime);
	for(i=0;i<ntime;i++){
		(void)dbProcess((struct dbCommon *)dbaddr.precord);
	}
        epicsTimeGetCurrent(&etime);
	totalTime = epicsTimeDiffInSeconds(&etime,&stime);
	recordsPerSecond = ntime/totalTime;
	printf("timeproc: N=%d totalTime=%e recordsPerSecond=%e\n",
		ntime,totalTime,recordsPerSecond);
	return(0);
}
