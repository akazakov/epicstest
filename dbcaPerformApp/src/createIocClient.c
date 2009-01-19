#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void commonFields(void)
{
	printf("\tfield(EGUF, \"10\")"); printf("\n");
	printf("\tfield(EGU,  \"Counts\")"); printf("\n");
	printf("\tfield(HOPR, \"10\")"); printf("\n");
	printf("\tfield(HIHI, \"8\")"); printf("\n");
	printf("\tfield(LOLO, \"2\")"); printf("\n");
	printf("\tfield(HIGH, \"6\")"); printf("\n");
	printf("\tfield(LOW,  \"4\")"); printf("\n");
	printf("\tfield(HHSV, \"MAJOR\")"); printf("\n");
	printf("\tfield(LLSV, \"MAJOR\")"); printf("\n");
	printf("\tfield(HSV,  \"MINOR\")"); printf("\n");
	printf("\tfield(LSV,  \"MINOR\")"); printf("\n");
}

int main(int argc,char **argv)
{
    int	npv;
    int i;

    if(argc!=2) {
	printf("usage: createIocClient <nrecords>\n");
	exit(1);
    }
    sscanf(argv[1],"%d",&npv);
    for(i=0; i<npv; i++) {
	printf("record(ai,\"mrkClient%04d\") {\n",i);
	commonFields();
	printf("}\n");
    }
    return(0);
}
