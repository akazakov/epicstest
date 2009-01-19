/*convertClientENUM.c - create convertClientENUM.db*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc,char **argv)
{
    printf("record(mbbi,\"enumCmbbi\") {\n");
    printf("\tfield(ONST,\"state 1\")\n");
    printf("\tfield(TWST,\"state 2\")\n");
    printf("\tfield(THST,\"state 3\")\n");
    printf("\tfield(FRST,\"state 4\")\n");
    printf("}\n");
    return(0);
}
