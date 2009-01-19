/*caPutCallback.c*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "cadef.h"
#include "dbDefs.h"
#include "epicsThread.h"
#include "epicsEvent.h"
#include "epicsTime.h"

#define MAXnumberCallback 20
static int numberCallback[20];
    
static chid mychid;
static epicsEventId pevent;
static int expectedCallback = 0;
static int verbose = 0;
static char *pvname = 0;
static char *pvalue1 = 0;
static char *pvalue2 = 0;
static char *pvalue = "not assigned";

static void printTime(void)
{
    epicsTimeStamp ts;

    epicsTimeGetCurrent(&ts);
    printf("%s %s %s %d %d ",
        pvname,pvalue,
        epicsThreadGetNameSelf(),ts.secPastEpoch,ts.nsec/1000000);
}

static void putCallback(struct event_handler_args eha)
{
    int *pnumberCallback = (int *)eha.usr;
    if(eha.status!=ECA_NORMAL) {
	printf("%s\n",ca_message(eha.status));
    }
    if(verbose) {
        printTime();
        printf("numberCallback %d expectedCallback %d\n",
            *pnumberCallback,expectedCallback);
    }
    if(*pnumberCallback == expectedCallback) {
        epicsEventSignal(pevent);
    } else {
        printTime();
        printf("putCallback expected %d got %d\n",
            expectedCallback,*pnumberCallback);
    }
}

static void usageExit(void)
{
    fprintf(stderr,"usage: caPutCallback -V -S sleepSecs -C clearEvery ");
    fprintf(stderr,"pvname value1 value2");
    exit(1);
}

int main(int argc,char **argv)
{
    char   *pval;
    double sleepSecs = 0.0;
    int	    clearEvery = 0;
    int     indValue = 1;
    int     indNumberCallback = 1;

    while((argc>1) && (argv[1][0] == '-')) {
        int i;
        char option;
        int  narg;
        option = toupper((argv[1])[1]);
        pval = argv[2];
        argc -= 1; narg = 1;
        if(option=='S') {
            sscanf(pval,"%lf",&sleepSecs);
            argc -= 1; ++narg;
        } else if(option=='C') {
            sscanf(pval,"%d",&clearEvery);
            argc -= 1; ++narg;
        } else if(option=='V') {
            verbose = 1;
        } else {
            usageExit();
        }
        for(i=1; i<argc; i++) argv[i] = argv[i + narg];
    }

    if(argc != 4) usageExit();
    pvname = argv[1];
    pvalue1 = argv[2];
    pvalue2 = argv[3];
    pevent = epicsEventCreate(epicsEventEmpty);
    SEVCHK(ca_context_create(ca_enable_preemptive_callback),
        "ca_task_initialize");
    while(1) {
        SEVCHK(ca_search(pvname,&mychid),"ca_search_and_connect");
        if(ca_pend_io(3.0)!=ECA_NORMAL) {
            epicsThreadSleep(5.0);
            continue;
        }
        while(1) {
            epicsEventWaitStatus status;
            if(indValue==0) {
                indValue = 1; pvalue = pvalue2;
            } else {
                indValue = 0; pvalue=pvalue1;
            }
            if(++indNumberCallback >= MAXnumberCallback) indNumberCallback=0;
            numberCallback[indNumberCallback] = ++expectedCallback;
            status = ca_array_put_callback(DBR_STRING,1,mychid,
                pvalue,putCallback,&numberCallback[indNumberCallback]);
            if(status!=ECA_NORMAL) {
                printf("ca_array_put_callback %s\n",ca_message(status));
                epicsThreadSleep(2.0);
                continue;
            }
            if((clearEvery>0) 
            && ((expectedCallback % clearEvery)==0)) {
                ca_flush_io();
                epicsThreadSleep(sleepSecs);
                SEVCHK(ca_clear_channel(mychid),"ca_clear_channel");
                ca_flush_io();
                expectedCallback = 0;
                epicsThreadSleep(sleepSecs);
                if(verbose) {
                    printTime();
                    printf("Issued ca_clear_channel expectedCallback %d\n",
                        expectedCallback);
                }
                break;
            }
            ca_flush_io();
            if(verbose) {
                printTime();
                printf("Issued ca_put_callback expectedCallback %d\n",
                    expectedCallback);
            }
            while(1) {
                status = epicsEventWaitWithTimeout(pevent,10.0);
                if(status==epicsEventWaitTimeout) {
                    if(verbose) {
                        printTime();
                        printf("timeout after 10 seconds\n");
                    }
                    continue;
                }
                break;
            }
            if(status!=epicsEventWaitOK) {
                int i;
                printTime();
                printf("eventWait status %d\n",status);
                for(i=0; i<MAXnumberCallback; i++) numberCallback[i]=0;
            }
            epicsThreadSleep(sleepSecs);
        }
    }
    ca_task_exit();
    return(0);
}
