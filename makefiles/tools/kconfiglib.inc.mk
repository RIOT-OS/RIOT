# Define tools to use
MENUCONFIG ?= $(RIOTTOOLS)/kconfiglib/riot_menuconfig.py
BASE_MENUCONFIG ?= $(RIOTTOOLS)/kconfiglib/menuconfig.py
GENCONFIG := $(RIOTTOOLS)/kconfiglib/genconfig.py

ifeq ($(RIOT_CI_BUILD),1)
  QUIETMESSAGES=\#
endif
$(BASE_MENUCONFIG):
	@$(QUIETMESSAGES) echo "[INFO] Kconfiglib not found - getting it"
	@$(MAKE) -C $(RIOTTOOLS)/kconfiglib > /dev/null
	@$(QUIETMESSAGES) echo "[INFO] Kconfiglib downloaded"


$(GENCONFIG): $(BASE_MENUCONFIG)
