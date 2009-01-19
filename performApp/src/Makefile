TOP=../..
include $(TOP)/configure/CONFIG

DBD += perform.dbd

PROD_IOC += perform

perform_SRCS += testChangeLink.c
perform_SRCS += timeSem.c
perform_SRCS += timedbNameToAddr.c
perform_SRCS += timeproc.c
perform_SRCS += perform_registerRecordDeviceDriver.cpp
perform_SRCS_DEFAULT += performMain.cpp
perform_SRCS_vxWorks += -nil-

perform_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
