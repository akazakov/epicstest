# Example vxWorks startup file
< cdCommands
< ../nfsCommands

cd topbin
ld < perform.munch

cd startup
dbLoadDatabase("../../dbd/perform.dbd")
perform_registerRecordDeviceDriver(pdbbase)
dbLoadRecords("../../performApp/Db/perform.db")

cd startup
iocInit
