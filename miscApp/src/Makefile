TOP=../..
include $(TOP)/configure/CONFIG

DBD += misc.dbd

PROD_IOC = misc

misc_SRCS += dbcaattrib.c
misc_SRCS += dbltest.c
misc_SRCS += devTestLinear.c
misc_SRCS += errStatusTest.c
misc_SRCS += errlogTest.c
misc_SRCS_vxWorks += fdUsage.c
misc_SRCS_vxWorks += find_malloc.c
misc_SRCS += generrMessage.c
misc_SRCS += generrlogPrintf.c
misc_SRCS += paritytest.c
misc_SRCS += testConvertFast.c
misc_SRCS += timeStampSub.c

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
misc_SRCS += misc_registerRecordDeviceDriver.cpp
misc_SRCS_DEFAULT += miscMain.cpp
misc_SRCS_vxWorks += -nil-

misc_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
