TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

PROD_LIBS += rsrvIoc
PROD_LIBS += dbtoolsIoc
PROD_LIBS += miscIoc
PROD_LIBS += asIoc
PROD_LIBS += dbIoc
PROD_LIBS += registryIoc
PROD_LIBS += dbStaticIoc
PROD_LIBS += ca
PROD_LIBS += Com

rsrvIoc_DIR = $(EPICS_BASE_LIB)
dbtoolsIoc_DIR = $(EPICS_BASE_LIB)
miscIoc_DIR = $(EPICS_BASE_LIB)
asIoc_DIR = $(EPICS_BASE_LIB)
dbIoc_DIR = $(EPICS_BASE_LIB)
registryIoc_DIR = $(EPICS_BASE_LIB)
dbStaticIoc_DIR = $(EPICS_BASE_LIB)
ca_DIR = $(EPICS_BASE_LIB)
Com_DIR = $(EPICS_BASE_LIB)

PROD_LIBS += miscTest
miscTest_DIR = $(INSTALL_LIB)

DBDNAME = commonApp.dbd

PROD += miscTest

miscTest_SRCS += registerRecordDeviceDriver.c
miscTest_SRCS += miscTest.c

-include ../../../common/baseLIBOBJS
miscTest_OBJS += $(LIBOBJS)

include $(TOP)/configure/RULES
include $(TOP)/configure/RULES.registerRecordDeviceDriver

#----------------------------------------
#  ADD RULES AFTER THIS LINE
