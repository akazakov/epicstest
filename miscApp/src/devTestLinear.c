/* test linear conversions both old and new */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <alarm.h>
#include <dbDefs.h>
#include <cvtTable.h>
#include <dbAccess.h>
#include <recSup.h>
#include <recGbl.h>
#include <devSup.h>
#include <link.h>
#include <aiRecord.h>
#include <aoRecord.h>
#include <epicsExport.h>

/*This test simulates a 12 bit ADC.
 *UP means Unipolor
 *BP means bipolar
*/
#define UPRAWF 4095
#define UPRAWL 0
#define BPRAWF 2047
#define BPRAWL (-2048)

/* create Analog in DSETs*/
LOCAL long initAiOldUP(struct aiRecord *prec);
LOCAL long readAiOldUP(struct aiRecord *prec);
LOCAL long linconvAiOldUP(struct aiRecord *prec, int after);

LOCAL long initAiUP(struct aiRecord *prec);
LOCAL long readAiUP(struct aiRecord *prec);
LOCAL long linconvAiUP(struct aiRecord *prec, int after);

LOCAL long initAiOldBP(struct aiRecord *prec);
LOCAL long readAiOldBP(struct aiRecord *prec);
LOCAL long linconvAiOldBP(struct aiRecord *prec, int after);

LOCAL long initAiRoffBP(struct aiRecord *prec);
LOCAL long readAiRoffBP(struct aiRecord *prec);
LOCAL long linconvAiRoffBP(struct aiRecord *prec, int after);

LOCAL long initAiEoffBP(struct aiRecord *prec);
LOCAL long readAiEoffBP(struct aiRecord *prec);
LOCAL long linconvAiEoffBP(struct aiRecord *prec, int after);

typedef struct AIDSET{
    long    number;
    DEVSUPFUN	report;
    DEVSUPFUN	init;
    DEVSUPFUN	initAirecord;
    DEVSUPFUN	get_ioint_info;
    DEVSUPFUN	readAiai;
    DEVSUPFUN	special_linconv;
} AIDSET;

static AIDSET devAiTestLinearOldUP = {
    6,0,0,initAiOldUP,0,readAiOldUP,linconvAiOldUP
};
epicsExportAddress(dset,devAiTestLinearOldUP);

static AIDSET devAiTestLinearUP = {
    6,0,0,initAiUP,0,readAiUP,linconvAiUP
};
epicsExportAddress(dset,devAiTestLinearUP);

static AIDSET devAiTestLinearOldBP = {
    6,0,0,initAiOldBP,0,readAiOldBP,linconvAiOldBP
};
epicsExportAddress(dset,devAiTestLinearOldBP);

static AIDSET devAiTestLinearRoffBP = {
    6,0,0,initAiRoffBP,0,readAiRoffBP,linconvAiRoffBP
};
epicsExportAddress(dset,devAiTestLinearRoffBP);

static AIDSET devAiTestLinearEoffBP = {
    6,0,0,initAiEoffBP,0,readAiEoffBP,linconvAiEoffBP
};
epicsExportAddress(dset,devAiTestLinearEoffBP);

/* create Analog out DSETs*/
LOCAL long initAoOldUP(struct aoRecord *prec);
LOCAL long writeAoOldUP(struct aoRecord *prec);
LOCAL long linconvAoOldUP(struct aoRecord *prec, int after);

LOCAL long initAoUP(struct aoRecord *prec);
LOCAL long writeAoUP(struct aoRecord *prec);
LOCAL long linconvAoUP(struct aoRecord *prec, int after);

LOCAL long initAoOldBP(struct aoRecord *prec);
LOCAL long writeAoOldBP(struct aoRecord *prec);
LOCAL long linconvAoOldBP(struct aoRecord *prec, int after);

LOCAL long initAoRoffBP(struct aoRecord *prec);
LOCAL long writeAoRoffBP(struct aoRecord *prec);
LOCAL long linconvAoRoffBP(struct aoRecord *prec, int after);

LOCAL long initAoEoffBP(struct aoRecord *prec);
LOCAL long writeAoEoffBP(struct aoRecord *prec);
LOCAL long linconvAoEoffBP(struct aoRecord *prec, int after);

typedef struct AODSET{
    long    number;
    DEVSUPFUN	report;
    DEVSUPFUN	init;
    DEVSUPFUN	initAirecord;
    DEVSUPFUN	get_ioint_info;
    DEVSUPFUN	write_ai;
    DEVSUPFUN	special_linconv;
} AODSET;

static AODSET devAoTestLinearOldUP = {
    6,0,0,initAoOldUP,0,writeAoOldUP,linconvAoOldUP
};
epicsExportAddress(dset,devAoTestLinearOldUP);

static AODSET devAoTestLinearUP = {
    6,0,0,initAoUP,0,writeAoUP,linconvAoUP
};
epicsExportAddress(dset,devAoTestLinearUP);

static AODSET devAoTestLinearOldBP = {
    6,0,0,initAoOldBP,0,writeAoOldBP,linconvAoOldBP
};
epicsExportAddress(dset,devAoTestLinearOldBP);

static AODSET devAoTestLinearRoffBP = {
    6,0,0,initAoRoffBP,0,writeAoRoffBP,linconvAoRoffBP
};
epicsExportAddress(dset,devAoTestLinearRoffBP);

static AODSET devAoTestLinearEoffBP = {
    6,0,0,initAoEoffBP,0,writeAoEoffBP,linconvAoEoffBP
};
epicsExportAddress(dset,devAoTestLinearEoffBP);

LOCAL long initAiOldUP(struct aiRecord *prec)
{
    if(prec->inp.type == CONSTANT)
	recGblInitConstantLink(&prec->inp,DBF_LONG,&prec->rval);
    linconvAiOldUP(prec,TRUE);
    prec->rval = UPRAWL;
    return(0);
}

LOCAL long readAiOldUP(struct aiRecord *prec)
{
    long status;
    status = dbGetLink(&(prec->inp),DBR_LONG, &(prec->rval),0,0);
    return(0);
}

LOCAL long linconvAiOldUP(struct aiRecord *prec, int after)
{
    double eguf = prec->eguf;
    double egul = prec->egul;

    if(!after) return(0);
    prec->eslo = (eguf - egul)/(double)UPRAWF;
    return(0);
}

LOCAL long initAiUP(struct aiRecord *prec)
{
    if(prec->inp.type == CONSTANT)
	recGblInitConstantLink(&prec->inp,DBF_LONG,&prec->rval);
    linconvAiUP(prec,TRUE);
    prec->rval = UPRAWL;
    return(0);
}

LOCAL long readAiUP(struct aiRecord *prec)
{
    long status;
    status = dbGetLink(&(prec->inp),DBR_LONG, &(prec->rval),0,0);
    return(0);
}

LOCAL long linconvAiUP(struct aiRecord *prec, int after)
{
    double eguf = prec->eguf;
    double egul = prec->egul;

    if(!after) return(0);
    prec->eslo = (eguf - egul)/(double)(UPRAWF - UPRAWL);
    prec->eoff = (UPRAWF*egul - UPRAWL*eguf)/(double)(UPRAWF - UPRAWL);
    return(0);
}

LOCAL long initAiOldBP(struct aiRecord *prec)
{
    if(prec->inp.type == CONSTANT)
	recGblInitConstantLink(&prec->inp,DBF_LONG,&prec->rval);
    linconvAiOldBP(prec,TRUE);
    prec->rval = (BPRAWF - BPRAWL)/2;
    return(0);
}

LOCAL long readAiOldBP(struct aiRecord *prec)
{
    long status;
    status = dbGetLink(&(prec->inp),DBR_LONG, &(prec->rval),0,0);
    if(!status) prec->rval += -BPRAWL;
    return(0);
}

LOCAL long linconvAiOldBP(struct aiRecord *prec, int after)
{
    if(!after) return(0);
    prec->eslo = (prec->eguf -prec->egul)/(double)(BPRAWF - BPRAWL);
    return(0);
}

LOCAL long initAiRoffBP(struct aiRecord *prec)
{
    if(prec->inp.type == CONSTANT)
	recGblInitConstantLink(&prec->inp,DBF_LONG,&prec->rval);
    linconvAiRoffBP(prec,TRUE);
    prec->rval = 0;
    return(0);
}

LOCAL long readAiRoffBP(struct aiRecord *prec)
{
    long status;
    status = dbGetLink(&(prec->inp),DBR_LONG, &(prec->rval),0,0);
    return(0);
}

LOCAL long linconvAiRoffBP(struct aiRecord *prec, int after)
{
    if(!after) return(0);
    prec->eslo = (prec->eguf -prec->egul)/(double)(BPRAWF - BPRAWL);
    prec->roff = -BPRAWL; 
    return(0);
}

LOCAL long initAiEoffBP(struct aiRecord *prec)
{
    if(prec->inp.type == CONSTANT)
	recGblInitConstantLink(&prec->inp,DBF_LONG,&prec->rval);
    linconvAiEoffBP(prec,TRUE);
    prec->rval = 0;
    return(0);
}

LOCAL long readAiEoffBP(struct aiRecord *prec)
{
    long status;
    status = dbGetLink(&(prec->inp),DBR_LONG, &(prec->rval),0,0);
    return(0);
}

LOCAL long linconvAiEoffBP(struct aiRecord *prec, int after)
{
    double eguf = prec->eguf;
    double egul = prec->egul;

    if(!after) return(0);
    prec->eslo = (eguf - egul)/(double)(BPRAWF - BPRAWL);
    prec->eoff = (BPRAWF*egul - BPRAWL*eguf)/(double)(BPRAWF - BPRAWL);
    return(0);
}

LOCAL long initAoOldUP(struct aoRecord *prec)
{
    if(prec->out.type == CONSTANT)
	recGblInitConstantLink(&prec->out,DBF_DOUBLE,&prec->val);
    linconvAoOldUP(prec,TRUE);
    prec->rval = UPRAWL;
    return(0);
}

LOCAL long writeAoOldUP(struct aoRecord *prec)
{
    long status;

    status = dbPutLink(&(prec->out),DBR_LONG, &(prec->rval),1);
    return(0);
}

LOCAL long linconvAoOldUP(struct aoRecord *prec, int after)
{
    if(!after) return(0);
    prec->eslo = (prec->eguf -prec->egul)/(double)UPRAWF;
    return(0);
}

LOCAL long initAoUP(struct aoRecord *prec)
{
    if(prec->out.type == CONSTANT)
	recGblInitConstantLink(&prec->out,DBF_DOUBLE,&prec->val);
    linconvAoUP(prec,TRUE);
    prec->rval = UPRAWL;
    return(0);
}

LOCAL long writeAoUP(struct aoRecord *prec)
{
    long status;
    status = dbPutLink(&(prec->out),DBR_LONG, &(prec->rval),1);
    return(0);
}

LOCAL long linconvAoUP(struct aoRecord *prec, int after)
{
    double eguf = prec->eguf;
    double egul = prec->egul;

    if(!after) return(0);
    prec->eslo = (eguf - egul)/(double)(UPRAWF - UPRAWL);
    prec->eoff = (UPRAWF*egul - UPRAWL*eguf)/(double)(UPRAWF - UPRAWL);
    return(0);
}

LOCAL long initAoOldBP(struct aoRecord *prec)
{
    if(prec->out.type == CONSTANT)
	recGblInitConstantLink(&prec->out,DBF_DOUBLE,&prec->val);
    linconvAoOldBP(prec,TRUE);
    prec->rval = (BPRAWF - BPRAWL)/2;
    return(0);
}

LOCAL long writeAoOldBP(struct aoRecord *prec)
{
    short rval = (short)prec->rval;

    rval += BPRAWL;
    dbPutLink(&(prec->out),DBR_SHORT, &rval,1);
    return(0);
}

LOCAL long linconvAoOldBP(struct aoRecord *prec, int after)
{
    if(!after) return(0);
    prec->eslo = (prec->eguf -prec->egul)/(double)(BPRAWF - BPRAWL);
    return(0);
}

LOCAL long initAoRoffBP(struct aoRecord *prec)
{
    if(prec->out.type == CONSTANT)
	recGblInitConstantLink(&prec->out,DBF_DOUBLE,&prec->val);
    linconvAoRoffBP(prec,TRUE);
    prec->rval = 0;
    return(0);
}

LOCAL long writeAoRoffBP(struct aoRecord *prec)
{
    long status;
    status = dbPutLink(&(prec->out),DBR_LONG, &(prec->rval),1);
    return(0);
}

LOCAL long linconvAoRoffBP(struct aoRecord *prec, int after)
{
    if(!after) return(0);
    prec->eslo = (prec->eguf -prec->egul)/(double)(BPRAWF - BPRAWL);
    prec->roff = -BPRAWL;
    return(0);
}

LOCAL long initAoEoffBP(struct aoRecord *prec)
{
    if(prec->out.type == CONSTANT)
	recGblInitConstantLink(&prec->out,DBF_DOUBLE,&prec->val);
    linconvAoEoffBP(prec,TRUE);
    prec->rval = 0;
    return(0);
}

LOCAL long writeAoEoffBP(struct aoRecord *prec)
{
    long status;
    status = dbPutLink(&(prec->out),DBR_LONG, &(prec->rval),1);
    return(0);
}

LOCAL long linconvAoEoffBP(struct aoRecord *prec, int after)
{
    double eguf = prec->eguf;
    double egul = prec->egul;

    if(!after) return(0);
    prec->eslo = (eguf - egul)/(double)(BPRAWF - BPRAWL);
    prec->eoff = (BPRAWF*egul - BPRAWL*eguf)/(double)(BPRAWF - BPRAWL);
    return(0);
}
