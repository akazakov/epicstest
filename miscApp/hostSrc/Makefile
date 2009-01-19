TOP=../..

include $(TOP)/configure/CONFIG

ca_DIR  = $(EPICS_BASE_LIB)
Com_DIR = $(EPICS_BASE_LIB)

PROD_HOST += testhost
PROD_HOST += errStatusTest
errStatusTest_SRCS += errStatusTestMain.cpp errStatusTest.c
PROD_HOST += cvtFastTest

iocLogServer_LIBS += ca Com
testhost_LIBS += ca Com
errStatusTest_LIBS += ca Com
cvtFastTest_LIBS += ca Com

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
errStatusTest.c:
	$(CP) ../../src/errStatusTest.c .
