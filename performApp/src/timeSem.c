/* timeSem.c */
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>

#include "epicsMutex.h"
#include "epicsEvent.h"
#include "epicsTime.h"
#include "dbDefs.h"

volatile int iteration;
#define niterations 1000000
int timeSem()
{
    epicsEventId event;
    epicsMutexId lock;
    epicsTimeStamp stime,etime;
    long i;
    double totalTime,microsecs;

    event = epicsEventMustCreate(epicsEventFull);
    lock = epicsMutexMustCreate();
    epicsTimeGetCurrent(&stime);
    for(i=0; i<niterations; i++) {
	epicsEventMustWait(event);
	epicsEventSignal(event);
	epicsEventMustWait(event);
	epicsEventSignal(event);
	epicsEventMustWait(event);
	epicsEventSignal(event);
	epicsEventMustWait(event);
	epicsEventSignal(event);
	epicsEventMustWait(event);
	epicsEventSignal(event);
    }
    epicsTimeGetCurrent(&etime);
    totalTime = epicsTimeDiffInSeconds(&etime,&stime);
    microsecs = 1e6/(niterations*5/totalTime);
    printf("Binary microsecs=%5.1f\n",microsecs);
    epicsTimeGetCurrent(&stime);
    for(i=0; i<niterations; i++) {
	epicsMutexMustLock(lock);
	epicsMutexUnlock(lock);
	epicsMutexMustLock(lock);
	epicsMutexUnlock(lock);
	epicsMutexMustLock(lock);
	epicsMutexUnlock(lock);
	epicsMutexMustLock(lock);
	epicsMutexUnlock(lock);
	epicsMutexMustLock(lock);
	epicsMutexUnlock(lock);
    }
    epicsTimeGetCurrent(&etime);
    totalTime = epicsTimeDiffInSeconds(&etime,&stime);
    microsecs = 1e6/(niterations*5/totalTime);
    printf("Mutex microsecs=%5.1f\n",microsecs);
    return(0);
}
