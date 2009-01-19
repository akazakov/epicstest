/*convertClient.c - create convertClient.db*/
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
	for(col=0;col<nTypes;col++) {
	    /*Generate records that get value from Master put*/
	    printf("record(type,\"%sCput%sto%s\") {\n",
		prefix,typeName[row],typeName[col]);
	    printf("\tfield(NELM,\"%d\")\n",arrayElem[arrayType]);
	    printf("\tfield(PREC,\"2\")\n");
	    printf("\tfield(FTVL,\"%s\")\n",typeName[col]);
	    printf("}\n");
	    /*Generate records that get value via CP from Master*/
	    printf("record(type,\"%sCget%sto%s\") {\n",
		prefix,typeName[row],typeName[col]);
	    printf("\tfield(NELM,\"%d\")\n",arrayElem[arrayType]);
	    printf("\tfield(FTVL,\"%s\")\n",typeName[col]);
	    printf("\tfield(INP,\"%sMtoC%sto%s CP\")\n",
		prefix,typeName[row],typeName[col]);
	    printf("}\n");
	}
    }
  }
  return(0);
}
