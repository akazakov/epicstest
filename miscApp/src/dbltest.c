#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "dbStaticLib.h"
#include "dbAccess.h"

long dbltest(char *filename)
{
    DBENTRY	dbentry;
    DBENTRY	*pdbentry=&dbentry;
    long	status;
    FILE	*stream = NULL;

    if(filename && strlen(filename)) {
        int fd;

        fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644 );
        if(fd<0) {
            fprintf(stderr,"%s could not be created\n",filename);
            return(0);
        }
        stream = fdopen(fd,"w+");
        if(stream==NULL) {
            fprintf(stderr,"%s could not be opened for output\n",filename);
            return(0);
        }
    } else {
        stream = stdout;
    }
    dbInitEntry(pdbbase,pdbentry);
    status = dbFirstRecordType(pdbentry);
    if(status) {
	fprintf(stream,"No record description\n");
    }
    while(!status) {
	status = dbFirstRecord(pdbentry);
	while(!status) {
	    fprintf(stream,"%s\n",dbGetRecordName(pdbentry));
            status = dbNextRecord(pdbentry);
	}
	status = dbNextRecordType(pdbentry);
    }
    dbFinishEntry(pdbentry);
    if(stream==stdout) {
        fflush(stream);
    } else {
        fclose(stream);
    }
    return(0);
}
