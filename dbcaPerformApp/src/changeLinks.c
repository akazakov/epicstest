/*changeLinks.c*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "epicsThread.h"
#include "cadef.h"
#include "dbDefs.h"

typedef struct{
    chid	mychid;
    short	read_access;
    short	write_access;
} MYNODE;

MYNODE *pmynode = NULL;

typedef enum {stateNull,stateMaster,stateClient,statePP}state;
#define numStates statePP+1

static void exceptionCallback(struct exception_handler_args args)
{
    chid        chid = args.chid;
    long        stat = args.stat; /* Channel access status code*/
    const char  *channel;
    static char *noname = "unknown";

    channel = (chid ? ca_name(chid) : noname);


    printf("exceptionCallback stat %s channel %s\n",
        ca_message(stat),channel);
}

static state nextState(int mask,state stateNow)
{
    int firstMask,indMask;

    firstMask = ((stateNow>=numStates) ? -1 : stateNow + 1);
    for(indMask=firstMask; indMask<numStates; indMask++) {
        if(mask&(1<<indMask)) return(indMask);
    }
    for(indMask=0; indMask<numStates; indMask++) {
        if(mask&(1<<indMask)) return(indMask);
    }
    printf("mask is 0. Why?\n");
    exit(-1);
}

int main(int argc,char **argv)
{
    int		npv;
    int		status;
    int		i;
    int		mask;
    int         rotate = 0;
    char	name[100];
    double	sleeptime;
    int         indFanout = 0;
    int         setFanoutNull = 1;
    chid        chidFanout[6];
    state       *stateNow; /*current state for each channel*/
    state       newState;

    if(argc<4) {
	printf("usage: changeLinks <nrecords> <sleep> "
               "<0x1=null,0x2=local CP,0x4=remote CP,0x8=PP> <rotate>\n");
	exit(1);
    }
    sscanf(argv[1],"%d",&npv);
    sscanf(argv[2],"%lf",&sleeptime);
    sscanf(argv[3],"%i",&mask);
    if(argc>=5)  sscanf(argv[4],"%i",&rotate);
    pmynode = (MYNODE *)calloc(npv,sizeof(MYNODE));
    stateNow = (state *)calloc(npv,sizeof(int));
    newState = 0;
    SEVCHK(ca_task_initialize(),"ca_task_initialize");
    SEVCHK(ca_add_exception_event(exceptionCallback,NULL),
        "ca_add_exception_event");
    for(indFanout=0; indFanout<6; indFanout++) {
	sprintf(name,"mrkFanout.LNK%1.1d",indFanout+1);
        SEVCHK(ca_search_and_connect(name,&chidFanout[indFanout], NULL,0),
	"ca_search_and_connect");
    }
    for(i=0; i<npv; i++) {
        stateNow[i] = newState;
        if(rotate) newState = nextState(mask,newState);    
	sprintf(name,"mrkMasterAiFromMaster%04d.INP",i);
	SEVCHK(ca_search_and_connect(name,&pmynode[i].mychid,
		NULL,0), "ca_search_and_connect");
    }
    ca_pend_io(0.0);
    indFanout=0;
    while(TRUE) {
        if(setFanoutNull) {
            name[0] = 0;
        } else {
            sprintf(name,"mrkMasterCalc%04d CP",indFanout);
        }

        if (ca_state(chidFanout[indFanout]) == cs_conn) {
            status = ca_put(DBR_STRING,chidFanout[indFanout],name);
            if(status!=ECA_NORMAL)
		printf("ca_put (mrkFanout.LNK%1.1d) error: %s\n",
                       indFanout+1, ca_message(status));
        }

        if(++indFanout>=6) {
            indFanout=0;
            setFanoutNull = ((setFanoutNull==0) ? 1 : 0);
        }

	for(i=0; i<npv; i++) {
	    if(ca_state(pmynode[i].mychid) != cs_conn) continue;
            switch(stateNow[i]) {
                case stateNull:
                    name[0] = 0; break;
                case stateMaster:
                    sprintf(name,"mrkMasterCalc%04d CP",i); break;
                case stateClient:
                    sprintf(name,"mrkClient%04d CP",i); break;
                case statePP:
                    sprintf(name,"mrkMasterCalc%04d PP",i); break;
            }
            stateNow[i] = nextState(mask,stateNow[i]);

	    status = ca_put(DBR_STRING,pmynode[i].mychid,name);
	    if(status!=ECA_NORMAL)
		printf("ca_put (mrkMasterAiFromMaster%04d.INP) error: %s\n",
                       i, ca_message(status));
	}

	ca_pend_event(sleeptime);
    }
    return(0);
}
