TOP=../..

include $(TOP)/configure/CONFIG

ifdef EZCA
  ezca_DIR = $(EZCA_LIB)

  PROD_HOST += ezcatest
  ezcatest_SRCS += ezcatest.c
  ezcatest_LIBS += ezca
  ezcatest_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

PROD_HOST += testputnotify
testputnotify_SRCS += testputnotify.c
testputnotify_LIBS += $(EPICS_BASE_HOST_LIBS)

PROD_HOST += testsyncgroup
testsyncgroup_SRCS += testsyncgroup.c
testsyncgroup_LIBS += $(EPICS_BASE_HOST_LIBS)

PROD_HOST += caPutCallback
caPutCallback_SRCS += caPutCallback.c
caPutCallback_LIBS += $(EPICS_BASE_HOST_LIBS)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
