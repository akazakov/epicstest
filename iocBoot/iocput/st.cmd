# Example vxWorks startup file
< cdCommands
< ../nfsCommands

cd topbin
ld < commonVx.munch

cd top
dbLoadDatabase("dbd/commonVx.dbd")
commonVx_registerRecordDeviceDriver(pdbbase)
dbLoadRecords("putApp/Db/cachenotify.db")
dbLoadRecords("putApp/Db/ezcaNotify.db")
dbLoadRecords("putApp/Db/notifyDuration.db")

cd startup
iocInit
