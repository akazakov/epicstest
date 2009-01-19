#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "epicsPrint.h"
#include "dbAccess.h"
#include "epicsTime.h"
#include "subRecord.h"
	
int timeStampInit(subRecord *precord)
{
	return(0);
}

int timeStampProcess(subRecord *precord)
{
    epicsTimeStamp stamp;
    long status;
    char textBuffer[50];
    
    status = dbCaGetTimeStamp(&precord->inpa,&stamp);
    if(status) {
	printf("timeStampProcess dbCaGetTimeStamp failed\n");
	return(0);
    }
    epicsTimeToStrftime(textBuffer,sizeof(textBuffer),
        "%a %b %d %Y %H:%M:%S",&stamp);
    printf("timeStampProcess %s\n",textBuffer);
    return(0);
}
