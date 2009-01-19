/* fdUsage.c - Device Support Routines for vxWorks statistics */

#include <vxWorks.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <private/iosLibP.h>
int fdUsage()
{
    int i,tot;

    printf("maxFiles %d\nIn Use: ",maxFiles);
    for(tot=0,i=0;i<maxFiles;i++)  {
	if(fdTable[i].inuse) {
	    printf("%d ",i);
	    ++tot;
	}
    }
    printf("\nTotal in use %d\n",tot);
    return(0);
}
