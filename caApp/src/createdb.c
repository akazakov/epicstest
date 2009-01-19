#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NPV 5000

int main(int argc,char **argv)
{
    int	npv;
    int i;

    if(argc!=2) {
	printf("usage: createdb <nrecords>\n");
	exit(1);
    }
    sscanf(argv[1],"%d",&npv);
    for(i=0; i<npv; i++) {
	printf("record(ai,\"mrk%04d\") {}\n",i);
    }
    return(0);
}
