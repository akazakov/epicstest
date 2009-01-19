/*cagetalarm.c*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cadef.h"

int main(int argc,char **argv)
{
    chid	mychid;
    double	waitTime = 5.0;
    struct dbr_stsack_string data;

    if(argc<2) {
	printf("usage: cagetalarm pvname \n");
	exit(1);
    }
    SEVCHK(ca_task_initialize(),"ca_task_initialize");
    SEVCHK(ca_search(argv[1],&mychid),"ca_search");
    SEVCHK(ca_pend_io(waitTime),"ca_pend_io");
    SEVCHK(ca_get(DBR_STSACK_STRING,mychid,&data),"ca_get");
    SEVCHK(ca_pend_io(waitTime),"ca_pend_io");
    printf("pv: %s status=%d severity=%d ackt=%d acks=%d value=%s\n",
	ca_name(mychid),data.status,data.severity,
	data.ackt,data.acks,data.value);
/*
    ca_task_exit();
*/
    return(0);
}
