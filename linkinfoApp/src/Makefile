TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#USR_CFLAGS += 

#=============================

# <name>.dbd will be created from <name>Include.dbd
DBD += linkinfo.dbd

#=============================

PROD_IOC = linkinfo

linkinfo_SRCS += devLinkinfo.c

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
linkinfo_SRCS += linkinfo_registerRecordDeviceDriver.cpp
linkinfo_SRCS_DEFAULT += linkinfoMain.cpp
linkinfo_SRCS_vxWorks += -nil-

#The following adds support from base/src/vxWorks
linkinfo_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

linkinfo_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

