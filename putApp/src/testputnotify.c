/*testputnotify.c*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cadef.h>
#include <dbDefs.h>

static chid	mychid;
static int	ntrys;
static int	gotCallback;

void putCallback(struct event_handler_args args)
{
    gotCallback = TRUE;
    printf("putCallback ");
    if(args.chid) printf(" pv=%s",ca_name(args.chid));
    SEVCHK(args.status,"putCallback");
}

int main(int argc,char **argv)
{
    int		status;
    double	value=1.0;
    char	*pvname;
    double	pendEventTime;
    int		maxTrys = 3;

    if(argc!=3) {
	printf("usage: testputnotify <pvname> <pendEventTime>\n");
	exit(1);
    }
    pvname = argv[1];
    sscanf(argv[2],"%le",&pendEventTime);
    SEVCHK(ca_task_initialize(),"ca_task_initialize");
    SEVCHK(ca_search(pvname,&mychid),"ca_search_and_connect");
    SEVCHK(ca_pend_io(1.0),"ca_pend_io");
    while(TRUE) {
	SEVCHK(ca_array_put_callback(DBR_DOUBLE,1,mychid,
		(void *)&value,putCallback,NULL),
		"ca_put_callback");
	ntrys=0;
	gotCallback=FALSE;
	while(!gotCallback && ntrys <=maxTrys) {
	    status = ca_pend_event(pendEventTime);
	    if(status!=ECA_NORMAL && status!=ECA_TIMEOUT)
		printf("ca_pend_event status=%s\n",ca_message(status));
	    ntrys++;
	    if(ntrys==maxTrys) {
		printf(" Never got callback\n");
		break;
	    }
	}
	printf(" number ca_pend_event = %d\n",ntrys);
    }
    return(0);
}
