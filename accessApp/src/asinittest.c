#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "epicsThread.h"
#include "dbAccess.h"
#include "errlog.h"
#include "asLib.h"
#include "asDbLib.h"

void asinittest(char *asfile1,char *asfile2)
{
    static int itteration=0;
    char *asfile;

    printf("asinittest %s %s\n",asfile1,asfile2);
    while(1) {
        printf("asinittest itteration %d\n",itteration);
        asfile = ((itteration%2) ? asfile1 : asfile2);
        asSetFilename(asfile);
        asInit();
        ++itteration;
        epicsThreadSleep(5.0);
    }
}
