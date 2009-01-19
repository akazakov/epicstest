TOP=../..

include $(TOP)/configure/CONFIG

PROD_HOST += convertMasterCreateDb
PROD_HOST += convertClientCreateDb
PROD_HOST += convertMasterENUMCreateDb
PROD_HOST += convertClientENUMCreateDb
PROD_HOST += testConvert
PROD_HOST += testConvertENUM

testConvert_LIBS += ca Com
testConvertENUM_LIBS += ca Com

DBDINC += typeRecord.h
DBD += convert.dbd

PROD_IOC += convert

convert_SRCS += typeRecord.c

convert_SRCS += convert_registerRecordDeviceDriver.cpp
convert_SRCS_DEFAULT += convertMain.cpp
convert_SRCS_vxWorks += -nil-

convert_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
