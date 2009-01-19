/* miscTest.c */
/* Author:  Marty Kraimer Date:    23MAR2000 */

/********************COPYRIGHT NOTIFICATION**********************************
This software was developed under a United States Government license
described on the COPYRIGHT_UniversityOfChicago file included as part
of this distribution.
****************************************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "osiThread.h"
#include "dbAccess.h"
#include "errlog.h"
#include "asLib.h"
#include "asDbLib.h"
#include "asCa.h"
#include "dbTest.h"
#include "registryRecordType.h"

int main(int argc,char *argv[])
{
    dbLoadDatabase("../dbd/commonApp.dbd",0,0);
    registerRecordDeviceDriver(pdbbase); 
    dbLoadRecords("Db/testLinearConvert.db",0);
    iocInit();
    threadSleep(1.0);
    /* Marty now what???? */
    threadExitMain();
    return(0);
}
