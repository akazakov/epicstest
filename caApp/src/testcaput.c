/*testcaput.c*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "epicsTime.h"
#include "cadef.h"

epicsTimeStamp	time_now;
char		myvalue[20];
static int	newvalue=0;

static int      readAccess=0;
static int      writeAccess=0;
static int      accessChanges=1; /* Print at startup */
static int      stateChanges=1;

typedef struct{
    chid	mychid;
    short	read_access;
    short	write_access;
} MYNODE;

MYNODE *pmynode = NULL;

void accessRightsCallback(struct access_rights_handler_args arha)
{
    chid	chid = arha.chid;
    MYNODE	*pnode = (MYNODE *)ca_puser(chid);
    int         rd_access = ca_read_access(chid);
    int         wr_access = ca_write_access(chid);

    if(rd_access != pnode->read_access) {
	if(rd_access) readAccess++; else readAccess--;
	pnode->read_access = rd_access;
    }
    if(wr_access != pnode->write_access) {
	if(wr_access) writeAccess++; else writeAccess--;
	pnode->write_access = wr_access;
    }
    accessChanges++;
}

void connection_callback(struct connection_handler_args cha)
{
    stateChanges++;
}

int main(int argc,char **argv)
{
    int		npv;
    int		status;
    int		i;
    int		notConnected;
    char	timestamp[40];
    double	pendEventTime=0;

    if(argc!=3) {
	printf("usage: testcaput <nrecords> <pendEventTime>\n");
	exit(1);
    }
    sscanf(argv[1],"%d",&npv);
    sscanf(argv[2],"%lf",&pendEventTime);
    pmynode = (MYNODE *)calloc(npv,sizeof(MYNODE));
    for(i=0; i<npv; i++) {
	memset((char *)&pmynode[i],0,sizeof(MYNODE));
    }
    SEVCHK(ca_task_initialize(),"ca_task_initialize");
    for(i=0; i<npv; i++) {
	char    name[40];

	sprintf(name,"mrk%04d",i);
	SEVCHK(ca_search_and_connect(name,&pmynode[i].mychid,
		connection_callback,&pmynode[i]),
		"ca_search_and_connect");

	SEVCHK(ca_replace_access_rights_event(pmynode[i].mychid,
		accessRightsCallback),
		"ca_replace_access_rights_event");
    }
    while(1) {
	newvalue++;
	sprintf(myvalue,"%d",newvalue);
	if(newvalue>=10) newvalue=0;

	status = ca_pend_event(pendEventTime);
	if(status!=ECA_NORMAL && status!=ECA_TIMEOUT)
		SEVCHK(status,"ca_pend_event");

	if(epicsTimeGetCurrent(&time_now)!=0)
		printf("epicsTimeGetCurrent error in main\n");
        epicsTimeToStrftime(timestamp,sizeof(timestamp),
            "%Y/%m/%d %H:%M:%S.%06f",&time_now);

	notConnected = 0;
	for (i=0; i<npv; i++) {
	    if (ca_state(pmynode[i].mychid) != cs_conn) {
		notConnected++;
		continue;       /* Don't ca_put if not connected */
	    }
	    status = ca_put(DBR_STRING, pmynode[i].mychid, myvalue);
	    if (status!=ECA_NORMAL)
		printf("ca_put (mrk%04d) error: %s\n", i, ca_message(status));
	}
        SEVCHK(ca_flush_io(),"ca_flush_io");

	if (accessChanges) {
	    printf("%5.5d events: readAccess %5.5d writeAccess %5.5d time %s\n",
                   accessChanges, readAccess, writeAccess, timestamp);
	    accessChanges = 0;
	}
	if (stateChanges) {
	    printf("%5.5d events: connected %5.5d                    time %s\n",
                   stateChanges, npv-notConnected, timestamp);
            stateChanges = 0;
	}
    }
    return(0);
}
