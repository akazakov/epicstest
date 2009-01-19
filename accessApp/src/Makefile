TOP=../..

include $(TOP)/configure/CONFIG

PROD_HOST += createAccessDb

DBD+= access.dbd

PROD_IOC += access

access_SRCS += access_registerRecordDeviceDriver.cpp
access_SRCS_DEFAULT += accessMain.cpp
access_SRCS_vxWorks += -nil-

access_SRCS += asinittest.c
access_SRCS += asTest.c
access_SRCS += asTrapWriteTest.c

#The following adds support from base/src/vxWorks
access_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

access_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
