#include <symLib.h>

char *adddr_of_malloc = NULL;
char *adddr_of_calloc = NULL;
char *adddr_of_free = NULL;

extern SYMTAB_ID sysSymTbl;

int find_malloc(void)
{
 SYM_TYPE type;
 STATUS   status;

 status  = symFindByName(sysSymTbl,
                         "_malloc",
                         &adddr_of_malloc,
                         &type);

 status |= symFindByName(sysSymTbl,
                         "_calloc",
                         &adddr_of_calloc,
                         &type);

 status |= symFindByName(sysSymTbl,
                         "_free",
                         &adddr_of_free,
                         &type);

 return status;
}
