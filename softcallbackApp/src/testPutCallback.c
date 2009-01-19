/*testPutCallback.c*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "cadef.h"
#include "dbDefs.h"
#include "epicsTime.h"
#include "epicsEvent.h"

#define NUM_PV 3
#define NUM_VAL 2
#define MAX_PV_NAME_LEN 40
#define EXPECTED_DELAY 1.0

typedef enum {connect, put} state;
typedef struct pvInfo{
    struct info *pinfo;
    double value;  /* Callback value */
    char   pvname[MAX_PV_NAME_LEN];
    chid   mychid; 
    evid   myevid;
    epicsTimeStamp time; /*Callback time*/
}pvInfo;

typedef struct info{
    state  phase;
    int    wasFailure;
    int    nCallbacks;
    pvInfo pv[NUM_PV];
    double value[NUM_VAL];
    epicsEventId event;
}info;
    
    
static void eventCallback(struct event_handler_args eha)
{
    pvInfo *ppvInfo = (pvInfo *)eha.usr;
    info   *pinfo = ppvInfo->pinfo;
    struct dbr_time_double *pdata = (struct dbr_time_double *)eha.dbr;

    if(eha.status!=ECA_NORMAL) {
	printf("%s error %s\n",ppvInfo->pvname,ca_message(eha.status));
        return;
    }
    if(pinfo->phase==put) {
        ppvInfo->value = pdata->value;
        ppvInfo->time = pdata->stamp;
    }
    pinfo->nCallbacks++;
    if(pinfo->nCallbacks==NUM_PV) epicsEventSignal(pinfo->event);
}

int main(int argc,char **argv)
{
    info data;
    int  i,j;
    epicsEventWaitStatus waitStatus;

    if(argc != NUM_PV + NUM_VAL + 1) {
        printf("usage: testPutCallback pv1 pv2 pv3 val1 val2\n");
        exit(1);
    }
    argv++; /*Skip command*/
    for(i=0; i<NUM_PV; i++) {
        strncpy(data.pv[i].pvname,*argv++,MAX_PV_NAME_LEN);
        data.pv[i].pinfo = &data;
    }
    for(i=0; i<NUM_VAL; i++) {
        sscanf(*argv++,"%lf",&data.value[i]);
    }
    data.event = epicsEventMustCreate(epicsEventEmpty);
    data.phase = connect;
    data.wasFailure = 0;
    data.nCallbacks = 0;
    SEVCHK(ca_context_create(ca_enable_preemptive_callback),"ca_context_create");
    for(i=0; i<NUM_PV; i++) {
        pvInfo *ppvInfo = &data.pv[i];
        chid *pmychid = &ppvInfo->mychid;
        evid *pmyevid = &ppvInfo->myevid;
        
	SEVCHK(ca_create_channel(ppvInfo->pvname,0,ppvInfo,20,pmychid),
	    "ca_create_channel");
	SEVCHK(ca_add_event(DBR_TIME_DOUBLE,
            *pmychid,eventCallback,ppvInfo,pmyevid),
	    "ca_add_event");
    }
    ca_flush_io();
    waitStatus = epicsEventWaitWithTimeout(data.event,10.0);
    if(waitStatus!=epicsEventWaitOK) {
        printf("After connect/create: epicsEventWaitWithTimeout failure %s\n",
                (waitStatus==epicsEventWaitTimeout ? "timeout" : "failure"));
        return(-1);
    }
    data.phase = put;
    for(i=0; i<NUM_VAL; i++) {
        pvInfo *pfirst = &data.pv[0];
        chid *pmychid = &pfirst->mychid;

        data.nCallbacks = 0;
        SEVCHK(ca_put(DBR_DOUBLE,*pmychid,&data.value[i]),"ca_put");
        ca_flush_io();
        waitStatus = epicsEventWaitWithTimeout(data.event,10.0);
        if(waitStatus!=epicsEventWaitOK) {
            printf("After ca_put: epicsEventWaitWithTimeout failure %s\n",
                (waitStatus==epicsEventWaitTimeout ? "timeout" : "failure"));
            return(-1);
        }
        /* All data values should be equal to data.value[i] */
        for(j=0; j<NUM_PV; j++) {
            pvInfo *ppvInfo = &data.pv[j];
            if(ppvInfo->value!=data.value[i]) {
                printf("value %f but %s is %f\n",
                    data.value[i],ppvInfo->pvname,ppvInfo->value);
                data.wasFailure = 1;
            }
        }
        /*time for 1,..,NUM_PV should be EXPECTED_DELAY secs > for PV 1 */
        for(j=1; j<NUM_PV; j++) {
            pvInfo *ppvInfo = &data.pv[j];
            double diff;
            diff = epicsTimeDiffInSeconds(&ppvInfo->time,&pfirst->time);
            if(fabs(diff - EXPECTED_DELAY)>.1) {
                printf("%s time diff is %f\n",ppvInfo->pvname,diff);
                data.wasFailure = 1;
            }
        }
    }
    if(data.wasFailure==0) printf("testDevAsyn OK\n");
    return(0);
}
