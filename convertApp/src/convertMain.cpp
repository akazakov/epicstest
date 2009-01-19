/* convertMain.c */
/* Author:  Marty Kraimer Date:    05MAY2000 */

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

#include "dbAccess.h"
#include "errlog.h"
#include "dbTest.h"
#include "iocsh.h"
#include "epicsThread.h"
#include "epicsExit.h"

int main(int argc,char *argv[])
{
    if(argc!=2) {
        printf("must provide command file\n");
        return(1);
    }
    iocsh(argv[1]);
    epicsThreadSleep(.2);
    iocsh(NULL);
    epicsExit(0);
    return(0);
}
