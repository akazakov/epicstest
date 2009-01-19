#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void commonFields(void)
{
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
	printf("usage: createIocMaster <nrecords>\n");
	exit(1);
    }
    sscanf(argv[1],"%d",&npv);
    printf("record(event,\"mrkMasterEvent\") {\n");
    printf("\tfield(VAL,\"1\")\n");
    printf("}\n");
    printf("record(fanout,\"mrkFanout\") {\n");
    printf("\tfield(LNK1,\"mrkMasterCalc0000\")\n");
    printf("\tfield(LNK2,\"mrkMasterCalc0001\")\n");
    printf("\tfield(LNK3,\"mrkMasterCalc0002\")\n");
    printf("\tfield(LNK4,\"mrkMasterCalc0003\")\n");
    printf("\tfield(LNK5,\"mrkMasterCalc0004\")\n");
    printf("\tfield(LNK6,\"mrkMasterCalc0005\")\n");
    printf("}\n");
    for(i=0; i<npv; i++) {
	printf("record(calc,\"mrkMasterCalc%04d\") {\n",i);
	printf("\tfield(SCAN, \"Event\")\n");
	printf("\tfield(EVNT, \"1\")\n");
	printf("\tfield(CALC, \"(A<B)?(A+C):D\")\n");
	printf("\tfield(INPA, \"mrkMasterCalc%04d.VAL  NPP NMS\")\n",i);
	printf("\tfield(INPB, \"9\")\n");
	printf("\tfield(INPC, \"1\")\n");
	printf("\tfield(FLNK, \"mrkMasterAo%04d\")\n",i);
	commonFields();
	printf("}\n");
	printf("record(ao,\"mrkMasterAo%04d\") {\n",i);
	printf("\tfield(DOL, \"mrkMasterCalc%04d.VAL  NPP NMS\")\n",i);
	printf("\tfield(OMSL, \"closed_loop\")\n");
	printf("\tfield(OUT, \"mrkClient%04d\")\n",i);
	printf("\tfield(EGUF, \"10\")"); printf("\n");
	commonFields();
	printf("}\n");
	printf("record(ai,\"mrkMasterAiFromClient%04d\") {\n",i);
	printf("\tfield(SCAN, \"1 second\")\n");
	printf("\tfield(INP, \"mrkClient%04d.VAL  NPP NMS\")\n",i);
	printf("\tfield(EGUF, \"10\")"); printf("\n");
	commonFields();
	printf("}\n");
	printf("record(ai,\"mrkMasterAiFromMaster%04d\") {\n",i);
	printf("\tfield(EGUF, \"10\")"); printf("\n");
	commonFields();
	printf("}\n");
    }
    return(0);
}
