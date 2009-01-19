/*camonitoralarm.c*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cadef.h"

typedef struct{
    chid		mychid;
    evid		myevid;
    struct dbr_stsack_string data;
} MYNODE;

MYNODE *pmynode = NULL;

static void printChidInfo(chid chid, char *message)
{
    printf("\n%s\n",message);
    printf("pv: %s  type(%d) nelements(%ld) host(%s)",
	ca_name(chid),ca_field_type(chid),ca_element_count(chid),
	ca_host_name(chid));
    printf(" read(%d) write(%d) state(%d)\n",
	ca_read_access(chid),ca_write_access(chid),ca_state(chid));
}

static void exceptionCallback(struct exception_handler_args args)
{
    chid	chid = args.chid;
    long	type = args.type;/* the type of the value returned*/
    long	count = args.count;
    long	stat = args.stat; /* Channel access status code*/

    printChidInfo(chid,"exceptionCallback");
    printf("type(%ld) count(%ld) stat(%ld)\n",type,count,stat);
}

static void connectionCallback(struct connection_handler_args args)
{
    chid	chid = args.chid;

    printChidInfo(chid,"connectionCallback");
}

static void accessRightsCallback(struct access_rights_handler_args args)
{
    chid	chid = args.chid;

    printChidInfo(chid,"accessRightsCallback");
}
static void eventCallback(struct event_handler_args eha)
{
    chid	chid = eha.chid;
    struct dbr_stsack_string *dbr = (struct dbr_stsack_string *)eha.dbr;

    printf("pv: %s status=%d severity=%d ackt=%d acks=%d\n",
	ca_name(chid),dbr->status,dbr->severity,dbr->ackt,dbr->acks);
}

int main(int argc,char **argv)
{
    double	waitTime = 0.0;

    if(argc<2) {
	printf("usage: camonitoralarm pvname waittime\n");
	exit(1);
    }
    if(argc==3) {
	sscanf(argv[2],"%lf",&waitTime);
    } 
    pmynode = (MYNODE *)calloc(1,sizeof(MYNODE));
    SEVCHK(ca_task_initialize(),"ca_task_initialize");
    SEVCHK(ca_add_exception_event(exceptionCallback,NULL),
	"ca_add_exception_event");
    SEVCHK(ca_search_and_connect(argv[1],&pmynode->mychid,
	connectionCallback,&pmynode),
	"ca_search_and_connect");
    SEVCHK(ca_replace_access_rights_event(pmynode->mychid,
	accessRightsCallback),
	"ca_replace_access_rights_event");
    SEVCHK(ca_add_masked_array_event(DBR_STSACK_STRING,1,pmynode->mychid,
	eventCallback,pmynode,0.0,0.0,0.0,
	&pmynode->myevid,DBE_ALARM),
	"ca_add_event");
    ca_pend_event(waitTime);
    ca_task_exit();
    return(0);
}
