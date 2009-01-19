TOP=../..

include $(TOP)/configure/CONFIG

ca_DIR  = $(EPICS_BASE_LIB)
Com_DIR = $(EPICS_BASE_LIB)

PROD_HOST += camonitoralarm
PROD_HOST += cagetalarm
PROD_HOST += caputacks
PROD_HOST += caputackt
PROD_HOST += cagetclassname

camonitoralarm_LIBS += ca Com
cagetalarm_LIBS += ca Com
caputacks_LIBS += ca Com
caputackt_LIBS += ca Com
cagetclassname_LIBS += ca Com

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
