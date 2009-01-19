# Example vxWorks startup file
< cdCommands
< ../nfsCommands
cd topbin

ld < commonVx.munch
cd startup
dbLoadDatabase("../../dbd/commonVx.dbd")
commonVx_registerRecordDeviceDriver(pdbbase)
dbLoadRecords("../../alarmApp/Db/alarm.db")
iocInit
