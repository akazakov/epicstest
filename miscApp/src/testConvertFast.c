#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "cvtFast.h"


#define NVALUES 5
int testConvertFast()
{
    char message[80];
    long values[NVALUES] = {-2147483648,-1,0,1,2147483647};
    long value;
    int i;

    for(i=0; i<NVALUES; i++) {
	value = values[i];
        cvtLongToString(value,message);
        printf("cvtLongToString %s\n",message);
        cvtLongToHexString(value,message);
        printf("cvtLongToHexString %s\n",message);
        cvtLongToOctalString(value,message);
        printf("cvtLongToOctalString %s\n",message);
    }
    return(0);
}
