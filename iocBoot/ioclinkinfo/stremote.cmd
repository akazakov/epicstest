# Example vxWorks startup file
< cdCommands
< ../nfsCommands

cd topbin
ld < linkinfo.munch

cd top
dbLoadDatabase("dbd/linkinfo.dbd")
linkinfo_registerRecordDeviceDriver(pdbbase)
dbLoadRecords("linkinfoApp/Db/linkinfoRemote.db")
cd startup
iocInit

