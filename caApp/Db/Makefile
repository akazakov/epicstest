TOP = ../..
include $(TOP)/configure/CONFIG_APP

TARGETS += ../catest.db
NRECORDS = 5000
 
include $(TOP)/configure/RULES

../catest.db: $(INSTALL_HOST_BIN)/createdb$(HOSTEXE)
	$(RM) $@
	$(INSTALL_HOST_BIN)/createdb$(HOSTEXE) $(NRECORDS) > $@

clean::
	@$(RM) ../catest.db
	@$(RM) catest.db
