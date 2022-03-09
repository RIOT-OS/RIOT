# Define tools to use
MENUCONFIG ?= $(RIOTTOOLS)/kconfiglib/riot_menuconfig.py
BASE_MENUCONFIG ?= $(RIOTTOOLS)/kconfiglib/menuconfig.py
GENCONFIG := $(RIOTTOOLS)/kconfiglib/genconfig.py

$(BASE_MENUCONFIG):
	@echo "[INFO] Kconfiglib not found - getting it"
	@$(MAKE) -C $(RIOTTOOLS)/kconfiglib
	@echo "[INFO] Kconfiglib downloaded"

$(GENCONFIG): $(BASE_MENUCONFIG)
