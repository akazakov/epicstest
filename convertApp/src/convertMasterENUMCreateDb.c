/*convertMasterENUM.c - Create convertMasterENUM.db*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define nfields 3
static char *field[nfields] = {"PRIO","DTYP","VAL"};

int main(int argc,char **argv)
{
    int		i,ifield;

    /*First create mbbi records*/
    for(i=0; i<2; i++) {
	if(i==0)
	    printf("record(mbbi,\"enumMDbmbbi\") {\n");
	else
	    printf("record(mbbi,\"enumMCambbi\") {\n");
	printf("\tfield(ONST,\"state 1\")\n");
	printf("\tfield(TWST,\"state 2\")\n");
	printf("\tfield(THST,\"state 3\")\n");
	printf("\tfield(FRST,\"state 4\")\n");
	printf("}\n");
    }
    /*Create records that interface to client*/
    for(ifield=0; ifield<nfields; ifield++) {
	printf("record(type,\"enumCget%s\") {\n",field[ifield]);
	printf("\tfield(FTVL,\"STRING\")\n");
	printf("\tfield(INP,\"enumCmbbi.%s CP\")\n",field[ifield]);
	printf("}\n");
	printf("record(type,\"enumCput%s\") {\n",field[ifield]);
	printf("\tfield(FTVL,\"STRING\")\n");
	printf("\tfield(OUT,\"enumCmbbi.%s CA\")\n",field[ifield]);
	printf("}\n");
    }
    /*Create records that interface to master*/
    for(ifield=0; ifield<nfields; ifield++) {
	printf("record(type,\"enumMDbget%s\") {\n",field[ifield]);
	printf("\tfield(SCAN,\".1 second\")\n");
	printf("\tfield(FTVL,\"STRING\")\n");
	printf("\tfield(INP,\"enumMDbmbbi.%s NPP\")\n",field[ifield]);
	printf("}\n");
	printf("record(type,\"enumMDbput%s\") {\n",field[ifield]);
	printf("\tfield(FTVL,\"STRING\")\n");
	printf("\tfield(OUT,\"enumMDbmbbi.%s PP\")\n",field[ifield]);
	printf("}\n");
	printf("record(type,\"enumMCaget%s\") {\n",field[ifield]);
	printf("\tfield(FTVL,\"STRING\")\n");
	printf("\tfield(INP,\"enumMCambbi.%s CP\")\n",field[ifield]);
	printf("}\n");
	printf("record(type,\"enumMCaput%s\") {\n",field[ifield]);
	printf("\tfield(FTVL,\"STRING\")\n");
	printf("\tfield(OUT,\"enumMCambbi.%s CA\")\n",field[ifield]);
	printf("}\n");
    }
    return(0);
}
