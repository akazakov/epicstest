TOP = ../..
include $(TOP)/configure/CONFIG_APP
 
TARGETS += ../client.db
TARGETS += ../master.db
NRECORDS = 500

include $(TOP)/configure/RULES

../client.db: $(INSTALL_HOST_BIN)/createIocClient$(HOSTEXE)
	$(RM) $@
	$(INSTALL_HOST_BIN)/createIocClient$(HOSTEXE) $(NRECORDS)  > $@

../master.db: $(INSTALL_HOST_BIN)/createIocMaster$(HOSTEXE)
	$(RM) $@
	$(INSTALL_HOST_BIN)/createIocMaster$(HOSTEXE)  $(NRECORDS) > $@

clean::
	@$(RM) ../client.db
	@$(RM) ../master.db
	@$(RM) client.db
	@$(RM) master.db
