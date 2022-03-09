# Define tools to use
FIXDEP_DIR ?= $(RIOTTOOLS)/fixdep
FIXDEP ?= $(FIXDEP_DIR)/fixdep

$(FIXDEP):
	@echo "[INFO] fixdep not found - building it"
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(FIXDEP_DIR)
