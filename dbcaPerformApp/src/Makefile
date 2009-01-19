TOP=../..

include $(TOP)/configure/CONFIG

PROD_HOST += changeLinks
changeLinks_SRCS  += changeLinks.c
changeLinks_LIBS  += $(EPICS_BASE_HOST_LIBS)
PROD_HOST += createIocClient
createIocClient_SRCS  += createIocClient.c
PROD_HOST += createIocMaster
createIocMaster_SRCS  += createIocMaster.c

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
