/*convertMaster.c - Create convertMaster.db*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char *arrayTypeName[2] = {"S","A"};
static int arrayElem[2] = {1,13};
#define nTypes 9
char *typeName[nTypes] = {
	"STRING","CHAR","UCHAR","SHORT","USHORT","LONG",
	"ULONG","FLOAT","DOUBLE"};

int main(int argc,char **argv)
{
  int arrayType,row,col;
  char	prefix[20];

  for(arrayType=0; arrayType<=1; arrayType++) {
    sprintf(prefix,"%s",arrayTypeName[arrayType]);
    for(row=0;row<nTypes;row++) {
	/*Master record to which the CA Unix task will write*/
	printf("record(type,\"%sput%s\") {\n",prefix,typeName[row]);
	printf("\tfield(NELM,\"%d\")\n",arrayElem[arrayType]);
	printf("\tfield(FTVL,\"%s\")\n",typeName[row]);
	printf("\tfield(FLNK,\"%sFanout%s\")\n",prefix,typeName[row]);
	printf("}\n");
	/*Fanout for Master and for Client */
	printf("record(fanout,\"%sFanout%s\") {\n",prefix,typeName[row]);
	printf("\tfield(LNK1,\"%sMtoMDb%sto%s\")\n",
		prefix,typeName[row],typeName[0]);
	printf("\tfield(LNK2,\"%sMtoMCa%sto%s\")\n",
		prefix,typeName[row],typeName[0]);
	printf("\tfield(LNK3,\"%sMtoC%sto%s\")\n",
		prefix,typeName[row],typeName[0]);
	printf("}\n");
	/*generate records that fanout type*/
	for(col=0;col<nTypes;col++) {
	    printf("record(type,\"%sMtoMDb%sto%s\") {\n",
		prefix,typeName[row],typeName[col]);
	    printf("\tfield(NELM,\"%d\")\n",arrayElem[arrayType]);
	    printf("\tfield(FTVL,\"%s\")\n",typeName[row]);
	    printf("\tfield(INP,\"%sput%s NPP\")\n",prefix,typeName[row]);
	    printf("\tfield(OUT,\"%sMputDb%sto%s PP\")\n",
		prefix,typeName[row],typeName[col]);
	    if(col<(nTypes-1))
		printf("\tfield(FLNK,\"%sMtoMDb%sto%s\")\n",
		    prefix,typeName[row],typeName[col+1]);
	    printf("}\n");
	    printf("record(type,\"%sMtoMCa%sto%s\") {\n",
		prefix,typeName[row],typeName[col]);
	    printf("\tfield(NELM,\"%d\")\n",arrayElem[arrayType]);
	    printf("\tfield(FTVL,\"%s\")\n",typeName[row]);
	    printf("\tfield(INP,\"%sput%s NPP\")\n",prefix,typeName[row]);
	    printf("\tfield(OUT,\"%sMputCa%sto%s CA\")\n",
		prefix,typeName[row],typeName[col]);
	    if(col<(nTypes-1))
		printf("\tfield(FLNK,\"%sMtoMCa%sto%s\")\n",
		    prefix,typeName[row],typeName[col+1]);
	    printf("}\n");
	    printf("record(type,\"%sMtoC%sto%s\") {\n",
		prefix,typeName[row],typeName[col]);
	    printf("\tfield(NELM,\"%d\")\n",arrayElem[arrayType]);
	    printf("\tfield(FTVL,\"%s\")\n",typeName[row]);
	    printf("\tfield(INP,\"%sput%s NPP\")\n",prefix,typeName[row]);
	    printf("\tfield(OUT,\"%sCput%sto%s CA\")\n",
		prefix,typeName[row],typeName[col]);
	    if(col<(nTypes-1))
		printf("\tfield(FLNK,\"%sMtoC%sto%s\")\n",
		    prefix,typeName[row],typeName[col+1]);
	    printf("}\n");
	}
	for(col=0;col<nTypes;col++) {
	    /*Generate records that get value from local put*/
	    printf("record(type,\"%sMputDb%sto%s\") {\n",
		prefix,typeName[row],typeName[col]);
	    printf("\tfield(NELM,\"%d\")\n",arrayElem[arrayType]);
	    printf("\tfield(PREC,\"2\")\n");
	    printf("\tfield(FTVL,\"%s\")\n",typeName[col]);
	    printf("}\n");
	    printf("record(type,\"%sMputCa%sto%s\") {\n",
		prefix,typeName[row],typeName[col]);
	    printf("\tfield(NELM,\"%d\")\n",arrayElem[arrayType]);
	    printf("\tfield(PREC,\"2\")\n");
	    printf("\tfield(FTVL,\"%s\")\n",typeName[col]);
	    printf("}\n");
	    /*Generate records that get value from local put*/
	    printf("record(type,\"%sMgetDb%sto%s\") {\n",
		prefix,typeName[row],typeName[col]);
	    printf("\tfield(SCAN,\".1 second\")\n");
	    printf("\tfield(NELM,\"%d\")\n",arrayElem[arrayType]);
	    printf("\tfield(FTVL,\"%s\")\n",typeName[col]);
	    printf("\tfield(INP,\"%sMtoMDb%sto%s NPP\")\n",
		prefix,typeName[row],typeName[col]);
	    printf("}\n");
	    printf("record(type,\"%sMgetCa%sto%s\") {\n",
		prefix,typeName[row],typeName[col]);
	    printf("\tfield(NELM,\"%d\")\n",arrayElem[arrayType]);
	    printf("\tfield(FTVL,\"%s\")\n",typeName[col]);
	    printf("\tfield(INP,\"%sMtoMDb%sto%s CP\")\n",
		prefix,typeName[row],typeName[col]);
	    printf("}\n");
	}
    }
  }
  return(0);
}
