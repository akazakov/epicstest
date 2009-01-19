/* program: ezcatest.c
 * purpose: test EZCA puts
 * M. Borland, 1995.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <cadef.h>
#include <ezca.h>
#include <epicsThread.h>

#define MAX_PV_NAME_LEN 40
int main(int argc, char **argv)
{
    double value = 1.0;
    char	pvname[MAX_PV_NAME_LEN];
    char	*pstr = pvname;
    int		retryCount=50;
    int		forever=0;

    if(argc>1) sscanf(argv[1],"%d",&retryCount);
    if(argc>2) sscanf(argv[2],"%d",&forever);
    /* set using group feature */
    fprintf(stderr, "starting group\n");
    fprintf(stderr,"Original Retry count %d\n",ezcaGetRetryCount());
    ezcaSetRetryCount(retryCount);
    while(1) {
	ezcaStartGroup();
	while(pstr = fgets(pvname,MAX_PV_NAME_LEN,stdin)) {
	    pstr[strlen(pvname)-1] = 0;/*strip newline*/
	    if (ezcaPut(pstr, ezcaDouble, 1, &value)!=EZCA_OK)
		fprintf(stderr, "problem ezcaPut'ing %s\n", pstr);
	}
	rewind(stdin);
	if (ezcaEndGroup()!=EZCA_OK) {
	    ezcaPerror("problem ending group");
	}
	if(!forever) break;
	epicsThreadSleep(1.0);
    }
    return(0);
}
