#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "epicsThread.h"
#include "dbDefs.h"
#include "epicsPrint.h"

int paritytest(int size,double delay)
{
    char *pmem;
    char *p;
    char c,cread;
    int i;

    if(delay==0) delay = 1.0;
    pmem = (char *)malloc(size);
    if(!pmem) {
	printf("malloc failed\n");
	return(0);
    }

    c=0;
    while(TRUE) {
	p=pmem;
	for(i=0; i<size; i++) *p++ = c;
	p=pmem;
	for(i=0; i<size; i++) {
	    cread = *p;
	    if(cread!=c) {
		printf("paritytest addr %p write %2.2x read %2.2x\n",
		    p,c,cread);
	    }
	    ++p;
	}
	epicsThreadSleep(delay);
	++c;
    }
    return(0);
}
