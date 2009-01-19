#include        <stdlib.h>
#include        <stdarg.h>
#include        <stdio.h>
#include        <string.h>
 
#include "epicsPrint.h"
#include "errMdef.h"
#include "error.h"
#include "errlog.h"

extern "C" {
void errStatusTest(void);
}

int main(int argc,char **argv)
{
    errStatusTest();
    return(0);
}
