# Example vxWorks startup file
< cdCommands
< ../nfsCommands
cd topbin

ld < commonVx.munch
cd startup
dbLoadDatabase("../../dbd/commonVx.dbd")
commonVx_registerRecordDeviceDriver(pdbbase)

# This has 5000 records in it, so it won't be quick:
dbLoadRecords("../../caApp/Db/catest.db")
tickGet()/60
memShow
iocInit
tickGet()/60
memShow

