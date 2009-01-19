/* dbcaattrib.c */

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
 
#include "dbDefs.h"
#include "dbBase.h"
#include "link.h"
#include "dbAccess.h"
#include "dbStaticLib.h"
#include "epicsPrint.h"
#include "errMdef.h"

#define UNITS_SIZE 16
void myCallback(void *userPvt)
{
    DBLINK	*plink = (DBLINK *)userPvt;
    double	low,high,lolo,hihi;
    long	nelements;
    short	precision;
    short	severity;
    char	units[UNITS_SIZE];
    long	status;

    epicsPrintf("getAttrib callback\n");
    status = dbCaGetControlLimits(plink,&low,&high);
    if(!status) {
	epicsPrintf("Control Limits: %f %f\n",low,high);
    } else {
	epicsPrintf("dbCaGetControlLimits failed\n");
    }
    status = dbCaGetGraphicLimits(plink,&low,&high);
    if(!status) {
	epicsPrintf("Graphics Limits: %f %f\n",low,high);
    } else {
	epicsPrintf("dbCaGetGraphicLimits failed\n");
    }
    status = dbCaGetAlarmLimits(plink,&lolo,&low,&high,&hihi);
    if(!status) {
	epicsPrintf("Alarm Limits: %f %f %f %f\n",lolo,low,high,hihi);
    } else {
	epicsPrintf("dbCaGetAlarmLimits failed\n");
    }
    status = dbCaGetNelements(plink,&nelements);
    if(!status) {
	epicsPrintf("nelements %ld\n",nelements);
    } else {
	epicsPrintf("dbCaGetNelements failed\n");
    }
    status = dbCaGetPrecision(plink,&precision);
    if(!status) {
	epicsPrintf("precision %hd\n",precision);
    } else {
	epicsPrintf("dbCaGetPrecision failed\n");
    }
    status = dbCaGetSevr(plink,&severity);
    if(!status) {
	epicsPrintf("severity %hd\n",severity);
    } else {
	epicsPrintf("dbCaGetSevr failed\n");
    }
    status = dbCaGetUnits(plink,units,UNITS_SIZE);
    if(!status) {
	epicsPrintf("units %s\n",units);
    } else {
	epicsPrintf("dbCaGetUnits failed\n");
    }
    return;
}

int dbcaattrib(char *pvname)
{
    DBADDR	dbaddr;
    long	status;
    DBLINK	*plink;
    
    
    
    status = dbNameToAddr(pvname,&dbaddr);
    if(status) {
	errMessage(status,"pvname");
	return(-1);
    }
    if(dbaddr.field_type < DBF_INLINK || (dbaddr.field_type >DBF_FWDLINK)) {
	epicsPrintf("Field is not a link\n");
	return(-1);
    }
    plink = (DBLINK *)dbaddr.pfield;
    if(plink->type != CA_LINK) {
	epicsPrintf("Field is not a channel access link\n");
	return(-1);
    }
    status = dbCaGetAttributes(plink,myCallback,(void *)plink);
    if(status) {
	errMessage(status,"from dbCaGetAttributes\n");
 	return(-1);
    }
    return(0);
}
