/*testsyncgroup.c*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cadef.h>
#include <dbDefs.h>

chid	mychid;
CA_SYNC_GID gid;

int main(int argc,char **argv)
{
    int		status;
    double	value=1.0;
    double	newvalue=0.0;
    char        *pvname;
    double	blockTime;

    if(argc!=3) {
	printf("usage: testsyncgroup <pvname> <blockTime>\n");
	exit(1);
    }
    pvname = argv[1];
    sscanf(argv[2],"%le",&blockTime);
    SEVCHK(ca_task_initialize(),"ca_task_initialize");
    SEVCHK(ca_search(pvname,&mychid),"ca_search_and_connect");
    SEVCHK(ca_pend_io(1.0),"ca_pend_io");
    SEVCHK(ca_sg_create(&gid),"ca_sg_create");
    SEVCHK(ca_sg_array_put(gid,DBR_DOUBLE,1,mychid,(void *)&value),
	"ca_sg_array_put");
    SEVCHK(ca_sg_array_get(gid,DBR_DOUBLE,1,mychid,(void *)&newvalue),
	"ca_sg_array_put");
    status = ca_sg_block(gid,blockTime);
    printf("ca_sg_block status=%s\n",ca_message(status));
    status = ca_sg_test(gid);
    printf("ca_sg_test status=%s\n",ca_message(status));
    printf("newvalue=%f\n",newvalue);
    return(0);
}
