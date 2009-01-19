TOP=../..

include $(TOP)/configure/CONFIG

DBD += commonVx.dbd
commonVx_DBD += base.dbd
commonVx_DBD += devTestAsyn.dbd
commonVx_DBD += aiAsyn.dbd

PROD_vxWorks += commonVx

commonVx_SRCS += devAiAsyn.c
commonVx_SRCS += commonVx_registerRecordDeviceDriver.cpp

ifdef VXSTATS
  commonVx_DBD += vxStats.dbd
  commonVx_LIBS += vxStats
endif

commonVx_LIBS += testDevIoc
commonVx_LIBS += $(EPICS_BASE_IOC_LIBS)

vxStats_DIR = $(VX_STATS_LIB)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
