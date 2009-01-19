/*testcaget.c*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "epicsTime.h"
#include "cadef.h"

static int	stateChanges=0;
static int	readAccess=0;
static int	writeAccess=0;
static int	accessChanged=0;
static int	eventChanges=0;
static int	oldEventChanges=0;

typedef struct{
    char		value[20];
    chid		mychid;
    evid		myevid;
    short		read_access;
    short		write_access;
} MYNODE;

MYNODE *pmynode = NULL;

void accessRightsCallback(struct access_rights_handler_args arha)
{
    chid	chid = arha.chid;
    MYNODE	*pnode = (MYNODE *)ca_puser(chid);

    if(ca_read_access(chid) != pnode->read_access) {
	if(ca_read_access(chid)) readAccess++; else readAccess--;
	pnode->read_access = ca_read_access(chid);
    }
    if(ca_write_access(chid) != pnode->write_access) {
	if(ca_write_access(chid)) writeAccess++; else writeAccess--;
	pnode->write_access = ca_write_access(chid);
    }
    accessChanged = 1;
}

void connection_callback(struct connection_handler_args cha)
{
    stateChanges++;
}

void event_callback(struct event_handler_args eha)
{
    MYNODE	*pnode = (MYNODE *)eha.usr;

    eventChanges++;
    if(eha.status != ECA_NORMAL) {
	if(eha.status!=ECA_NORDACCESS) {
	    printf("event_callback");
	    if(pnode) printf(" pv %s",ca_name(pnode->mychid));
	    printf(" status %s\n",ca_message(eha.status));
	}
	return;
    }
    if(!ca_read_access(pnode->mychid)) return;
    strncpy(pnode->value,(char *)eha.dbr,sizeof(pnode->value)-1);
    pnode->value[sizeof(pnode->value)-1]='\0';
}

int main(int argc,char **argv)
{
    int		npv; 
    int		status;
    int		i;
    epicsTimeStamp	time_now;
    char	timestamp[40];
    int		notConnected;
    int		prevNotConnected;
    double	pendEventTime=2.0;


    if(argc!=3) {
	printf("usage: testcaget <nrecords> <pendEventTime>\n");
	exit(1);
    }
    sscanf(argv[1],"%d",&npv);
    sscanf(argv[2],"%le",&pendEventTime);
    pmynode = (MYNODE *)calloc(npv,sizeof(MYNODE));
    notConnected = npv+1;/*Make message get printed first time*/
    for(i=0; i<npv; i++) {
	memset((char *)&pmynode[i],0,sizeof(MYNODE));
    }
    SEVCHK(ca_task_initialize(),"ca_task_initialize");
    for(i=0; i<npv; i++) {
	char	name[40];

	sprintf(name,"mrk%04d",i);
	SEVCHK(ca_search_and_connect(name,&pmynode[i].mychid,
		connection_callback,&pmynode[i]),
		"ca_search_and_connect");
	SEVCHK(ca_replace_access_rights_event(pmynode[i].mychid,
		accessRightsCallback),
		"ca_replace_access_rights_event");
	SEVCHK(ca_add_event(DBR_STRING,pmynode[i].mychid,event_callback,
		&pmynode[i],&pmynode[i].myevid),
		"ca_add_event");
    }
    while(1) {
	status = ca_pend_event(pendEventTime);
	if(status!=ECA_NORMAL && status!=ECA_TIMEOUT)
		SEVCHK(status,"ca_pend_event");
	if(epicsTimeGetCurrent(&time_now)!=0)
		printf("epicsTimeGetCurrent error in main\n");
        epicsTimeToStrftime(timestamp,sizeof(timestamp),
            "%Y/%m/%d %H:%M:%S.%06f",&time_now);
	if(stateChanges) {
	    printf("stateChanges %5.5d time %s\n",stateChanges,timestamp);
	    stateChanges = 0;
	}
	if(accessChanged) {
	    printf("readAccess %5.5d writeAccess %5.5d time %s\n",
		readAccess,writeAccess,timestamp);
	    accessChanged = 0;
	}
	if(eventChanges) {
	    if(eventChanges!=oldEventChanges) {
/*
		printf("eventChanges %5.5d time %s\n",eventChanges,timestamp);
*/
	    }
	    oldEventChanges = eventChanges;
	    eventChanges = 0;
	}
	prevNotConnected=notConnected;
	notConnected = 0;
	for(i=0; i<npv; i++) {
	    if(ca_state(pmynode[i].mychid) !=cs_conn) notConnected++;
	}
	if(notConnected!=prevNotConnected) {
	    printf("notConnected %5.5d time %s\n",notConnected,timestamp);
	}
    }
    return(0);
}
