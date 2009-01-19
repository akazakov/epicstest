TOP = ../..
include $(TOP)/configure/CONFIG_APP

TARGETS += ../convertClient.db
TARGETS += ../convertClientENUM.db
TARGETS += ../convertMaster.db
TARGETS += ../convertMasterENUM.db
 
include $(TOP)/configure/RULES

../convertClient.db: $(INSTALL_HOST_BIN)/convertClientCreateDb$(HOSTEXE)
	$(INSTALL_HOST_BIN)/convertClientCreateDb$(HOSTEXE) > $@

../convertClientENUM.db: $(INSTALL_HOST_BIN)/convertClientENUMCreateDb$(HOSTEXE)
	$(INSTALL_HOST_BIN)/convertClientENUMCreateDb$(HOSTEXE) > $@

../convertMaster.db: $(INSTALL_HOST_BIN)/convertClientENUMCreateDb$(HOSTEXE)
	$(INSTALL_HOST_BIN)/convertMasterCreateDb$(HOSTEXE) > $@

../convertMasterENUM.db: $(INSTALL_HOST_BIN)/convertMasterENUMCreateDb$(HOSTEXE)
	$(INSTALL_HOST_BIN)/convertMasterENUMCreateDb$(HOSTEXE)  > $@

clean::
	@$(RM) ../convertClient.db
	@$(RM) convertClient.db
	@$(RM) ../convertClientENUM.db
	@$(RM) convertClientENUM.db
	@$(RM) ../convertMaster.db
	@$(RM) convertMaster.db
	@$(RM) ../convertMasterENUM.db
	@$(RM) convertMasterENUM.db
