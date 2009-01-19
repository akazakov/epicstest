/* timedbNameToAddr.c */
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#include "epicsTime.h"
#include "dbDefs.h"
#include "dbBase.h"
#include "ellLib.h"
#include "dbAccess.h"
#include "dbStaticLib.h"
#include "dbCommon.h"
#include "ellLib.h"

typedef struct {
	ELLNODE	node;
	char	*pvname;
}pvNode;
	
#define MAX_PV_NAME_LEN 40

int timedbNameToAddr(char *filename,int ntime)
{
	ELLLIST	list;
	pvNode	*ppvNode;
	char	pvname[MAX_PV_NAME_LEN];
	char	*pstr = pvname;
	epicsTimeStamp stime,etime;
        int i;
	double totalTime;
	int	microSecondsPerSearch;
	struct dbAddr dbaddr;
	FILE	*fp;

	fp = fopen(filename,"r");
	if(!fp) {
		printf("Error opening file\n");
		return(-1);
	}
	ellInit(&list);
	while((pstr = fgets(pvname,MAX_PV_NAME_LEN,fp))) {
		pstr[strlen(pvname)-1] = 0;/*strip newline*/
		ppvNode = dbCalloc(1,sizeof(pvNode));
		ppvNode->pvname = dbCalloc(strlen(pvname)+1,sizeof(char));
		strcpy(ppvNode->pvname,pstr);
		ellAdd(&list,&ppvNode->node);
	}
	printf("Total names to search is %d\n",ellCount(&list));
        epicsTimeGetCurrent(&stime);
	for(i=0;i<ntime;i++){
	    ppvNode = (pvNode *)ellFirst(&list);
	    while(ppvNode) {
		dbNameToAddr(ppvNode->pvname,&dbaddr);
		ppvNode = (pvNode *)ellNext(&ppvNode->node);
	    }
	}
        epicsTimeGetCurrent(&etime);
	totalTime = epicsTimeDiffInSeconds(&etime,&stime);
	microSecondsPerSearch = totalTime*1e6/((double)ntime
            * (double)(ellCount(&list)));
	printf("timedbNameToAddr: N=%d totalTime=%e microSecondsPerSearch=%d\n",
		ntime,totalTime,microSecondsPerSearch);
	while((ppvNode = (pvNode *)ellLast(&list))) {
		free((void *)ppvNode->pvname);
		ellDelete(&list,&ppvNode->node);
		free((void *)ppvNode);
	}
	return(0);
}
