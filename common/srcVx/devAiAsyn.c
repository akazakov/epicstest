/* devAiAsyn.c */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "alarm.h"
#include "callback.h"
#include "cvtTable.h"
#include "dbDefs.h"
#include "dbAccess.h"
#include "recGbl.h"
#include "recSup.h"
#include "devSup.h"
#include "link.h"
#include "dbCommon.h"
#include "aiRecord.h"
#include "epicsExport.h"

/* Create the dset for devAiAsyn */
static long init_record();
static long read_ai();
struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	read_ai;
	DEVSUPFUN	special_linconv;
}devAiAsyn={
	6,
	NULL,
	NULL,
	init_record,
	NULL,
	read_ai,
	NULL
};
epicsExportAddress(dset,devAiAsyn);

static long init_record(pai)
    struct aiRecord	*pai;
{
    CALLBACK *pcallback;
    /* ai.inp must be a CONSTANT*/
    switch (pai->inp.type) {
    case (CONSTANT) :
	pcallback = (CALLBACK *)(calloc(1,sizeof(CALLBACK)));
	pai->dpvt = (void *)pcallback;
	if(recGblInitConstantLink(&pai->inp,DBF_DOUBLE,&pai->val))
	    pai->udf = FALSE;
	break;
    default :
	recGblRecordError(S_db_badField,(void *)pai,
		"devAiAsyn (init_record) Illegal INP field");
	return(S_db_badField);
    }
    return(0);
}

static long read_ai(pai)
    struct aiRecord	*pai;
{
    CALLBACK *pcallback = (CALLBACK *)pai->dpvt;
    /* ai.inp must be a CONSTANT*/
    switch (pai->inp.type) {
    case (CONSTANT) :
	if(pai->pact) {
		return(2); /* don`t convert*/
	} else {
                if(pai->val<=0) return(2);
		pai->pact=TRUE;
                callbackRequestProcessCallbackDelayed(
                    pcallback,pai->prio,pai,pai->val);
    		return(0);
	}
    default :
        if(recGblSetSevr(pai,SOFT_ALARM,INVALID_ALARM)){
		if(pai->stat!=SOFT_ALARM) {
			recGblRecordError(S_db_badField,(void *)pai,
			   "devAiAsyn (read_ai) Illegal INP field");
		}
	}
    }
    return(0);
}
