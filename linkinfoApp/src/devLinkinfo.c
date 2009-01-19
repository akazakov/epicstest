/* devLinkinfo.c */
/* base/src/dev $Id: devLinkinfo.c 165 2003-04-02 15:45:50Z mrk $ */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <alarm.h>
#include <cvtTable.h>
#include <dbDefs.h>
#include <dbStaticLib.h>
#include <dbAccess.h>
#include <recGbl.h>
#include <recSup.h>
#include <devSup.h>
#include <link.h>
#include <dbCa.h>
#include <aiRecord.h>
#include <epicsExport.h>

/* Create the dset for devLinkinfo */
static long init_record();
static long read_ai();
static struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	read_ai;
	DEVSUPFUN	special_linconv;
}devLinkinfo={
	6,
	NULL,
	NULL,
	init_record,
	NULL,
	read_ai,
	NULL
};
epicsExportAddress(dset,devLinkinfo);

typedef struct myDpvt {
    int gotAttrib;
}myDpvt;

static void myAttribCallback(void *pvt)
{
    aiRecord *precord = (aiRecord *)pvt;
    myDpvt *pmyDpvt = (myDpvt *)precord->dpvt;
    pmyDpvt->gotAttrib = 1;
    printf("%s myAttribCallback\n",precord->name);
    
}

static long init_record(aiRecord *pai)
{
    struct link *plink = &pai->inp;
    myDpvt *pmyDpvt = (myDpvt *)pai->dpvt;
    long status;

    pai->dpvt = pmyDpvt = dbCalloc(1,sizeof(myDpvt));
    /* ai.inp must be a CONSTANT or a PV_LINK or a DB_LINK or a CA_LINK*/
    switch (pai->inp.type) {
    case (CONSTANT) :
        if(recGblInitConstantLink(&pai->inp,DBF_DOUBLE,&pai->val))
            pai->udf = FALSE;
	break;
    case (PV_LINK) :
    case (DB_LINK) :
        pmyDpvt->gotAttrib = 1;
        break;
    case (CA_LINK) :
    status = dbCaGetAttributes(plink,myAttribCallback,(void *)pai);
    if(status) errlogPrintf("%s dbCaGetAttributes failed\n",pai->name);
	break;
    default :
	recGblRecordError(S_db_badField, (void *)pai,
		"devLinkinfo (init_record) Illegal INP field");

	return(S_db_badField);
    }
    /* Make sure record processing routine does not perform any conversion*/
    pai->linr = 0;
    return(0);
}

static long read_ai(aiRecord *pai)
{
    long status;
    myDpvt *pmyDpvt = (myDpvt *)pai->dpvt;
    struct link *plink = &pai->inp;
    long nelements;
    short severity;
    epicsTimeStamp now;
    char nowText[40];
    double controlLow,controlHigh,displayLow,displayHigh;
    double alarmLolo,alarmLow,alarmHigh,alarmHihi;
    short precision;
    char units[20];

    status = dbGetLink(plink,DBR_DOUBLE, &(pai->val),0,0);
    if (plink->type!=CONSTANT && RTN_SUCCESS(status)) pai->udf = FALSE;
    if(plink->type!=CA_LINK && plink->type!=DB_LINK) return(2);
    printf("%s ",pai->name);
    if(!dbIsLinkConnected(plink)) {
        printf("not connected\n");
        return(2);
    }
    status = dbGetNelements(plink,&nelements);
    if(status) printf("%s dbCaGetNelements failed\n",pai->name);
    status = dbGetSevr(plink,&severity);
    if(status) printf("%s dbCaGetSevr failed\n",pai->name);
    status = dbGetTimeStamp(plink,&now);
    if(status) printf("%s dbGetTimeStamp failed\n",pai->name);
    epicsTimeToStrftime(nowText,sizeof(nowText),"%Y/%m/%d %H:%M:%S",&now);
    printf(" connected nelements %ld sevr %hd\ntime %s DBFtype %d\n",
        nelements,severity,nowText,dbGetLinkDBFtype(plink));
    if(!pmyDpvt->gotAttrib) return(2);
    status = dbGetControlLimits(plink,&controlLow,&controlHigh);
    if(status) printf("%s dbGetControlLimits failed\n",pai->name);
    printf("controlLimits %f %f",controlLow,controlHigh);
    status = dbGetGraphicLimits(plink,&displayLow,&displayHigh);
    if(status) printf("%s dbGetGraphicLimits failed\n",pai->name);
    printf(" displayLimits %f %f\n",displayLow,displayHigh);
    status = dbGetAlarmLimits(plink,
        &alarmLolo,&alarmLow,&alarmHigh,&alarmHihi);
    if(status) printf("%s dbGetAlarmLimits failed\n",pai->name);
    printf("alarmLimits %f %f %f %f\n",alarmLolo,alarmLow,alarmHigh,alarmHihi);
    status = dbGetPrecision(plink,&precision);
    if(status) printf("%s dbGetPrecision failed\n",pai->name);
    status = dbGetUnits(plink,units,sizeof(units));
    if(status) printf("%s dbGetUnits failed\n",pai->name);
    printf("precision %hd units %s\n",precision,units);
    return(2); /*don't convert*/
}
