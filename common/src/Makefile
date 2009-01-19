TOP=../..

include $(TOP)/configure/CONFIG

DBD += common.dbd

PROD_IOC += common

common_SRCS += devAiAsyn.c
common_SRCS += common_registerRecordDeviceDriver.cpp
common_SRCS += commonMain.cpp

common_LIBS += testDevIoc
common_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
