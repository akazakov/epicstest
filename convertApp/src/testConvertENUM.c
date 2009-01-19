/*convertTestENUM.c*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cadef.h"

#define nvalues 2
static char *dtypValue[nvalues] = {"Soft Channel","Raw Soft Channel"};
static char *prioValue[nvalues] = {"LOW","HIGH"};
static char *valValue[nvalues] = {"state 1","state 4"};

static chid putCchid[3];
static chid putMDbchid[3];
static chid putMCachid[3];
static chid getCchid[3];
static chid getMDbchid[3];
static chid getMCachid[3];

static char getCvalue[3][MAX_STRING_SIZE];
static char getMDbvalue[3][MAX_STRING_SIZE];
static char getMCavalue[3][MAX_STRING_SIZE];

void printReport(int indval)
{

  printf("\nPut Value           enumCmbbi           enumMDbmbbi         enumMCambbi\n\n");
    printf("%-20.20s%-20.20s%-20.20s%-20.20s\n",
	dtypValue[indval],getCvalue[0],getMDbvalue[0],getMCavalue[0]);
    printf("%-20.20s%-20.20s%-20.20s%-20.20s\n",
	prioValue[indval],getCvalue[1],getMDbvalue[1],getMCavalue[1]);
    printf("%-20.20s%-20.20s%-20.20s%-20.20s\n",
	valValue[indval],getCvalue[2],getMDbvalue[2],getMCavalue[2]);
}

int main(int argc,char **argv)
{
  int indval,field;

  SEVCHK(ca_task_initialize(),"ca_task_initialize");
  SEVCHK(ca_search("enumCputDTYP",&putCchid[0]),"ca_search failure");
  SEVCHK(ca_search("enumCputPRIO",&putCchid[1]),"ca_search failure");
  SEVCHK(ca_search("enumCputVAL" ,&putCchid[2]),"ca_search failure");
  SEVCHK(ca_search("enumMDbputDTYP",&putMDbchid[0]),"ca_search failure");
  SEVCHK(ca_search("enumMDbputPRIO",&putMDbchid[1]),"ca_search failure");
  SEVCHK(ca_search("enumMDbputVAL" ,&putMDbchid[2]),"ca_search failure");
  SEVCHK(ca_search("enumMCaputDTYP",&putMCachid[0]),"ca_search failure");
  SEVCHK(ca_search("enumMCaputPRIO",&putMCachid[1]),"ca_search failure");
  SEVCHK(ca_search("enumMCaputVAL" ,&putMCachid[2]),"ca_search failure");
  SEVCHK(ca_search("enumCmbbi.DTYP" ,&getCchid[0]),"ca_search failure");
  SEVCHK(ca_search("enumCmbbi.PRIO" ,&getCchid[1]),"ca_search failure");
  SEVCHK(ca_search("enumCmbbi.VAL" ,&getCchid[2]),"ca_search failure");
  SEVCHK(ca_search("enumMDbmbbi.DTYP" ,&getMDbchid[0]),"ca_search failure");
  SEVCHK(ca_search("enumMDbmbbi.PRIO" ,&getMDbchid[1]),"ca_search failure");
  SEVCHK(ca_search("enumMDbmbbi.VAL" ,&getMDbchid[2]),"ca_search failure");
  SEVCHK(ca_search("enumMCambbi.DTYP" ,&getMCachid[0]),"ca_search failure");
  SEVCHK(ca_search("enumMCambbi.PRIO" ,&getMCachid[1]),"ca_search failure");
  SEVCHK(ca_search("enumMCambbi.VAL" ,&getMCachid[2]),"ca_search failure");
  SEVCHK(ca_pend_io(5.0),"ca_pend_io failure");

  for(indval=0; indval<2; indval++) {
      SEVCHK(ca_put(DBR_STRING,putCchid[0],dtypValue[indval]),"ca_put");
      SEVCHK(ca_put(DBR_STRING,putCchid[1],prioValue[indval]),"ca_put");
      SEVCHK(ca_put(DBR_STRING,putCchid[2],valValue[indval]),"ca_put");
      SEVCHK(ca_put(DBR_STRING,putMDbchid[0],dtypValue[indval]),"ca_put");
      SEVCHK(ca_put(DBR_STRING,putMDbchid[1],prioValue[indval]),"ca_put");
      SEVCHK(ca_put(DBR_STRING,putMDbchid[2],valValue[indval]),"ca_put");
      SEVCHK(ca_put(DBR_STRING,putMCachid[0],dtypValue[indval]),"ca_put");
      SEVCHK(ca_put(DBR_STRING,putMCachid[1],prioValue[indval]),"ca_put");
      SEVCHK(ca_put(DBR_STRING,putMCachid[2],valValue[indval]),"ca_put");
      /*Wait until evertthing updated*/
      ca_pend_event(2.0);
      for(field=0; field<3; field++) {
	SEVCHK(ca_get(DBR_STRING,getCchid[field],&getCvalue[field]),
		"ca_get");
	SEVCHK(ca_get(DBR_STRING,getMDbchid[field],&getMDbvalue[field]),
		"ca_get");
	SEVCHK(ca_get(DBR_STRING,getMCachid[field],&getMCavalue[field]),
		"ca_get");
      }
      SEVCHK(ca_pend_io(5.0),"ca_pend_io failure");
      printReport(indval);
  }
  return(0);
}
