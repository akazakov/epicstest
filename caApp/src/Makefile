TOP=../..

include $(TOP)/configure/CONFIG

PROD_HOST += createdb
createdb_SRCS += createdb.c
PROD_HOST += testcaput
testcaput_SRCS += testcaput.c 
testcaput_LIBS  += $(EPICS_BASE_HOST_LIBS)
PROD_HOST += testcaget
testcaget_SRCS += testcaget.c 
testcaget_LIBS  += $(EPICS_BASE_HOST_LIBS)
PROD_HOST += testcaconnect
testcaconnect_SRCS += testcaconnect.c 
testcaconnect_LIBS  += $(EPICS_BASE_HOST_LIBS)
PROD_HOST += testcagetNew
testcagetNew_SRCS += testcagetNew.c 
testcagetNew_LIBS  += $(EPICS_BASE_HOST_LIBS)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
