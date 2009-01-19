# Example vxWorks startup file
< cdCommands
< ../nfsCommands

cd topbin
ld < misc.munch
putenv("EPICS_IOC_LOG_PORT=7004")
putenv("EPICS_IOC_LOG_INET=164.54.8.12")
errlogPrintf("This is a test to see if message gets logged\n");

cd top
dbLoadDatabase("dbd/misc.dbd")
misc_registerRecordDeviceDriver(pdbbase)
dbLoadRecords("miscApp/Db/testLinearConvert.db")

cd startup
iocInit
iocLogInit
