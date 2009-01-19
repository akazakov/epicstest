/* typeRecord.c */
/*
 *      Original Author:  Marty Kraimer
 *      Date:            18APR96
 *
 *      Experimental Physics and Industrial Control System (EPICS)
 *
 *      Copyright 1991, the Regents of the University of California,
 *      and the University of Chicago Board of Governors.
 *
 *      This software was produced under  U.S. Government contracts:
 *      (W-7405-ENG-36) at the Los Alamos National Laboratory,
 *      and (W-31-109-ENG-38) at Argonne National Laboratory.
 *
 *      Initial development by:
 *              The Controls and Automation Group (AT-8)
 *              Ground Test Accelerator
 *              Accelerator Technology Division
 *              Los Alamos National Laboratory
 *
 *      Co-developed with
 *              The Controls and Computing Group
 *              Accelerator Systems Division
 *              Advanced Photon Source
 *              Argonne National Laboratory
 *
 * Modification Log:
 * -----------------
 * .01  16APR96		mrk	adapted from waveform record
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "dbDefs.h"
#include "errlog.h"
#include "alarm.h"
#include "cvtTable.h"
#include "dbAccess.h"
#include "dbScan.h"
#include "dbEvent.h"
#include "dbFldTypes.h"
#include "devSup.h"
#include "recSup.h"
#include "recGbl.h"
#include "special.h"
#include "menuConvert.h"
#define GEN_SIZE_OFFSET
#include "typeRecord.h"
#undef  GEN_SIZE_OFFSET
#include "epicsExport.h"

/* Create RSET - Record Support Entry Table*/
#define report NULL
#define initialize NULL
static long init_record();
static long process();
#define special NULL
#define get_value NULL
static long cvt_dbaddr();
static long get_array_info();
static long put_array_info();
static long get_units();
static long get_precision();
#define get_enum_str NULL
#define get_enum_strs NULL
#define put_enum_str NULL
static long get_graphic_double();
static long get_control_double();
#define get_alarm_double NULL

rset typeRSET={
	RSETNUMBER,
	report,
	initialize,
	init_record,
	process,
	special,
	get_value,
	cvt_dbaddr,
	get_array_info,
	put_array_info,
	get_units,
	get_precision,
	get_enum_str,
	get_enum_strs,
	put_enum_str,
	get_graphic_double,
	get_control_double,
	get_alarm_double
};
epicsExportAddress(rset,typeRSET);

/*sizes of field types*/
static int sizeofTypes[] = {0,1,1,2,2,4,4,4,8,2};

static void monitor();


static long init_record(precord,pass)
    struct typeRecord	*precord;
    int pass;
{

    if (pass==0){
	if(precord->nelm<=0) precord->nelm=1;
	if(precord->ftvl == 0) {
		precord->val = (char *)calloc(precord->nelm,MAX_STRING_SIZE);
	} else {
		if(precord->ftvl>DBF_ENUM) precord->ftvl=2;
		precord->val = (char *)calloc(precord->nelm,
			sizeofTypes[precord->ftvl]);
	}
	if(precord->nelm==1) {
	    precord->nord = 1;
	} else {
	    precord->nord = 0;
	}
	return(0);
    }
    return(0);
}

static long process(precord)
	struct typeRecord	*precord;
{
	long		status = 0;
	long		nRequest = precord->nelm;

        precord->pact = TRUE;
	if(precord->inp.type!=CONSTANT) {
		if(nRequest==1)
		    status=dbGetLink(&precord->inp,precord->ftvl,
			precord->val,0,0);
		else
		    status=dbGetLink(&precord->inp,precord->ftvl,
			precord->val,0,&nRequest);
		if(!status) {
			if(nRequest>0) precord->nord = nRequest;
		}
	}
	if(precord->nord>0) precord->udf=FALSE;
	recGblGetTimeStamp(precord);
	if(precord->nord) {
	    dbPutLink(&precord->out,precord->ftvl,precord->val,precord->nord);
	}
	monitor(precord);
        /* process the forward scan link record */
        recGblFwdLink(precord);
        precord->pact=FALSE;
        return(0);
}

static long cvt_dbaddr(paddr)
    struct dbAddr *paddr;
{
    struct typeRecord *precord=(struct typeRecord *)paddr->precord;

    paddr->pfield = (void *)(precord->val);
    paddr->no_elements = precord->nelm;
    paddr->field_type = precord->ftvl;
    if(precord->ftvl==0)  paddr->field_size = MAX_STRING_SIZE;
    else paddr->field_size = sizeofTypes[precord->ftvl];
    paddr->dbr_field_type = precord->ftvl;
    return(0);
}

static long get_array_info(paddr,no_elements,offset)
    struct dbAddr *paddr;
    long	  *no_elements;
    long	  *offset;
{
    struct typeRecord	*precord=(struct typeRecord *)paddr->precord;

    *no_elements =  precord->nord;
    *offset = 0;
    return(0);
}

static long put_array_info(paddr,nNew)
    struct dbAddr *paddr;
    long	  nNew;
{
    struct typeRecord	*precord=(struct typeRecord *)paddr->precord;

    precord->nord = nNew;
    if(precord->nord > precord->nelm) precord->nord = precord->nelm;
    return(0);
}

static long get_units(paddr,units)
    struct dbAddr *paddr;
    char	  *units;
{
    struct typeRecord	*precord=(struct typeRecord *)paddr->precord;

    strncpy(units,precord->egu,DB_UNITS_SIZE);
    return(0);
}

static long get_precision(paddr,precision)
    struct dbAddr *paddr;
    long	  *precision;
{
    struct typeRecord	*precord=(struct typeRecord *)paddr->precord;

    *precision = precord->prec;
    return(0);
}

static long get_graphic_double(paddr,pgd)
    struct dbAddr *paddr;
    struct dbr_grDouble *pgd;
{
    struct typeRecord     *precord=(struct typeRecord *)paddr->precord;

    if(paddr->pfield==(void *)precord->val){
        pgd->upper_disp_limit = precord->hopr;
        pgd->lower_disp_limit = precord->lopr;
    } else recGblGetGraphicDouble(paddr,pgd);
    return(0);
}
static long get_control_double(paddr,pcd)
    struct dbAddr *paddr;
    struct dbr_ctrlDouble *pcd;
{
    struct typeRecord     *precord=(struct typeRecord *)paddr->precord;

    if(paddr->pfield==(void *)precord->val){
        pcd->upper_ctrl_limit = precord->hopr;
        pcd->lower_ctrl_limit = precord->lopr;
    } else recGblGetControlDouble(paddr,pcd);
    return(0);
}

static void monitor(precord)
    struct typeRecord	*precord;
{
	unsigned short	monitor_mask;

        monitor_mask = recGblResetAlarms(precord);
	monitor_mask |= (DBE_LOG|DBE_VALUE);
        db_post_events(precord,precord->val,monitor_mask);
	return;

}
