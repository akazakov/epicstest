# Example vxWorks startup file
< cdCommands
< ../nfsCommands

cd topbin
ld < linkinfo.munch

cd top
dbLoadDatabase("dbd/linkinfo.dbd")
linkinfo_registerRecordDeviceDriver(pdbbase)
dbLoadRecords("demoApp/Db/demo.db")
dbLoadRecords("linkinfoApp/Db/linkinfoLocal.db")
cd startup
iocInit
dbpf "mrkbi1","1"
