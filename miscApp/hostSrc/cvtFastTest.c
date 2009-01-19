/* Test program for cvtfast.c */

#define STRING_LEN 80

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "cvtFast.h"

int main(int argc, char **argv)
{
    long lval,oval,hval,oval1,hval1;
    int i,ostatus,hstatus;
    char *string,ostring[STRING_LEN],hstring[STRING_LEN];

    printf("\nTest cvtLongToOctalString and cvtLongToHexString\n");

    lval = 0;
    for(i=0; i < 5; i++) {
	switch(i) {
	case 0:
	  /* LONG_MAX */
	    string="LONG_MAX";
	    lval=LONG_MAX;
	    break;
	case 1:
	  /* LONG_MIN */
	    string="LONG_MIN";
	    lval=LONG_MIN;
	    break;
	case 2:
	  /* -1 */
	    string="-1";
	    lval=-1;
	    break;
	case 3:
	  /* 0 */
	    string="0";
	    lval=0;
	    break;
	case 4:
	  /* 1 */
	    string="1";
	    lval=1;
	    break;
	}
	ostatus=cvtLongToOctalString(lval,ostring);
	if(ostatus) {
	    oval=strtol(ostring,NULL,0);
	    sscanf(ostring,"%li",&oval1);
	} else {
	    oval=oval1=0;
	}
	hstatus=cvtLongToHexString(lval,hstring);
	if(hstatus) {
	    hval=strtol(hstring,NULL,0);
	    sscanf(hstring,"%li",&hval1);
	} else {
	    hval=hval1=0;
	}
	printf("%s\n"
	  "  val=%ld\n"
	  "  cvtLongToOctalString: \"%s\"\n"
	  "    strtol:             %ld %s\n"
	  "    scanf[%%li]:         %ld %s\n"
	  "  cvtLongToHexString:   \"%s\"\n"
	  "    strtol:             %ld %s\n"
	  "    scanf[%%li]:         %ld %s\n",
	  string,lval,
	  ostatus?ostring:"Failed",
	  oval,lval==oval?"":"Error",
	  oval1,lval==oval1?"":"Error",
	  hstatus?hstring:"Failed",
	  hval,lval==hval?"":"Error",
	  hval1,lval==hval1?"":"Error");
    }

    return(0);
}
