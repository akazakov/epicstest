/*caExample.c*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cadef.h"

static char *arrayTypeName[2] = {"S","A"};

#define ntypes 9
char *typeName[ntypes] = {
	"STRING","CHAR","UCHAR","SHORT","USHORT","LONG",
	"ULONG","FLOAT","DOUBLE"};

static chid putchid[2][ntypes];
static chid MputDbchid[2][ntypes][ntypes];
static chid MputCachid[2][ntypes][ntypes];
static chid MgetDbchid[2][ntypes][ntypes];
static chid MgetCachid[2][ntypes][ntypes];
static chid Cputchid[2][ntypes][ntypes];
static chid Cgetchid[2][ntypes][ntypes];

#define	nvalues 13
static char value[nvalues][MAX_STRING_SIZE] = {
	"0.0","1.0","-1.0","127","128",
	"32767","32768","2147483647","2147483648",
	"0.01","-0.01","1e9","-1e9"};
/*arrays to hold return values*/
static double putvalue[2][ntypes][nvalues];
static double MputDbvalue[2][ntypes][ntypes][nvalues];
static double MputCavalue[2][ntypes][ntypes][nvalues];
static double MgetDbvalue[2][ntypes][ntypes][nvalues];
static double MgetCavalue[2][ntypes][ntypes][nvalues];
static double Cputvalue[2][ntypes][ntypes][nvalues];
static double Cgetvalue[2][ntypes][ntypes][nvalues];

static volatile int nGetsToGo = 0;

static void caEventHandler(struct event_handler_args arg)
{
    struct dbr_time_double *rtnval;
    double 	*put;
    double	*val;
    int			i,nelements;

    if(arg.status != ECA_NORMAL) {
	printf("caEventHandler error %s pv %s\n",
	    ca_message(arg.status),ca_name(arg.chid));
	return;
    }
    nGetsToGo--;
    put = (double *)arg.usr;
    nelements = arg.count;
    rtnval = (struct dbr_time_double*)arg.dbr;
    val = &rtnval->value;
    for(i=0;i<nelements; i++) {
	put[i] = val[i];
    }
}

static char *format[ntypes] = {
	"%15.2f",
	"%5.0f","%6.0f",
	"%7.0f","%7.0f",
	"%12.0f","%12.0f",
	"%15.2f","%15.2f"};

void printReport(int arrayType)
{
  int	row,col,indval;

  printf("\f\n");
  printf("Type            STRING CHAR UCHAR  SHORT USHORT");
  printf("        LONG       ULONG          FLOAT         DOUBLE\n");
  for(indval=0; indval<nvalues; indval++) {
    for(row=0; row<ntypes; row++) {
      printf("value \"%s\" %sput%s %.2f\n",
	value[indval],arrayTypeName[arrayType],
	typeName[row],putvalue[arrayType][row][indval]);
      printf("MputDb ");
      for(col=0; col<ntypes; col++) {
        printf(format[col], MputDbvalue[arrayType][row][col][indval]);
      }
      printf("\n");
      printf("MputCa ");
      for(col=0; col<ntypes; col++) {
        printf(format[col], MputCavalue[arrayType][row][col][indval]);
      }
      printf("\n");
      printf("MgetDb ");
      for(col=0; col<ntypes; col++) {
        printf(format[col], MgetDbvalue[arrayType][row][col][indval]);
      }
      printf("\n");
      printf("MgetCa ");
      for(col=0; col<ntypes; col++) {
        printf(format[col], MgetCavalue[arrayType][row][col][indval]);
      }
      printf("\n");
      printf("Cput   ");
      for(col=0; col<ntypes; col++) {
        printf(format[col], Cputvalue[arrayType][row][col][indval]);
      }
      printf("\n");
      printf("Cget   ");
      for(col=0; col<ntypes; col++) {
        printf(format[col], Cgetvalue[arrayType][row][col][indval]);
      }
      printf("\n");
    }
  }
}

int main(int argc,char **argv)
{
  char	prefix[20];
  char	pvname[40];
  int	ind;
  int   arrayType,row,col;
  int	ntrys;

  SEVCHK(ca_task_initialize(),"ca_task_initialize");
  for(arrayType=0; arrayType<=1; arrayType++) {
    sprintf(prefix,"%s",arrayTypeName[arrayType]);
    for(row=0;row<ntypes;row++) {
	sprintf(pvname,"%sput%s",prefix,typeName[row]);
        SEVCHK(ca_search(pvname,&putchid[arrayType][row]),"ca_search failure");
	for(col=0;col<ntypes;col++) {
	    sprintf(pvname,"%sMputDb%sto%s",
		prefix,typeName[row],typeName[col]);
            SEVCHK(ca_search(pvname,&MputDbchid[arrayType][row][col]),
		"ca_search failure");
	    sprintf(pvname,"%sMputCa%sto%s",
		prefix,typeName[row],typeName[col]);
            SEVCHK(ca_search(pvname,&MputCachid[arrayType][row][col]),
		"ca_search failure");
	    sprintf(pvname,"%sMgetDb%sto%s",
		prefix,typeName[row],typeName[col]);
            SEVCHK(ca_search(pvname,&MgetDbchid[arrayType][row][col]),
		"ca_search failure");
	    sprintf(pvname,"%sMgetCa%sto%s",
		prefix,typeName[row],typeName[col]);
            SEVCHK(ca_search(pvname,&MgetCachid[arrayType][row][col]),
		"ca_search failure");
	    sprintf(pvname,"%sCput%sto%s",
		prefix,typeName[row],typeName[col]);
            SEVCHK(ca_search(pvname,&Cputchid[arrayType][row][col]),
		"ca_search failure");
	    sprintf(pvname,"%sCget%sto%s",
		prefix,typeName[row],typeName[col]);
            SEVCHK(ca_search(pvname,&Cgetchid[arrayType][row][col]),
		"ca_search failure");
	}
    }
  }
  SEVCHK(ca_pend_io(60.0),"ca_pend_io failure");
  /*handle arrayType=0 and 1 separately*/
  arrayType = 0;
  sprintf(prefix,"%s",arrayTypeName[arrayType]);
  for(row=0;row<ntypes;row++) {
    for(ind=0; ind<nvalues; ind++) {
      SEVCHK(ca_array_put(DBR_STRING,1,putchid[arrayType][row],value[ind]),
	"ca_array_put");
      SEVCHK(ca_pend_io(5.0),"ca_pend_io failure");
      ca_pend_event(.4);
      SEVCHK(ca_array_get(DBR_DOUBLE,1,putchid[arrayType][row],
	&putvalue[arrayType][row][ind]),"ca_array_get");
      SEVCHK(ca_pend_io(5.0),"ca_pend_io failure");
      nGetsToGo = 0;
      for(col=0;col<ntypes;col++) {
	nGetsToGo += 6;
	SEVCHK(ca_get_callback(DBR_TIME_DOUBLE,
		MputDbchid[arrayType][row][col],
		caEventHandler,&MputDbvalue[arrayType][row][col][ind]),
		"ca_get_callback");
	SEVCHK(ca_get_callback(DBR_TIME_DOUBLE,
		MputCachid[arrayType][row][col],
		caEventHandler,&MputCavalue[arrayType][row][col][ind]),
		"ca_get_callback");
	SEVCHK(ca_get_callback(DBR_TIME_DOUBLE,
		MgetDbchid[arrayType][row][col],
		caEventHandler,&MgetDbvalue[arrayType][row][col][ind]),
		"ca_get_callback");
	SEVCHK(ca_get_callback(DBR_TIME_DOUBLE,
		MgetCachid[arrayType][row][col],
		caEventHandler,&MgetCavalue[arrayType][row][col][ind]),
		"ca_get_callback");
	SEVCHK(ca_get_callback(DBR_TIME_DOUBLE,
		Cputchid[arrayType][row][col],
		caEventHandler,&Cputvalue[arrayType][row][col][ind]),
		"ca_get_callback");
	SEVCHK(ca_get_callback(DBR_TIME_DOUBLE,
		Cgetchid[arrayType][row][col],
		caEventHandler,&Cgetvalue[arrayType][row][col][ind]),
		"ca_get_callback");
      }
      ntrys = 0;
      while(nGetsToGo>0 && (ntrys++ < 1000)) {
	ca_pend_event(.01);
      }
    }
  }
  printReport(arrayType);
  arrayType = 1;
  sprintf(prefix,"%s",arrayTypeName[arrayType]);
  for(row=0;row<ntypes;row++) {
      SEVCHK(ca_array_put(DBR_STRING,nvalues,putchid[arrayType][row],value),
	"ca_array_put");
      SEVCHK(ca_pend_io(5.0),"ca_pend_io failure");
      ca_pend_event(.4);
      SEVCHK(ca_array_get(DBR_DOUBLE,nvalues,putchid[arrayType][row],
	&putvalue[arrayType][row]),"ca_array_get");
      SEVCHK(ca_pend_io(5.0),"ca_pend_io failure");
      nGetsToGo = 0;
      for(col=0;col<ntypes;col++) {
	nGetsToGo += 6;
	SEVCHK(ca_array_get_callback(DBR_TIME_DOUBLE,nvalues,
		MputDbchid[arrayType][row][col],
		caEventHandler,&MputDbvalue[arrayType][row][col]),
		"ca_array_get_callback");
	SEVCHK(ca_array_get_callback(DBR_TIME_DOUBLE,nvalues,
		MputCachid[arrayType][row][col],
		caEventHandler,&MputCavalue[arrayType][row][col]),
		"ca_array_get_callback");
	SEVCHK(ca_array_get_callback(DBR_TIME_DOUBLE,nvalues,
		MgetDbchid[arrayType][row][col],
		caEventHandler,&MgetDbvalue[arrayType][row][col]),
		"ca_array_get_callback");
	SEVCHK(ca_array_get_callback(DBR_TIME_DOUBLE,nvalues,
		MgetCachid[arrayType][row][col],
		caEventHandler,&MgetCavalue[arrayType][row][col]),
		"ca_array_get_callback");
	SEVCHK(ca_array_get_callback(DBR_TIME_DOUBLE,nvalues,
		Cputchid[arrayType][row][col],
		caEventHandler,&Cputvalue[arrayType][row][col]),
		"ca_array_get_callback");
	SEVCHK(ca_array_get_callback(DBR_TIME_DOUBLE,nvalues,
		Cgetchid[arrayType][row][col],
		caEventHandler,&Cgetvalue[arrayType][row][col]),
		"ca_array_get_callback");
      }
      ntrys = 0;
      while(nGetsToGo>0 && (ntrys++ < 1000)) {
	ca_pend_event(.01);
      }
  }
  printReport(arrayType);
  return(0);
}
