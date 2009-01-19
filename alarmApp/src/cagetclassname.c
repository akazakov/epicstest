/*cagetclassname.c*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "cadef.h"

int main(int argc,char **argv)
{
    chid	mychid;
    double	waitTime = 5.0;
    char	classname[80];

    if(argc<2) {
	printf("usage: cagetclassname pvname \n");
	exit(1);
    }
    SEVCHK(ca_task_initialize(),"ca_task_initialize");
    SEVCHK(ca_search(argv[1],&mychid),"ca_search");
    SEVCHK(ca_pend_io(waitTime),"ca_pend_io");
    SEVCHK(ca_get(DBR_CLASS_NAME,mychid,&classname),"ca_get");
    SEVCHK(ca_pend_io(waitTime),"ca_pend_io");
    printf("pv: %s classname %s\n",ca_name(mychid),classname);
/*
    ca_task_exit();
*/
    return(0);
}
