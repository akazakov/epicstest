/*caputacks.c*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cadef.h"

int main(int argc,char **argv)
{
    chid		mychid;
    unsigned short	value;

    if(argc!=3) {
	printf("usage: caputackt pvname value\n");
	exit(1);
    }
    sscanf(argv[2],"%hu",&value);
    SEVCHK(ca_task_initialize(),"ca_task_initialize");
    SEVCHK(ca_search(argv[1],&mychid), "ca_search");
    ca_pend_io(5.0);
    SEVCHK(ca_put(DBR_PUT_ACKS,mychid,&value),"ca_put");
    ca_pend_event(1.0);
/*
    ca_task_exit();
*/
    return(0);
}
