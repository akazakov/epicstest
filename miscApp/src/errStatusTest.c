#include        <stdlib.h>
#include        <stdarg.h>
#include        <stdio.h>
#include        <string.h>
 
#include "epicsPrint.h"
#include "errMdef.h"
#include "error.h"
#include "errlog.h"

int errStatusTest()
{
    long status;
    unsigned int facility,value;
    int rtnval;
    char    name[256];


    for(facility=0; facility<700;facility++) {
        for(value=0;value<300;value++) {
            status = (facility<<16) + value;
            rtnval = errSymFind(status,name);
            if(rtnval==0) printf("%d %d %s\n",facility,value,name);
        }
    }
    return(0);
}
